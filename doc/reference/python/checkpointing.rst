.. _reference/python/checkpointing:

Checkpointing
*************

HDF5-based checkpointing for DMFT calculations. ModEST exposes two
classes:

* :py:class:`~triqs_modest.utils.file_io.CheckpointBase` — the
  ``c2py``-wrapped C++ checkpoint manager. Use it directly for pure
  C++ workflows or single-rank Python scripts.
* :py:class:`~triqs_modest.utils.checkpoint.Checkpointer` — a
  Python wrapper around ``CheckpointBase`` that is safe to call on all
  MPI ranks, supports a create-or-open constructor with optional initial
  data, accepts arbitrary keyword extras via :py:class:`h5.HDFArchive`,
  and exposes the convergence-log helper ``summarize()``.

.. autosummary::

   triqs_modest.utils.file_io.IterationData
   triqs_modest.utils.file_io.CheckpointBase
   triqs_modest.utils.checkpoint.Checkpointer

Iteration data
==============

Each iteration stored in the checkpoint is an
:py:class:`~triqs_modest.utils.file_io.IterationData` carrying the
restart-required fields and several optional diagnostic / CSC fields:

* **Restart payload** (always populated):
  ``mu``, ``Sigma_imp_list``, ``Sigma_hartree_list``, ``Sigma_dc_list``.
  ``Sigma_dc_list`` is ``[]`` for non-CSC runs and populated for CSC.
* **Solver outputs** (optional):
  ``Gimp_list`` — impurity Green's functions returned by the solver.
* **Solver inputs** (optional):
  ``Gloc_list`` — local Green's function fed to the solver;
  ``Delta_list`` — hybridization function fed to the solver.

Optional fields default to empty lists. Simple DMFT runs can ignore
them; CSC and machine-learning workflows fill them in.

The on-disk HDF5 layout reflects this split: each iteration is written
under ``<name>/inputs/{Gloc_list, Delta_list}`` and
``<name>/outputs/{Gimp_list, Sigma_imp_list, Sigma_hartree_list,
Sigma_dc_list}``.

Diagnostics
===========

:py:meth:`~triqs_modest.utils.checkpoint.Checkpointer.summarize` prints
a per-iteration convergence log to ``stdout`` (or a user-supplied
stream). Columns are ``iter``, ``mu``, and the orbital-resolved diagonal
occupations from ``Gimp_list`` and ``Gloc_list``. Column groups are
skipped if no iteration has the corresponding list populated, and
``--`` placeholders appear for iterations whose individual lists are
empty. The call is a no-op on non-master ranks.
