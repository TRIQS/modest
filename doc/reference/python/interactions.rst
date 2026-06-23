.. _reference/python/interactions:

Interaction Hamiltonians
************************

ModEST provides factory functions that build a local interaction
Hamiltonian as a TRIQS many-body operator on the flavors of the
impurity problem. The generic form is

.. math::

   H_{\mathrm{int}} =
   \frac{1}{2} \sum_{\substack{\alpha\beta\gamma\delta \\ \sigma\sigma'}}
   U_{\alpha\beta\gamma\delta}\;
   c^{\dagger}_{\alpha\sigma}\, c^{\dagger}_{\beta\sigma'}\,
   c_{\delta\sigma'}\, c_{\gamma\sigma},

where :math:`\alpha,\beta,\gamma,\delta` index the orbitals of the
impurity, :math:`\sigma,\sigma'` are spin (or block-diagonal) indices,
and :math:`U_{\alpha\beta\gamma\delta}` is the matrix of two-particle
interactions. The factories below differ in how
:math:`U_{\alpha\beta\gamma\delta}` is parametrized — density–density
only, Kanamori, or full rotationally-invariant Slater — and return a
``triqs.operators.Operator`` ready to be handed to a TRIQS impurity
solver.

.. autosummary::

   triqs_modest.hamiltonians.make_density_density
   triqs_modest.hamiltonians.make_kanamori
   triqs_modest.hamiltonians.make_slater
