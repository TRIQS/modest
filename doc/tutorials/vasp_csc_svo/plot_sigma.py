# ============================================================================
# Track the impurity self-energy and Green's function across the CSC iterations
# ============================================================================
#
# Reads the checkpoint.h5 archive written by vasp_modest_csc.py and produces
# sigma_convergence.png with two panels, both on the low-frequency Matsubara
# window 0 < w_n < 5 eV and one curve per global (charge) iteration, coloured
# from the first (light) to the last (dark):
#
#   * left  : imaginary part of the impurity self-energy   Im Sigma(i w_n)
#   * right : imaginary part of the impurity Green's function Im G_imp(i w_n)
#
# All t2g/spin blocks are symmetry-equivalent here, so we follow a single
# representative block.
#
# Usage:  python plot_sigma.py [checkpoint.h5]
# Pass the shipped reference archive to reproduce the tutorial figure:
#   python plot_sigma.py checkpoint_ref.h5

import sys

import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

from triqs.gfs import BlockGf, Gf  # noqa: F401  (needed so h5 recognises the Gf)
from h5 import HDFArchive

fname = sys.argv[1] if len(sys.argv) > 1 else "checkpoint.h5"

block = "up_0"          # representative (degenerate) impurity block
w_max = 5.0             # plot the Matsubara window 0 < w_n < w_max (eV)


def iteration_keys(ar):
    keys = [k for k in ar.keys() if k.startswith("it=")]
    return sorted(keys, key=lambda k: int(k.split("=")[1]))


def positive_branch(g):
    """positive Matsubara frequencies and the (1,1) diagonal data, masked to w_max."""
    n_iw = g.data.shape[0] // 2
    w = np.array([complex(x).imag for x in g.mesh])[n_iw:]
    d = g.data[n_iw:, 0, 0]
    m = w <= w_max
    return w[m], d[m]


with HDFArchive(fname, "r") as ar:
    keys = iteration_keys(ar)
    n_it = len(keys)
    sigma, gimp = [], []
    for k in keys:
        wn, s = positive_branch(ar[k]["Sigma_iw"][block])
        _,  g = positive_branch(ar[k]["Gimp_iw"][block])
        sigma.append((wn, s.imag))
        gimp.append((wn, g.imag))

cmap = plt.get_cmap("viridis")
fig, (axS, axG) = plt.subplots(1, 2, figsize=(11, 4.2))

for i in range(n_it):
    col = cmap(i / max(n_it - 1, 1))
    lbl = f"it {i + 1}" if (i == 0 or i == n_it - 1) else None
    axS.plot(sigma[i][0], sigma[i][1], "-o", ms=4, lw=1.2, color=col, label=lbl)
    axG.plot(gimp[i][0],  gimp[i][1],  "-o", ms=4, lw=1.2, color=col, label=lbl)

axS.set_xlabel(r"$\omega_n$ (eV)")
axS.set_ylabel(r"$\mathrm{Im}\,\Sigma_\mathrm{imp}(i\omega_n)$ (eV)")
axS.set_title("Impurity self-energy")
axS.set_xlim(0, w_max)
axS.legend(frameon=False)

axG.set_xlabel(r"$\omega_n$ (eV)")
axG.set_ylabel(r"$\mathrm{Im}\,G_\mathrm{imp}(i\omega_n)$ (eV$^{-1}$)")
axG.set_title("Impurity Green's function")
axG.set_xlim(0, w_max)

sm = plt.cm.ScalarMappable(cmap=cmap, norm=plt.Normalize(vmin=1, vmax=n_it))
cbar = fig.colorbar(sm, ax=axG, pad=0.02)
cbar.set_label("charge iteration")

fig.tight_layout()
fig.savefig("sigma_convergence.png", dpi=150)
print(f"wrote sigma_convergence.png ({n_it} iterations, w_n < {w_max} eV)")
