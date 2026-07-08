.. _userguide_transport:

Kubo formalism: optics and transport
**********************************

This page discusses the Kubo formalism for optics and transport in ModEST.
Here, we cover the transport distribution
:math:`\Gamma_{\alpha\beta}(\omega,\Omega)`, the transport function
:math:`\Phi_{\alpha\beta}(\omega)`, and the Onsager (kinetic) integrals that yield the
optical conductivity, Seebeck coefficient, and electronic thermal
conductivity, together with the **choice of units** and the units of every
computed object.

It builds on the notation of :ref:`userguide_theory_and_notation` (Bloch space
:math:`\mathcal{B}`, correlated space :math:`\mathcal{C}`, the :math:`\sigma`
block index, and the projectors :math:`P^{\sigma}_{m\nu}(\mathbf{k})`).

The transport distribution (Kubo bubble)
========================================

The central object is the bubble term of the current–current correlation
function, the *transport distribution*

.. math::

   \Gamma_{\alpha\beta}(\omega,\Omega)
   = \frac{1}{V} \sum_{\mathbf{k}} w_{\mathbf{k}}\,
     \mathrm{Tr}\!\left[\, v_{\mathbf{k},\alpha}\,
       A_{\mathbf{k}}(\omega+\Omega)\,
       v_{\mathbf{k},\beta}\,
       A_{\mathbf{k}}(\omega) \right],

where :math:`\alpha,\beta \in \{x,y,z\}` are Cartesian directions,
:math:`V` is the primitive unit-cell volume, :math:`w_{\mathbf{k}}` are the
:math:`\mathbf{k}`-point weights, and the trace runs over the band index.
:math:`A_{\mathbf{k}}(\omega)` is the band-basis spectral function

.. math::

   A_{\mathbf{k}}(\omega)
   = \frac{i}{2\pi}\left[ G_{\mathbf{k}}(\omega) - G_{\mathbf{k}}^{\dagger}(\omega) \right]
   = -\frac{1}{\pi}\,\mathrm{Im}\,G_{\mathbf{k}}(\omega),

with the full band-basis lattice Green's function

.. math::

   G_{\mathbf{k}}(\omega)
   = \left[ (\omega + i\delta + \mu)\,\mathbb{1}
            - H(\mathbf{k}) - P^{\dagger}(\mathbf{k})\,\Sigma(\omega)\,P(\mathbf{k}) \right]^{-1}.

Because :math:`H(\mathbf{k})` is diagonal in the band basis and
:math:`P^{\dagger}\Sigma P` is a rank-:math:`M` correction
(:math:`M = \dim\mathcal{C} \ll N_\nu`), :math:`G_{\mathbf{k}}` is assembled
with the rank-reduced Woodbury identity (see
:ref:`userguide_theory_and_notation`) rather than a dense
:math:`N_\nu \times N_\nu` inversion at every frequency.

.. note::

   The frequency :math:`\omega` is measured **from the chemical potential**:
   :math:`G_{\mathbf{k}}` is built with :math:`\omega + \mu`, so spectral
   weight at :math:`\omega = 0` sits at :math:`\mu`, and the Fermi factors
   below use :math:`\omega` directly.

Velocities and the band-window intersection
--------------------------------------------

The velocity matrices :math:`v_{\mathbf{k},\alpha}` are the band-basis
momentum/velocity matrix elements loaded alongside the one-body elements
(see :py:func:`~triqs_modest.obe.one_body_elements_from_dft_converter` with
``read_velocities=True``). The spectral function lives on the dispersion band
window while the velocities live on the optics band window; the trace is taken
over their per-:math:`\mathbf{k}` **intersection** (precomputed at load into
the joint window).

Symmetrization
--------------

In some DFT codes, the :math:`\mathbf{k}`-sum runs over the irreducible Brillouin zone. The full
Brillouin-zone average is restored by averaging the velocities over the
:math:`N_{\mathrm{sym}}` Cartesian point-group operations
:math:`R` supplied by the DFT converter,

.. math::

   v_{\mathbf{k},\alpha} \;\to\;
   \frac{1}{N_{\mathrm{sym}}} \sum_{R}\,
   \big(R\,v_{\mathbf{k}}\big)_{\alpha},
   \qquad
   \big(R\,v_{\mathbf{k}}\big)_{\alpha} = \sum_{c} R_{\alpha c}\, v_{\mathbf{k},c}.

Total, intraband, and interband
--------------------------------

:math:`\Gamma` is returned split into three pieces. The **total** is the full
band trace above. The **intraband** part keeps only band-diagonal terms,

.. math::

   \Gamma^{\mathrm{intra}}_{\alpha\beta}(\omega,\Omega)
   = \frac{1}{V} \sum_{\mathbf{k}} w_{\mathbf{k}} \sum_{n}
     \big(v_{\mathbf{k},\alpha}\big)_{nn}\, A_{n}(\omega+\Omega)\,
     \big(v_{\mathbf{k},\beta}\big)_{nn}\, A_{n}(\omega),

and the **interband** part is the remainder,
:math:`\Gamma^{\mathrm{inter}} = \Gamma - \Gamma^{\mathrm{intra}}`.

The transport function
======================

The *transport function* is the non-interacting (:math:`\Sigma = 0`)
intraband building block, evaluated directly from the bare DFT bands
:math:`\varepsilon_{n}(\mathbf{k})` with a Lorentzian-broadened delta,

.. math::

   \Phi_{\alpha\beta}(\omega)
   = \frac{1}{V} \sum_{\mathbf{k}} w_{\mathbf{k}} \sum_{n}
     \big(v_{\mathbf{k},\alpha}\big)_{nn}\,
     \big(v_{\mathbf{k},\beta}\big)_{nn}\,
     \delta_{\eta}\!\big(\omega - \varepsilon_{n}(\mathbf{k})\big),
   \qquad
   \delta_{\eta}(x) = \frac{1}{\pi}\frac{\eta}{x^{2} + \eta^{2}}.

It is a raw building block (no Onsager prefactor is applied), useful as a
:math:`\Sigma`-independent sanity check on the velocities and windows.

Onsager coefficients
====================

The frequency integration is performed in Python
(:py:func:`~triqs_modest.optics.optical_conductivity`) on the precomputed
:math:`\Gamma`. Writing the spin factor as :math:`(2 - \mathrm{SP})`
(:math:`\mathrm{SP}=0` for a spin-degenerate calculation, :math:`1` otherwise)
and the Fermi function :math:`f(\omega) = 1/(1+e^{\beta\omega})`, the Onsager
kinetic moments at :math:`\Omega = 0` are

.. math::

   A^{(n)}_{\alpha\beta}
   = \pi\,(2-\mathrm{SP}) \int d\omega\;
     \Gamma_{\alpha\beta}(\omega, 0)\, f(\omega)\big[1 - f(\omega)\big]\,
     (\beta\omega)^{n},
   \qquad n = 0, 1, 2 .

The optical conductivity is

.. math::

   \sigma_{\alpha\beta}(\Omega = 0) &= \beta\, C\, A^{(0)}_{\alpha\beta}, \\
   \sigma_{\alpha\beta}(\Omega \neq 0)
   &= C\,\frac{\pi\,(2-\mathrm{SP})}{\Omega}
      \int d\omega\; \Gamma_{\alpha\beta}(\omega,\Omega)\,
      \big[f(\omega) - f(\omega+\Omega)\big],

and the DC Seebeck coefficient and electronic thermal conductivity are

.. math::

   S_{\alpha\beta} = -\frac{A^{(1)}_{\alpha\beta}}{A^{(0)}_{\alpha\beta}}\,
     \frac{k_{B}}{e},
   \qquad
   \kappa_{\alpha\beta} = \left(
       A^{(2)}_{\alpha\beta}
       - \frac{\big(A^{(1)}_{\alpha\beta}\big)^{2}}{A^{(0)}_{\alpha\beta}}
     \right) C_{\kappa}.

:math:`S` and :math:`\kappa` require :math:`\Omega = 0` to be present in the
external mesh (they use the moments at :math:`\Omega = 0`); they are returned
as ``NaN`` otherwise, or when :math:`A^{(0)} \approx 0`.

.. _userguide_transport_units:

Units and conventions
=====================

ModEST standardizes **all** transport quantities,
so that the same Onsager prefactors apply regardless of the originating DFT
code. Internally:

* band energies and :math:`\omega`, :math:`\Omega` are in **eV**,
* the inverse temperature :math:`\beta` is in **eV**\ :sup:`-1`,
* velocities :math:`v_{\mathbf{k},\alpha}` are in **eV·Å**,
* the cell volume :math:`V` is in **Å**\ :sup:`3`.

With these choices the spectral function :math:`A` carries units of
eV\ :sup:`-1`, so the transport distribution has units

.. math::

   [\Gamma] = \frac{(\mathrm{eV\cdot\text{Å}})^{2}\,(\mathrm{eV}^{-1})^{2}}
                   {\mathrm{\text{Å}}^{3}} = \mathrm{\text{Å}}^{-1},

and the transport function :math:`[\Phi] = \mathrm{eV\cdot\text{Å}^{-1}}`.

SI prefactors and output units
-------------------------------

The prefactors above are derived from :mod:`scipy.constants` assuming eV·Å velocities and Å\ :sup:`3` volume:

.. math::

   C \equiv \frac{e^{2}}{\hbar}\times 10^{5},
   \qquad
   \frac{k_{B}}{e} = k_{B}[\mathrm{eV/K}]\times 10^{6},
   \qquad
   C_{\kappa} = C\, k_{B}[\mathrm{eV/K}].

:math:`C` converts :math:`\Gamma` in Å\ :sup:`-1` to
:math:`10^{3}\,\Omega^{-1}\mathrm{cm}^{-1}`. The units of every reported
quantity are:

.. list-table::
   :header-rows: 1
   :widths: 30 30 40

   * - Object
     - Symbol
     - Units
   * - Transport distribution
     - :math:`\Gamma_{\alpha\beta}(\omega,\Omega)`
     - Å\ :sup:`-1`
   * - Transport function
     - :math:`\Phi_{\alpha\beta}(\omega)`
     - eV·Å\ :sup:`-1`
   * - Optical / DC conductivity
     - :math:`\sigma_{\alpha\beta}(\Omega)`
     - :math:`10^{3}\,\Omega^{-1}\mathrm{cm}^{-1}`
   * - Seebeck coefficient
     - :math:`S_{\alpha\beta}`
     - μV/K
   * - Thermal conductivity
     - :math:`\kappa_{\alpha\beta}`
     - W/(m·K)