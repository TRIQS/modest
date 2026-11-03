"""
Checkpointer — an MPI-aware checkpoint manager for DMFT calculations.

Wraps CheckpointBase (the c2py-exposed C++ class) and adds:
- Smart create-or-open constructor, safe on all MPI ranks
- Optional initial data stored via HDFArchive (master only)
- Keyword-extras written alongside required iteration data (master only)
- restart() broadcasts last iteration to all ranks (collective)
- Full Python iteration, indexing, and len() support
- summarize() prints a convergence log of mu and orbital-resolved occupations
"""

import os
import sys
import numpy as np
import triqs.utility.mpi as mpi
from h5 import HDFArchive
from .file_io import CheckpointBase, IterationData

__all__ = ["Checkpointer", "IterationData"]


class Checkpointer:
    """MPI-aware checkpoint manager for DMFT calculations.

    Stores per iteration the restart-required self-energies (``mu``,
    ``Sigma_imp_list``, ``Sigma_hartree_list``) along with optional inputs
    (``Gloc_list``, ``Delta_list``) and outputs (``Gimp_list``,
    ``Sigma_dc_list``) of the impurity solve. The optional fields default
    to empty lists — simple DMFT runs can ignore them; CSC and ML workflows
    fill them in. Anything else can still be passed through ``**extras``
    and written to the same HDF5 group via ``HDFArchive``.

    All methods are safe to call on all MPI ranks — no ``is_master_node()`` guards
    required in user code. Only the master process performs HDF5 file I/O.

    Parameters
    ----------
    dirname : str
        Path to the checkpoint directory.
    initial_data : dict, optional
        Data to store in initial_data.h5 (e.g. ``{"obe": obe, "embedding": E}``).
        Ignored if dirname already exists (i.e., when reopening).

    Examples
    --------
    Create or resume, then run the DMFT loop (all ranks):

    .. code-block:: python

        chkpt = Checkpointer("my_calc", initial_data={"obe": obe, "Embedding": E})
        last  = chkpt.restart()              # collective — data on all ranks
        if last is not None:
            Sigma_dyn    = last.Sigma_imp_list[0]
            Sigma_static = last.Sigma_hartree_list[0]
            Sigma_dc     = last.Sigma_dc_list[0] if last.Sigma_dc_list else None
        else:
            Sigma_dyn, Sigma_static = make_zero_self_energies(mesh)
            Sigma_dc = None

        it_shift = len(chkpt)

        for n in range(it_shift, n_loops):
            ...
            chkpt.append(IterationData(
                mu=mu,
                Sigma_imp_list=[Sigma_dyn], Sigma_hartree_list=[Sigma_static],
                Gimp_list=[Gimp], Sigma_dc_list=[Sigma_dc] if Sigma_dc is not None else [],
                Gloc_list=[Gloc], Delta_list=[Delta],
            ))

        chkpt.summarize()                    # print a convergence log
    """

    def __init__(self, dirname, initial_data=None) -> None:
        self._dirname = dirname
        self._cpp = None
        if mpi.is_master_node():
            self._cpp = CheckpointBase(dirname)
            if initial_data is not None and len(self._cpp) == 0:
                with HDFArchive(os.path.join(dirname, "initial_data.h5"), "w") as ar:
                    for key, value in initial_data.items():
                        ar[key] = value
        return None

    def append(self, iteration_data, **extras) -> None:
        """Append an iteration to the checkpoint.

        Safe on all MPI ranks — non-master ranks are silently skipped.

        Parameters
        ----------
        iteration_data : IterationData
            Required restart data: mu, Sigma_imp_list, Sigma_hartree_list.
        **extras
            Additional named quantities written to the same HDF5 group via
            HDFArchive. Supports any TRIQS-serializable type (block_gf, matrix,
            scalar, etc.).
        """
        if mpi.is_master_node():
            current_iter = len(self._cpp)
            self._cpp.append(iteration_data)
            if extras:
                with HDFArchive(os.path.join(self._dirname, "iterations.h5"), "a") as ar:
                    for key, value in extras.items():
                        ar[str(current_iter)][key] = value

    def restart(self) -> IterationData|None:
        """Return last iteration data on all MPI ranks, or None if empty.

        Collective operation — all ranks must call this together. Master reads
        from disk; the returned ``IterationData`` is broadcast to all ranks.

        Carries the restart-relevant fields: ``mu``, ``Sigma_imp_list``,
        ``Sigma_hartree_list``, and ``Sigma_dc_list``. The DC list is ``[]``
        for non-CSC runs and populated for CSC; callers can detect with
        ``if it.Sigma_dc_list:``. Diagnostic fields (``Gimp_list``,
        ``Gloc_list``, ``Delta_list``) are not in the restart payload — read
        them via ``chkpt[i]`` on master or via ``summarize()``.

        Returns
        -------
        IterationData or None
            Last stored iteration on all ranks, None if no iterations exist.
        """
        n_iter = None
        if mpi.is_master_node(): n_iter = len(self._cpp)
        n_iter = mpi.bcast(n_iter)
        if n_iter == 0: return None

        iter_data = None
        if mpi.is_master_node():
            last = self._cpp[-1]
            iter_data = IterationData(
                mu=last.mu,
                Sigma_imp_list=last.Sigma_imp_list,
                Sigma_hartree_list=last.Sigma_hartree_list,
                Sigma_dc_list=last.Sigma_dc_list,
            )
        iter_data = mpi.bcast(iter_data)
        return iter_data


    def summarize(self, file=None) -> None:
        """Print a per-iteration convergence log (master only).

        Columns are ``iter``, ``mu``, and the orbital-resolved diagonal
        occupations from ``Gimp_list`` and ``Gloc_list``. Column groups are
        skipped if no iteration in the checkpoint has the corresponding list
        populated. Iterations whose individual lists are empty show ``--``
        placeholders.

        Parameters
        ----------
        file : file-like, optional
            Destination stream; defaults to ``sys.stdout``.
        """
        if not mpi.is_master_node(): return
        if file is None: file = sys.stdout

        n_iter = len(self._cpp)
        if n_iter == 0:
            print("Checkpointer: no iterations stored.", file=file)
            return

        iters = [self._cpp[n] for n in range(n_iter)]

        def orbital_layout(blocklist):
            """Return [(imp_idx, block_name, orb_idx), ...] from the first non-empty list, or []."""
            for it in iters:
                lst = blocklist(it)
                if lst:
                    layout = []
                    for i, bgf in enumerate(lst):
                        for bn, g in bgf:
                            for a in range(g.target_shape[0]):
                                layout.append((i, bn, a))
                    return layout
            return []

        imp_layout = orbital_layout(lambda it: it.Gimp_list)
        loc_layout = orbital_layout(lambda it: it.Gloc_list)

        headers = ["iter", "mu"]
        headers += [f"n_imp{i}_{bn}_{a}_imp" for (i, bn, a) in imp_layout]
        headers += [f"n_imp{i}_{bn}_{a}_loc" for (i, bn, a) in loc_layout]
        widths = [max(len(h), 12) for h in headers]

        def fmt_row(cells):
            return "  ".join(f"{c:>{w}}" for c, w in zip(cells, widths))

        print(fmt_row(headers), file=file)
        print("  ".join("-" * w for w in widths), file=file)

        def diag_occupations(blocklist, layout):
            if not blocklist:
                return ["--"] * len(layout)
            diags = {}
            for i, bgf in enumerate(blocklist):
                for bn, g in bgf:
                    diags[(i, bn)] = np.diag(g.density()).real
            return [f"{diags[(i, bn)][a]:.6f}" for (i, bn, a) in layout]

        for n, it in enumerate(iters):
            row = [str(n), f"{it.mu:.6f}"]
            row += diag_occupations(it.Gimp_list, imp_layout)
            row += diag_occupations(it.Gloc_list, loc_layout)
            print(fmt_row(row), file=file)


    @property
    def dirname(self) -> str:
        """Path to the checkpoint directory."""
        return self._dirname

    def __getitem__(self, i) -> IterationData:
        """Index access (master only). Returns None on non-master ranks."""
        if mpi.is_master_node(): return self._cpp[i]
        return None

    def __len__(self) -> int:
        """Number of stored iterations on master, 0 on non-master.

        Use ``mpi.bcast(len(chkpt))`` to obtain the count on all ranks.
        """
        n_iter = None
        if mpi.is_master_node(): n_iter = len(self._cpp)
        return mpi.bcast(n_iter)

    def __iter__(self):
        for i in range(len(self)):
            yield self[i]

    def __repr__(self):
        if mpi.is_master_node():
            n = len(self._cpp)
            return f"Checkpointer('{self._dirname}', n_iter={n})"
