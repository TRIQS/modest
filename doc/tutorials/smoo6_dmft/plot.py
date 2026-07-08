import numpy as np

import matplotlib.pyplot as plt

from triqs_modest.utils import Checkpointer

# --- reopen the checkpoint written by dmft.py and grab the last iteration ---
U, J, beta = 2.0, 0.20, 40
ckpt = Checkpointer(f"beta{beta}_U{U}_J{J}.ckpt")
last = ckpt[len(ckpt) - 1]

Delta = last.Delta_list[0]        # hybridization Δ(iω_n), one BlockGf for the t2g impurity
Gloc  = last.Gloc_list[0]         # local Green's function on the correlated subspace
Sigma_dyn = last.Sigma_imp_list[0]      # dynamic part of the impurity self-energy
Sigma_hf  = last.Sigma_hartree_list[0]  # static Hartree-Fock part (constant matrices)

# The whole point of the rotation: in the local (crystal-field) basis every block
# of Δ is a scalar — the off-diagonal hybridization has been rotated away.
print("Δ block structure:", [(bl, g.target_shape) for bl, g in Delta])

# The three t2g crystal-field orbitals: the degenerate pair is xz/yz, the singlet is xy.
orb_labels = {"up_0": r"$xz$", "up_1": r"$yz$", "up_2": r"$xy$"}

om = np.fromiter(Delta.mesh, complex).imag

# --- Im Δ(iω_n): the t2g crystal-field-resolved hybridization ---
fig, ax = plt.subplots(1, 2, figsize=(6.75, 2.75))
for bl, g in Delta:
    if bl.startswith("up"):
        ax[0].plot(om, g[0, 0].data.imag, "o-", lw=0.5, ms=3, mec="k", label=orb_labels[bl])
ax[0].set_xlim(0, 10)
ax[0].set_xlabel(r"$\omega_n$ (eV)")
ax[0].set_ylabel(r"$\mathrm{Im}\,\Delta(i\omega_n)$ (eV)")
ax[0].legend(frameon=False, fontsize=9)

# --- Im G_loc(iω_n): the correlated-subspace Green's function ---
for bl, g in Gloc:
    if bl.startswith("up"):
        ax[1].plot(om, g[0, 0].data.imag, "o-", lw=0.5, ms=3, mec="k", label=orb_labels[bl])
ax[1].set_xlim(0, 10)
ax[1].set_xlabel(r"$\omega_n$ (eV)")
ax[1].set_ylabel(r"$\mathrm{Im}\,G_\mathrm{loc}(i\omega_n)$ (eV$^{-1}$)")
ax[1].legend(frameon=False, fontsize=9)

fig.tight_layout()
fig.savefig("media/smoo6_hybridization.png", dpi=200)

# --- self-energy Σ(iω_n) = Σ_dynamic(iω_n) + Σ_HartreeFock ---
# The full impurity self-energy is the dynamic (frequency-dependent) part plus
# the static Hartree-Fock shift returned by the solver.
Sigma = Sigma_dyn.copy()
for ibl, (bl, g) in enumerate(Sigma):
    Sigma[bl] << g + Sigma_hf[ibl]

fig2, ax2 = plt.subplots(1, 2, figsize=(6.75, 2.75))
for bl, g in Sigma:
    if bl.startswith("up"):
        ax2[0].plot(om, g[0, 0].data.imag, "o-", lw=0.5, ms=3, mec="k", label=orb_labels[bl])
        ax2[1].plot(om, g[0, 0].data.real, "o-", lw=0.5, ms=3, mec="k", label=orb_labels[bl])
ax2[0].set_xlim(0, 10)
ax2[0].set_xlabel(r"$\omega_n$ (eV)")
ax2[0].set_ylabel(r"$\mathrm{Im}\,\Sigma(i\omega_n)$ (eV)")
ax2[0].legend(frameon=False, fontsize=9)
ax2[1].set_xlim(0, 10)
ax2[1].set_xlabel(r"$\omega_n$ (eV)")
ax2[1].set_ylabel(r"$\mathrm{Re}\,\Sigma(i\omega_n)$ (eV)")
ax2[1].legend(frameon=False, fontsize=9)

fig2.tight_layout()
fig2.savefig("media/smoo6_self_energy.png", dpi=200)
plt.show()
