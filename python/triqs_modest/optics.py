r"""Optics: Onsager frequency integration of the Kubo transport distribution.

The compiled module :mod:`triqs_modest.bubble` provides the MPI-parallel k-sum kernels
``transport_distribution`` / ``transport_function`` (the current-current bubble). This module adds the cheap,
master-side frequency integration that turns a transport distribution
:math:`\Gamma_{\alpha\beta}(\omega,\Omega)` into the Onsager kinetic coefficients and the physical
observables (conductivity, Seebeck, thermal conductivity). The split is deliberate: compute
:math:`\Gamma` once (expensive), then post-process it here as often as you like (sweep temperature,
integration rule, directions) without repeating the k-sum.

Physics (following the bubble package)
--------------------------------------
Onsager moments (DC, :math:`\Omega=0`)::

    A_n = pi (2 - SP) \int d\omega  Gamma(\omega) f(\omega)[1 - f(\omega)] (beta*omega)^n

Optical conductivity::

    sigma(Omega=0) = beta * A_0                                             (working units)
    sigma(Omega!=0) = pi (2-SP)/Omega \int d\omega Gamma(\omega,Omega) [f(\omega) - f(\omega+Omega)]

Transport coefficients::

    S = -(A_1/A_0) * (k_B/e)          [uV/K]
    kappa = (A_2 - A_1^2/A_0) * C_kappa   [W/(m K)]

Units note
----------
The SI prefactors below follow the bubble package and assume band velocities in eV*Angstrom and the
cell volume in Angstrom^3 (so :math:`\Gamma` carries units of Angstrom^{-1} after the ``1/V`` k-sum
normalisation). Verify this matches the units the DFT converter wrote before trusting the absolute
magnitude of ``sigma``/``kappa``; the frequency grid ``omega`` must be measured from the chemical
potential ``mu`` (this is how the kernel builds ``G = [omega + i*delta + mu - H]^{-1}``).
"""

from dataclasses import dataclass
from typing import Dict

import numpy as np
import scipy.constants as _cst
from scipy.integrate import simpson as _simpson

__all__ = ["optical_conductivity", "OpticalConductivity"]

# --- unit conversion constants (from scipy.constants; velocities eV*Angstrom, volume Angstrom^3) ---
_KB_EV = _cst.physical_constants["Boltzmann constant in eV/K"][0]
_CONVERT_TO_SI = _cst.e**2 / _cst.hbar * 1e5  # [Angstrom^-1] -> [10^3 Ohm^-1 cm^-1]
_SEEBECK_FACTOR = _KB_EV * 1e6                # (k_B/e) in uV/K
# Note the extra 1e5 vs _CONVERT_TO_SI. It is NOT a second SI conversion; it undoes an output-unit
# reduction baked into _CONVERT_TO_SI. The full-SI conductivity prefactor is e^2/hbar * 1e10 (the 1e10
# is the Angstrom^-1 -> m^-1 of Gamma), giving sigma in S/m. But we REPORT sigma in 10^3 Ohm^-1 cm^-1
# = 1e5 S/m — a unit 1e5x larger than S/m — so _CONVERT_TO_SI = e^2/hbar * 1e5 is that full prefactor
# divided by 1e5, i.e. 1e5 SHORT of S/m. kappa is reported in genuine SI, W/(m K), and its Onsager form
# needs the conductivity as a true S/m number, i.e. the full 1e10 prefactor = _CONVERT_TO_SI * 1e5.
_KAPPA_FACTOR = _CONVERT_TO_SI * 1e5 * _KB_EV  # (W/m/K)

_DC_TOL = 1e-10   # |Omega| below which Omega is treated as the DC (Omega=0) point
_A0_TOL = 1e-15   # |A_0| below which S/kappa are undefined (returned as NaN)

# Which member of the transport distribution to integrate.
_GAMMA_ATTR = {"total": "Gamma", "intra": "Gamma_intra", "inter": "Gamma_inter"}


def _fermi(omega, beta):
    r"""Fermi-Dirac f(omega) = 1/(1+e^{beta*omega}), via the stable form (1 - tanh(beta*omega/2))/2."""
    return 0.5 * (1.0 - np.tanh(0.5 * beta * omega))


def _integrate(integrand, x, method="simpson"):
    """1-D numerical integral of ``integrand`` over grid ``x`` ('simpson' | 'trapz' | 'rect')."""
    if method == "simpson":
        return float(_simpson(integrand, x=x))
    if method == "trapz":
        return float(np.trapz(integrand, x))
    if method == "rect":
        return float(np.sum(integrand) * (x[1] - x[0]))
    raise ValueError(f"optics: unknown integration method '{method}'")


def _dc_index(Om_mesh):
    """Index of the DC point (Omega=0) in ``Om_mesh``, or None if absent."""
    hits = np.where(np.abs(np.asarray(Om_mesh, dtype=float)) < _DC_TOL)[0]
    return int(hits[0]) if len(hits) else None


def _onsager_moments(Gamma_dc, omega, beta, spin_factor, method="simpson"):
    r"""Onsager moments ``[A_0, A_1, A_2]`` at Omega=0 for one direction.

    ``A_n = pi (2-SP) \int d\omega Gamma(\omega) f(\omega)[1-f(\omega)] (beta*omega)^n``.
    ``Gamma_dc`` is the Omega=0 slice, shape ``(n_omega,)``.
    """
    f = _fermi(omega, beta)
    base = Gamma_dc * f * (1.0 - f)
    omega_beta = omega * beta
    pref = np.pi * spin_factor
    return np.array([_integrate(base * omega_beta**n, omega, method) * pref for n in range(3)])


def _sigma_of_Omega(Gamma, omega, Om_mesh, beta, spin_factor, method="simpson"):
    r"""Optical conductivity ``sigma(Omega)`` for one direction, shape ``(n_Om,)`` in 10^3 Ohm^-1 cm^-1.

    DC branch (``|Omega| < _DC_TOL``): ``sigma = beta * C * pi (2-SP) \int d\omega Gamma f (1-f)``.
    AC branch: ``sigma = beta * C * pi (2-SP)/(Omega beta) \int d\omega Gamma(\omega,Omega) [f(\omega)-f(\omega+Omega)]``.
    ``Gamma`` is the single-direction distribution, shape ``(n_Om, n_omega)``.
    """
    f = _fermi(omega, beta)
    pref = np.pi * spin_factor
    sigma = np.zeros(len(Om_mesh))
    for iq, Om in enumerate(Om_mesh):
        if abs(Om) < _DC_TOL:
            integrand = Gamma[iq] * f * (1.0 - f)
        else:
            integrand = Gamma[iq] * (f - _fermi(omega + Om, beta)) / (Om * beta)
        sigma[iq] = beta * _CONVERT_TO_SI * _integrate(integrand, omega, method) * pref
    return sigma


@dataclass
class OpticalConductivity:
    """Result of :func:`optical_conductivity`.

    Attributes
    ----------
    sigma : dict[str, np.ndarray]
        Optical conductivity sigma(Omega) per direction, shape ``(n_Om,)``, in 10^3 Ohm^-1 cm^-1.
    seebeck : dict[str, float]
        DC Seebeck coefficient per direction, in uV/K. NaN if Omega=0 is absent from the mesh
        (or the DC conductivity A_0 is ~0).
    kappa : dict[str, float]
        DC electronic thermal conductivity per direction, in W/(m K). NaN under the same conditions.
    td : transport_distribution_t
        The transport distribution the coefficients were computed from (as passed in).
    contribution : str
        Which part of the transport distribution was integrated ('total', 'intra', or 'inter').
    """

    sigma: Dict[str, np.ndarray]
    seebeck: Dict[str, float]
    kappa: Dict[str, float]
    td: object
    contribution: str


def optical_conductivity(td, beta, method="simpson", contribution="total"):
    r"""Onsager transport coefficients from a precomputed transport distribution.

    Pure (cheap) post-processing of the Kubo bubble: given ``td`` from
    :func:`~triqs_modest.bubble.transport_distribution`, integrate over frequency to get the optical
    conductivity sigma(Omega), and — if the DC point (Omega=0) is present — the Seebeck coefficient S
    and electronic thermal conductivity kappa. The directions, frequency meshes, and spin factor are all
    read from ``td``; ``beta`` is the only extra physics input, so the expensive k-sum need not be
    repeated to sweep temperature, integration method, or band contribution.

    Parameters
    ----------
    td : transport_distribution_t
        Output of :func:`~triqs_modest.bubble.transport_distribution` (already normalised by the cell volume).
    beta : float
        Inverse temperature 1/(k_B T) in 1/eV.
    method : {'simpson', 'trapz', 'rect'}, optional
        Frequency-integration rule (default 'simpson').
    contribution : {'total', 'intra', 'inter'}, optional
        Which part of the transport distribution to integrate: the full bubble (``'total'``, default),
        the intraband (band-diagonal, Drude-like) part (``'intra'``), or the interband part
        (``'inter'``). Uses ``td.Gamma``, ``td.Gamma_intra``, or ``td.Gamma_inter`` respectively.

    Returns
    -------
    OpticalConductivity
    """
    if contribution not in _GAMMA_ATTR:
        raise ValueError(f"optical_conductivity: contribution must be one of {list(_GAMMA_ATTR)}, got {contribution!r}")

    omega = np.asarray(td.omega_mesh)
    Om_mesh = np.asarray(td.Om_mesh, dtype=float)
    Gamma = np.asarray(getattr(td, _GAMMA_ATTR[contribution]))  # (n_dir, n_Om, n_omega)
    directions = list(td.directions)
    spin_factor = 2.0 - td.spin_polarization
    idc = _dc_index(Om_mesh)

    sigma, seebeck, kappa = {}, {}, {}
    for i, d in enumerate(directions):
        sigma[d] = _sigma_of_Omega(Gamma[i], omega, Om_mesh, beta, spin_factor, method)
        if idc is None:
            seebeck[d] = kappa[d] = float("nan")
            continue
        A0, A1, A2 = _onsager_moments(Gamma[i][idc], omega, beta, spin_factor, method)
        if abs(A0) > _A0_TOL:
            seebeck[d] = -A1 / A0 * _SEEBECK_FACTOR
            kappa[d] = (A2 - A1**2 / A0) * _KAPPA_FACTOR
        else:
            seebeck[d] = kappa[d] = float("nan")

    return OpticalConductivity(sigma=sigma, seebeck=seebeck, kappa=kappa, td=td, contribution=contribution)
