"""
Checkpointer — an MPI-aware checkpoint manager for DMFT calculations.

Wraps CheckpointBase (the c2py-exposed C++ class) and adds:
- Smart create-or-open constructor, safe on all MPI ranks
- Optional initial data stored via HDFArchive (master only)
- Keyword-extras written alongside required iteration data (master only)
- restart() broadcasts last iteration to all ranks (collective)
- Full Python iteration, indexing, and len() support
"""

import os
import triqs.utility.mpi as mpi
from h5 import HDFArchive
from .file_io import CheckpointBase, IterationData

__all__ = ["Checkpointer", "IterationData"]


class Checkpointer:
    """MPI-aware checkpoint manager for DMFT calculations.

    Enforces saving the minimal restart set (mu, Sigma_imp_list, Sigma_hartree_list)
    per iteration and optionally stores additional quantities (Green's functions,
    density matrices, etc.) in the same HDF5 group via keyword arguments.

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

        chkpt    = Checkpointer("my_calc", initial_data={"obe": obe, "Embedding" : E})
        last     = chkpt.restart()          # collective — data on all ranks
        if last is not None:
            Sigma_dyn    = last.Sigma_imp_list[0]
            Sigma_static = last.Sigma_hartree_list[0]
        else:
            Sigma_dyn, Sigma_static = make_zero_self_energies(mesh)

        it_shift = len(chkpt)

        for n in range(it_shift, n_loops):
            ...
            chkpt.append(
                IterationData(mu=mu, Sigma_imp_list=[Sigma_dyn],
                              Sigma_hartree_list=[Sigma_static]),
                Delta_iw=Delta, Gimp_iw=Gimp, dm=dm,
            )
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
        from disk; components (mu, Sigma_imp_list, Sigma_hartree_list) are
        broadcast individually to all ranks.

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
            mu             = self._cpp[-1].mu                
            Sigma_imp_list = self._cpp[-1].Sigma_imp_list    
            Sigma_H_list   = self._cpp[-1].Sigma_hartree_list
            iter_data = IterationData(mu=mu, Sigma_imp_list=Sigma_imp_list, Sigma_hartree_list=Sigma_H_list)
        iter_data = mpi.bcast(iter_data)
        return iter_data


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
