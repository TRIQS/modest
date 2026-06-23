.. _reference/python/obe_utils:

OBE utilities
*************

It is often useful to manipulate the data stored in an OBE container —
e.g. to add a local term to the tight-binding Hamiltonian, to extend a
non-spin-polarized OBE into a spin-resolved one, to fold a
:math:`\mathbf{k}`-dependent Hamiltonian to a coarser grid, or to apply
a unitary rotation to the local basis. The helpers below operate
directly on :py:class:`triqs_modest.obe_tb.OneBodyElementsTb` and
related containers.

.. autosummary::

   triqs_modest.obe_tb.add_local_term
   triqs_modest.obe_tb.extend_to_spin
   triqs_modest.obe_tb.fold
   triqs_modest.obe_tb.rotate
