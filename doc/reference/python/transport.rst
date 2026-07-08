.. _reference/python/transport:

Transport & optical conductivity
********************************

The Kubo bubble (current–current correlation) evaluated on the DFT + DMFT
lattice: the transport distribution
:math:`\Gamma_{\alpha\beta}(\omega,\Omega)` and transport function
:math:`\Phi_{\alpha\beta}(\omega)`, and the frequency integration that turns
:math:`\Gamma` into the optical conductivity :math:`\sigma(\Omega)`, Seebeck
coefficient :math:`S`, and electronic thermal conductivity :math:`\kappa`.

The workflow is two steps:

#. an expensive, MPI-parallel :math:`\mathbf{k}`-sum that builds
   :math:`\Gamma` (the compiled :py:mod:`triqs_modest.bubble` kernels), then
#. a cheap frequency integration (:py:mod:`triqs_modest.optics`) that consumes
   :math:`\Gamma` and can be reused across temperatures and integration rules
   without repeating the :math:`\mathbf{k}`-sum.

For the definitions, derivations, and — importantly — the **units** of every
quantity (velocities in eV·Å, volume in Å³, :math:`\sigma` in
:math:`10^{3}\,\Omega^{-1}\mathrm{cm}^{-1}`, :math:`S` in μV/K, :math:`\kappa`
in W/(m·K)), see :ref:`userguide_transport`.

.. code-block:: python

   import numpy as np
   import triqs_modest as tm

   # 1. DFT + transport input (velocities + volume), standardized to eV·Å / Å³
   target_density, obe = tm.one_body_elements_from_dft_converter(
       "seedname.h5", read_velocities=True)

   # 2. Kubo bubble on a real-frequency Sigma (expensive, MPI over k)
   Om_mesh = np.array([0.0])                    # DC -> sigma, S, kappa
   td = tm.transport_distribution(
       obe, mu, Sigma_w, Om_mesh, ["xx", "yy", "zz"], broadening=0.05)

   # 3. Onsager integration (cheap, reusable across beta / method)
   res = tm.optical_conductivity(td, beta=100.0)
   res.sigma["xx"]      # 10^3 Ohm^-1 cm^-1
   res.seebeck["xx"]    # uV/K
   res.kappa["xx"]      # W/(m K)

Transport distribution and transport function
=============================================

:py:func:`~triqs_modest.bubble.transport_distribution` builds the full Kubo
bubble :math:`\Gamma_{\alpha\beta}(\omega,\Omega)` (total, intraband, and
interband) from an on-grid one-body-elements object carrying the band
velocities and cell volume, the chemical potential, and a real-frequency
self-energy (whose mesh defines the internal :math:`\omega` grid). The
band-basis Green's function is assembled with the rank-reduced Woodbury fast
path, and the :math:`\mathbf{k}`-sum is MPI-parallel with an internal
``all_reduce``.

:py:func:`~triqs_modest.bubble.transport_function` computes the
non-interacting (:math:`\Sigma = 0`) transport function
:math:`\Phi_{\alpha\beta}(\omega)` directly from the bare DFT bands — a
:math:`\Sigma`-independent building block useful for validation.

Directions are passed as two-character strings (``"xx"``, ``"xy"``, …) parsed
internally to Cartesian index pairs.

.. autosummary::

   triqs_modest.bubble.transport_distribution
   triqs_modest.bubble.transport_function

Optical conductivity
====================

:py:func:`~triqs_modest.optics.optical_conductivity` is pure post-processing of
a precomputed transport distribution: it reads the directions, frequency
meshes, and spin factor from the ``td`` struct and integrates over frequency to
produce :math:`\sigma(\Omega)` for every :math:`\Omega`, plus — when the DC
point :math:`\Omega = 0` is present — the Seebeck coefficient :math:`S` and
electronic thermal conductivity :math:`\kappa`. ``beta`` is the only extra
physics input, so temperature sweeps and integration-rule changes do not
require repeating the :math:`\mathbf{k}`-sum. :math:`S` and :math:`\kappa` are
returned as ``NaN`` when :math:`\Omega = 0` is absent from the mesh (or when
:math:`A^{(0)} \approx 0`).

.. autosummary::

   triqs_modest.optics.optical_conductivity

Container types
===============

:py:class:`~triqs_modest.bubble.TransportDistributionT` carries the total,
intraband, and interband :math:`\Gamma` (each of shape
``(n_dir, n_Om, n_omega)``), the :math:`\omega` and :math:`\Omega` meshes, the
direction labels, and the spin-polarization flag.
:py:class:`~triqs_modest.bubble.TransportFunctionT` carries
:math:`\Phi` and its :math:`\omega` mesh.
:py:class:`~triqs_modest.optics.OpticalConductivity` bundles the ``sigma``,
``seebeck``, and ``kappa`` dictionaries (keyed by direction) with the raw
``td`` they were computed from.

.. autosummary::

   triqs_modest.bubble.TransportDistributionT
   triqs_modest.bubble.TransportFunctionT
   triqs_modest.optics.OpticalConductivity
