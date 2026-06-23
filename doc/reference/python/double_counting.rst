.. _reference/python/double_counting:

Double counting
***************

Routines for computing the double-counting correction
:math:`\Sigma_{\mathrm{DC}}` that (approximately) removes the
local-interaction contribution already present in the DFT
exchange-correlation functional.

Embedded self-energy
====================

The double-counting correction enters DMFT through the **embedded
self-energy**: it is the impurity self-energy minus the double-counting
term that is embedded back to the lattice,

.. math::

   \Sigma_{\mathrm{embed}}^{\sigma}(\omega)
   = \mathrm{embed}\bigl(\Sigma_{\mathrm{imp}}^{\sigma}(\omega)
                         - \Sigma_{\mathrm{DC}}^{\sigma}\bigr).

The subtraction happens **before** embedding, so
:math:`\Sigma_{\mathrm{DC}}^{\sigma}` enters with the same projector
structure as :math:`\Sigma_{\mathrm{imp}}^{\sigma}`.

.. autosummary::

   triqs_modest.utils.dc.DcSolver
   triqs_modest.utils.dc.dc_formulas

Where does the double-counting term appear in the DMFT loop
===========================================================

The DMFT self-consistency condition

.. math::

   G_{\mathrm{loc}} \;\stackrel{!}{=}\; G_{\mathrm{imp}},

reads, when both sides are written explicitly,

.. math::
   :label: eq_dc_selfconsistency

   \sum_{\mathbf{k}} P(\mathbf{k})\,
   \bigl[i\omega_n + \mu - H(\mathbf{k})
         - P^{\dagger}\bigl(\Sigma_{\mathrm{imp}}(i\omega_n)
                              - \Sigma_{\mathrm{DC}}\bigr) P\bigr]^{-1}\,
   P^{\dagger}(\mathbf{k})
   =
   \frac{1}{i\omega_n - E_{\mathrm{imp}} - \Delta(i\omega_n)
            - \Sigma_{\mathrm{imp}}(i\omega_n)}.

When we embed the self-energy back into the lattice it is important
that the double-counting term is subtracted from the impurity
self-energy.

The impurity-level definition follows from taking the high-frequency
expansion of both sides of :eq:`eq_dc_selfconsistency`,

.. math::

   \mu - \langle H(\mathbf{k}) \rangle_{\mathbf{k}}
   - \Sigma_{\infty} + \Sigma_{\mathrm{DC}}
   = - E_{\mathrm{imp}} - \Sigma_{\infty},

and solving for :math:`E_{\mathrm{imp}}` gives

.. math::
   :label: eq_dc_eimp

   E_{\mathrm{imp}} = \langle H(\mathbf{k}) \rangle_{\mathbf{k}}
                       - \mu - \Sigma_{\mathrm{DC}}.

The double-counting term therefore appears in **two** places in the
DMFT loop:

#. **The embedded self-energy**,
   :math:`\Sigma_{\mathrm{embed}} = \Sigma_{\mathrm{imp}} - \Sigma_{\mathrm{DC}}`,
   which is what gets upfolded to the lattice through :math:`P`.
#. **The local impurity levels**,
   :math:`E_{\mathrm{imp}} = \langle H(\mathbf{k}) \rangle_{\mathbf{k}}
   - \mu - \Sigma_{\mathrm{DC}}`,
   which enter the hybridization function via
   :math:`\Delta(\omega) = \omega - E_{\mathrm{imp}}
   - G_{\mathrm{loc}}^{-1}(\omega) - \Sigma_{\mathrm{imp}}(\omega)`.
