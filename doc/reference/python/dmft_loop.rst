.. _reference/python/dmft_loop:

DMFT loop
*********

Typical quantities computed at each DMFT iteration: local Green's
function, chemical-potential search, impurity levels, and hybridization
function.

Local Green's function
======================

The local Green's function on the correlated subspace
:math:`\mathcal{C}` is obtained by k-summing the lattice Green's
function projected with :math:`P(\mathbf{k})`:

.. math::

   G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}(\omega)
   = \sum_{\mathbf{k}} P(\mathbf{k})\,
     \bigl[(\omega + \mu)\,\mathbb{1} - H^{\sigma}(\mathbf{k})
       - P^{\dagger}(\mathbf{k})\,
         \bigl(\Sigma_{\mathrm{imp}}^{\sigma}(\omega)
               - \Sigma_{\mathrm{DC}}^{\sigma}\bigr)\,
         P(\mathbf{k})\bigr]^{-1}\,
     P^{\dagger}(\mathbf{k}).

ModEST evaluates this in :math:`\mathcal{C}` via the Woodbury-reduced
form :eq:`eq_DysonW`.

.. autosummary::

   triqs_modest.local_gf.gloc

Chemical-potential search
=========================

The chemical potential is determined by a root-finding problem on the
total electron density:

.. math::

   \mu_{\star} :\quad
   n(\mu_{\star}) - n_{\mathrm{target}} = 0,
   \qquad
   n(\mu) = \sum_{\sigma}\sum_{\mathbf{k}} n^{\sigma}(\mathbf{k};\,\mu),

where :math:`n^{\sigma}(\mathbf{k};\,\mu)` is the lattice density of
:eq:`eq_deflattdensity` (computed efficiently in :math:`\mathcal{C}`
through :eq:`eq_densityW`) and :math:`n_{\mathrm{target}}` is the target
filling. ModEST exposes several algorithms (bisection, Brent, …) and is
modular enough that an external root finder can be plugged in.

.. autosummary::

   triqs_modest.rho_and_mu.find_chemical_potential
   triqs_modest.rho_and_mu.density
   triqs_modest.rho_and_mu.density_nk

Impurity levels and hybridization function
==========================================

The impurity-level matrix :math:`E_{\mathrm{imp}}` is read off from the
high-frequency expansion of the local Green's function and includes the
double-counting correction:

.. math::

   E_{\mathrm{imp}}
   = \langle H^{\sigma}(\mathbf{k}) \rangle_{\mathbf{k}}
   - \mu
   - \Sigma_{\mathrm{DC}}^{\sigma}.

The hybridization function is then defined by the Dyson equation on the
impurity,

.. math::

   \Delta^{\sigma}(\omega)
   = \omega\,\mathbb{1} - E_{\mathrm{imp}}
     - \bigl[G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}(\omega)\bigr]^{-1}
     - \Sigma_{\mathrm{imp}}^{\sigma}(\omega).

The derivation of :math:`E_{\mathrm{imp}}` and the role of
:math:`\Sigma_{\mathrm{DC}}^{\sigma}` are spelled out in
:ref:`reference/python/double_counting`.

.. autosummary::

   triqs_modest.atomic_levels_and_delta.impurity_levels
   triqs_modest.atomic_levels_and_delta.hybridization
