.. _reference/python/one_body_elements:

One-body elements
*****************

The generic representation of DFT (or Wannier90) data used as one of the primary inputs to
a DMFT calculation: factory functions that read from each supported source,
the container types they produce, and the supporting types that describe the
correlated subspace and the downfolding from Bloch space.

.. note::

   The HDF5 input files consumed by
   :func:`~triqs_modest.obe.one_body_elements_from_dft_converter` are
   produced by the DFT-code-specific converters in
   `dftkit <https://github.com/TRIQS/dftkit>`_, which translate output from
   VASP, Wien2k, and Quantum ESPRESSO into the common archive layout that
   ModEST reads.

Factories
=========

.. autosummary::

   triqs_modest.obe.one_body_elements_from_dft_converter
   triqs_modest.obe.one_body_elements_on_high_symmetry_path
   triqs_modest.obe.one_body_elements_with_partial_projectors
   triqs_modest.obe.make_one_body_elements_gw
   triqs_modest.obe_tb.one_body_elements_from_wannier90

Containers
==========

.. autosummary::

   triqs_modest.obe.OneBodyElementsOnGrid
   triqs_modest.obe_tb.OneBodyElementsTb
   triqs_modest.obe.OneBodyElementsGw

Supporting types
================

.. autosummary::

   triqs_modest.obe.BandDispersion
   triqs_modest.obe.DownfoldingProjector
   triqs_modest.obe.DownfoldingProjectorExt
   triqs_modest.obe.LocalSpace
   triqs_modest.obe.AtomicOrbs
   triqs_modest.obe.IbzSymmetryOps

Operations
==========

Apply a unitary rotation :math:`U` to the local correlated basis of an
existing :py:class:`~triqs_modest.obe.OneBodyElementsOnGrid` (or its
:py:class:`~triqs_modest.obe.IbzSymmetryOps`). The projector
:math:`P(\mathbf{k})` is transformed as in :eq:`eq_ChangeBasisP`.

.. autosummary::

   triqs_modest.obe.rotate_local_basis
