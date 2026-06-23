.. _reference/python/csc:

Charge self-consistency
***********************

The DFT-driver framework and the charge-density correction that close
the charge self-consistent DFT+DMFT (CSC) loop. This is the generic DFT
driver framework that abstracts the DFT electronic-structure code from
ModEST.

For more details on specific drivers of DFT electronic structure codes,
please see `TRIQS/dftkit <https://github.com/TRIQS/dftkit>`_.

Generic DFT driver framework
============================

.. autosummary::

   triqs_modest.dft_driver.DftDriver

Charge-density correction
=========================

The charge-density correction is the band-resolved density matrix of
the interacting (DMFT-corrected) Green's function, expressed in the
Kohn–Sham basis:

.. math::

   N_{\nu\nu'}^{\sigma}(\mathbf{k})
   = \frac{1}{\beta}\sum_{n}\,
     \bigl[G_{\mathcal{B}}^{\sigma}(\mathbf{k}, i\omega_n)\bigr]_{\nu\nu'}\,
     e^{i\omega_n 0^{+}},

with :math:`G_{\mathcal{B}}^{\sigma}` the lattice Green's function of
:eq:`eq_lattG`. In a CSC iteration, ModEST hands the deviation from
the Kohn–Sham density,

.. math::

   \Delta N_{\nu\nu'}^{\sigma}(\mathbf{k})
   = N_{\nu\nu'}^{\sigma}(\mathbf{k})
     - n_{F}\bigl(\varepsilon_{\nu}^{\sigma}(\mathbf{k}) - \mu\bigr)\,
       \delta_{\nu\nu'},

back to the DFT code as the correction to the KS density.

.. autosummary::

   triqs_modest.local_gf.charge_density_correction
