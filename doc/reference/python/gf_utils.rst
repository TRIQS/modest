.. _reference/python/gf_utils:

Green's function utilities
**************************

Helpers for detecting and enforcing (symmetrizing) the block structure
of Green's functions. Typically used to symmetrize impurity-solver
output before feeding it into the next DMFT iteration.

Analyzing degenerate blocks
===========================

:py:func:`analyze_degenerate_blocks <triqs_modest.utils.analyze_gfs.analyze_degenerate_blocks>`
scans the matrix-valued blocks of a block Green's function and
partitions them into *degenerate groups* — sets of blocks that are
numerically equal (up to a user-set threshold) when compared at
:math:`\tau = 0` or :math:`i\omega_0`. This decomposes the Green's
function into smaller, independent blocks so that the TRIQS impurity
solver can treat each unique block on its own rather than carrying
redundant copies through the calculation.

Symmetrizing a block Green's function
=====================================

Given a partition :math:`\{ \mathcal{D}_g \}` of degenerate blocks
returned by ``analyze_degenerate_blocks``,
:py:func:`symmetrize <triqs_modest.utils.analyze_gfs.symmetrize>`
replaces every block in a group by the average over the group:

.. math::

   \bar{G}_{b}(\omega)
   = \frac{1}{|\mathcal{D}_g|}
     \sum_{b' \in \mathcal{D}_g} G_{b'}(\omega),
   \qquad
   G_{b}(\omega) \;\leftarrow\; \bar{G}_{b}(\omega)
   \quad \forall\, b \in \mathcal{D}_g.

This enforces the degeneracy exactly, removing any residual numerical
breaking introduced by the impurity solver.

.. autosummary::

   triqs_modest.utils.analyze_gfs.analyze_degenerate_blocks
   triqs_modest.utils.analyze_gfs.symmetrize
