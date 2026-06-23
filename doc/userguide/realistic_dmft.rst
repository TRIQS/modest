.. _userguide_realistic_dmft:

Realistic DMFT and electronic structure
***************************************

This page describes how dynamical mean-field theory (DMFT) is combined
with realistic electronic-structure methods — density-functional theory
(DFT) -- and how ModEST organizes
that combination in code. The project stands on the shoulders of `triqs/dft_tools <https://triqs.github.io/dft_tools>`_. 
While introducing all of the concepts, we will introduce the core ModEST abstractions: *one-body elements*, *embedding*,
and *k-summation*.

From DFT to a correlated low-energy problem
===========================================

DFT in the Kohn–Sham (KS) framework gives a one-particle description of
the full electronic structure in terms of Bloch states
:math:`|\psi_{\nu}^{\sigma}(\mathbf{k})\rangle` with eigenvalues
:math:`\varepsilon_{\nu}^{\sigma}(\mathbf{k})`. For materials with
narrow, partially-filled bands (typically of :math:`d` or :math:`f`
character) the KS descriptions is no longer a good description of the low-energy properties of the material.
This indicates that entanglement (or the quantumness of the electrons) is imporant and we must negotiate
with solving a quantum many-body problem. 

Conceptually, the DMFT framework requires one to single out a *correlated subspace*
:math:`\mathcal{C}` spanned by localized orbitals
:math:`|\chi_{m_a}^{R_a\,\sigma}\rangle` (Wannier functions, projected
local orbitals, …) and to compute a self-energy
:math:`\Sigma_{\mathcal{C}}^{\sigma}(\omega)` only on that subspace. The
connection between the two spaces is given by the projector
:math:`P_{m\nu}^{\sigma}(\mathbf{k})` introduced in
:ref:`userguide_theory_and_notation`. Everything else — full Bloch
states, all bands, the lattice :math:`\mathbf{k}`-grid — enters through
one-body quantities that ModEST groups into a single
:py:class:`one-body-elements <triqs_modest.obe.OneBodyElementsOnGrid>`
object.

The lattice Hamiltonian + self-energy problem
=============================================

The interacting lattice Green's function reads (see
:eq:`eq_lattG`–:eq:`eq_lattSigma`)

.. math::
   :label: rdmft_lattG

   \bigl[G_{\mathcal{B}}^{\sigma}\bigr]^{-1}(\mathbf{k}, \omega)
   = (\omega + \mu)\,\mathbb{1} - H^{\sigma}(\mathbf{k})
     - P^{\dagger}(\mathbf{k})\,
       \bigl[\Sigma_{\mathrm{imp}}^{\sigma}(\omega)
             - \Sigma_{\mathrm{DC}}^{\sigma}\bigr]\,P(\mathbf{k}),

where :math:`H^{\sigma}(\mathbf{k})` is the Kohn–Sham one-body
Hamiltonian on the chosen band window, :math:`\mu` is the chemical
potential, :math:`\Sigma_{\mathrm{imp}}^{\sigma}` is the impurity
self-energy supplied by an impurity solver, and
:math:`\Sigma_{\mathrm{DC}}^{\sigma}` is the double-counting correction
that removes the static local-interaction contribution already present
in the DFT exchange-correlation functional.

The DMFT self-consistency cycle
===============================

DMFT closes a loop on the impurity Green's function by demanding that
the impurity Green's function equals the local Green's function
obtained by projecting :math:`G_{\mathcal{B}}` onto :math:`\mathcal{C}`:

.. math::
   :label: rdmft_selfconsistency

   G_{\mathrm{imp}}^{\sigma}(\omega) \;\stackrel{!}{=}\;
   G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}(\omega)
   = \sum_{\mathbf{k}} P(\mathbf{k})\,
     G_{\mathcal{B}}^{\sigma}(\mathbf{k}, \omega)\,P^{\dagger}(\mathbf{k}).

Operationally, each DMFT iteration consists of the following steps:

#. **Upfold the embedded self-energy.** Given the current impurity
   self-energy :math:`\Sigma_{\mathrm{imp}}^{\sigma}` and the
   double-counting term :math:`\Sigma_{\mathrm{DC}}^{\sigma}`, build the
   embedded self-energy
   :math:`\Sigma_{\mathrm{embed}}^{\sigma}
   = \Sigma_{\mathrm{imp}}^{\sigma} - \Sigma_{\mathrm{DC}}^{\sigma}`
   and upfold it to :math:`\mathcal{B}` via :math:`P^{\dagger}\cdot P`.

#. **Compute the local Green's function** by k-summation
   (:eq:`eq_locG` / :eq:`eq_DysonW`).

#. **Adjust the chemical potential** so that the total particle number
   matches the target density.

#. **Extract impurity-level inputs** for the solver: the impurity
   levels :math:`E_{\mathrm{imp}}` and the hybridization function
   :math:`\Delta(\omega)` obtained from
   :math:`G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}`.

#. **Solve the impurity model** to obtain a new
   :math:`\Sigma_{\mathrm{imp}}^{\sigma}` and iterate to convergence.

Charge self-consistency
=======================

In one-shot DFT+DMFT the KS Hamiltonian :math:`H^{\sigma}(\mathbf{k})`
is fixed at its DFT value. In *charge self-consistent* DFT+DMFT (CSC)
the DMFT-corrected density is fed back into the DFT code, which
recomputes :math:`H^{\sigma}(\mathbf{k})` and the projectors
:math:`P(\mathbf{k})`. The two loops are nested: DFT updates the
electronic density and the Kohn–Sham potential; DMFT updates the
self-energy on :math:`\mathcal{C}`. ModEST provides a generic
:py:class:`triqs_modest.dft_driver.DftDriver` interface that hides
code-specific details (VASP, Quantum ESPRESSO, Wien2k, …) behind two
hooks: ``run_initial_stage`` (initial DFT run) and ``run_update_stage``
(DFT update given a DMFT charge-density correction).

How ModEST organizes the calculation
====================================

The three primitives that appear in every realistic DMFT calculation
map directly onto ModEST objects:

* **One-body elements** — the projector :math:`P(\mathbf{k})`, the
  Kohn–Sham dispersion :math:`\varepsilon_{\nu}^{\sigma}(\mathbf{k})`,
  and (for CSC) the band character matrices and the symmetries used to
  unfold to the full Brillouin zone. See
  :ref:`reference/python/one_body_elements`.

* **Embedding** — the embedding object is the central bookkeeper that
  carries every mapping between the impurity problems and the solid:
  upfolding the impurity self-energies into the embedded self-energy
  on :math:`\mathcal{C}`, downfolding the local Green's function back
  to each impurity, applying the basis rotations that absorb local
  coordinate systems and symmetry adaptations, and gluing equivalent
  atoms, magnetic-order patterns, split/dropped impurities, and
  cluster-DMFT groupings into a single, consistent description. The
  DMFT loop only ever sees impurity-shaped objects on one side and
  lattice-shaped objects on the other — the embedding handles the
  translation in both directions. See :ref:`reference/python/embedding`.

* **k-summation** — local Green's function, density, chemical-potential
  search, and charge-density correction. The k-sums are implemented on
  the Woodbury-reduced form of the Dyson equation (:eq:`eq_DysonW`):
  every inversion happens in the small :math:`\mathcal{C}` space rather
  than in the full band space :math:`\mathcal{B}`, which is what makes
  the lattice-side cost of a DMFT iteration cheap even for large band
  windows. The k-sums are MPI-parallel, with per-:math:`\mathbf{k}`
  work dispatched across ranks. See :ref:`reference/python/dmft_loop`
  and :ref:`reference/python/csc`.

For the underlying notation, conventions, and derivations, continue to
:ref:`userguide_theory_and_notation`.
