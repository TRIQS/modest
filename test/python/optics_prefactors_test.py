"""Lock the Onsager post-processing prefactors in triqs_modest.optics.

This is a data-free unit test on a synthetic transport distribution: a constant
Gamma(omega, Omega=0) = Gamma0. For a flat Gamma the Onsager moments are analytic,
so the optical conductivity, Seebeck coefficient and (crucially) the electronic
thermal conductivity kappa can be checked against independently computed values.

It guards in particular the kappa SI prefactor: kappa must use the conductivity in
base SI (S/m), which is 1e5 larger than the 10^3 Ohm^-1 cm^-1 that the sigma prefactor
targets. A missing/extra factor of 1e5 in _KAPPA_FACTOR makes kappa fail here.
"""
import types
import numpy as np
import scipy.constants as cst
from scipy.integrate import simpson

from triqs_modest.optics import optical_conductivity

BETA = 40.0          # 1/eV  (T ~ 290 K)
GAMMA0 = 0.7         # flat transport distribution [Angstrom^-1]
SP = 0               # spin-degenerate -> spin factor 2

# independently reconstructed constants (velocities eV*Angstrom, volume Angstrom^3)
KB = cst.physical_constants["Boltzmann constant in eV/K"][0]
C_SIGMA = cst.e**2 / cst.hbar * 1e5          # Gamma[1/Ang] -> sigma [10^3 Ohm^-1 cm^-1]
C_KAPPA = C_SIGMA * 1e5 * KB                 # -> kappa [W/(m K)]   (note the extra 1e5)
SEEBECK = KB * 1e6                           # (k_B/e) in uV/K


def _fermi(w, beta):
    return 0.5 * (1.0 - np.tanh(0.5 * beta * w))


def _make_td(omega):
    g = np.full((1, 1, omega.size), GAMMA0)  # (n_dir, n_Om, n_omega), single dir "xx", Om=0
    return types.SimpleNamespace(
        omega_mesh=omega, Om_mesh=np.array([0.0]),
        Gamma=g, Gamma_intra=g, Gamma_inter=np.zeros_like(g),
        directions=["xx"], spin_polarization=SP,
    )


def test_optics_prefactors_flat_gamma():
    # wide, dense, symmetric grid so f(1-f) is fully resolved and its tails vanish
    omega = np.linspace(-3.0, 3.0, 6001)
    td = _make_td(omega)
    res = optical_conductivity(td, beta=BETA, method="simpson")

    # ---- independent reference (Onsager moments of a flat Gamma) ----
    f = _fermi(omega, BETA)
    base = GAMMA0 * f * (1.0 - f)
    spin = 2.0 - SP
    A0 = np.pi * spin * simpson(base, x=omega)
    A1 = np.pi * spin * simpson(base * (omega * BETA), x=omega)
    A2 = np.pi * spin * simpson(base * (omega * BETA) ** 2, x=omega)

    sigma_dc_ref = BETA * C_SIGMA * A0
    seebeck_ref = -A1 / A0 * SEEBECK
    kappa_ref = (A2 - A1**2 / A0) * C_KAPPA

    assert np.isclose(res.sigma["xx"][0], sigma_dc_ref, rtol=1e-10), \
        (res.sigma["xx"][0], sigma_dc_ref)
    # A1 ~ 0 by particle-hole symmetry -> Seebeck ~ 0; compare on the kappa/absolute instead
    assert abs(res.seebeck["xx"] - seebeck_ref) < 1e-6, (res.seebeck["xx"], seebeck_ref)
    assert np.isclose(res.kappa["xx"], kappa_ref, rtol=1e-10), (res.kappa["xx"], kappa_ref)

    # physical sanity: Wiedemann-Franz. For a flat Gamma the Lorenz number is exactly
    # L0 = (pi^2/3)(k_B/e)^2. Check kappa = L0 * T * sigma_DC(SI) to a few percent.
    T = 1.0 / (KB * BETA)
    sigma_si = res.sigma["xx"][0] * 1e5           # 10^3 Ohm^-1 cm^-1 -> S/m
    L0 = (np.pi**2 / 3.0) * (cst.k / cst.e) ** 2  # W Ohm / K^2
    kappa_wf = L0 * T * sigma_si
    assert np.isclose(res.kappa["xx"], kappa_wf, rtol=0.02), (res.kappa["xx"], kappa_wf)


if __name__ == "__main__":
    test_optics_prefactors_flat_gamma()
    print("optics_prefactors_test: OK")
