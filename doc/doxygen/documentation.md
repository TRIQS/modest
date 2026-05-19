@page documentation API Documentation

[TOC]

The Modular Electronic Structure Toolkit (ModEST) provides the necessary tooling to perform DMFT-type calculations in the context of
electronic structure including density-functional theory (DFT) and many-body perturbation theory (MBPT).

Modularity is achieved through four key abstractions

* one-body elements,
* embedding,
* k-summation

These abstractions let you describe complex DMFT problems -- from simple single-site DMFT to multi-impurity, spin-orbit, or cluster DMFT
-- in a clean and modular way.

Efficiency and performance are achieved via a modern C++ implementation, and all functionality is fully exposed in Python, making it possible
to prototype and run production calculations in either language.

If you are looking for a specific function, class, etc., try using the search bar in the top left corner.

---

## One-body elements
@ref one_body_elements (obe for short) contains all of the one-body data that is typically converted from a density-functional theory
(DFT) calculation or Wannier90 calculation. The one-body elements struct contains four components:

* @ref triqs::modest::local_space defines the local correlated subspace to be solved within DMFT.
* @ref triqs::modest::band_dispersion contains the band dispersion \f$\varepsilon(k, \nu, \sigma)\f$  where \f$\varepsilon\f$ is the
  energy at momentum k, band index \f$\sigma\f$, and spin \f$\sigma\f$.
* @ref triqs::modest::downfolding_projector contains the projector which downfolds from Bloch space \f$ \mathcal{B} \f$ (indexed by
  \f$\nu\f$) to the correlated space (index by \f$m\f$).
* @ref triqs::modest::ibz_symmetry_ops contains the operations to symmetrize observables over the full Brillouin zone.

To make it easy to build this object, ModEST provides factory functions that convert raw DFT data (from codes like VASP, Wien2k, or Wannier90)
into fully prepared OBE objects ready for DMFT.


## Embedding
The @ref embedding abstraction is the central controller of the DMFT problem: it defines how the correlated subspace (\f$\mathcal{C}\f$)
is partitioned into impurities and handles the bookkeeping needed to map back and forth between the lattice and the impurities.

Typical use cases covered by embedding:

* **Equivalent atoms**: Use the same impurity model (i.e. same \f$n_{\mathrm{imp}}\f$) for different blocks \f$ \alpha \f$.
* **Magnetic order**: Just reverse the spin for some atoms, \f$\tau(\sigma) = 1-\sigma\f$.
* **Splitting impurity models**: Depending on the block structure of the problem, the corresponding impurity model connected to atom \f$a\f$
  can be *split_imp*. Different impurity solvers can then be used for each of these split impurity models. The self-energies from both models map
  back to the a single atom \f$a\f$. Or a trivial solver is used to obtain zero self-energy (*drop_imp*).
* **cluster DMFT**: Re-group a subset of atoms \f$a\f$ into a larger *super atom* which will be treated as a cluster DMFT problem.

The embedding object provides:

* Factory functions to conveniently construct common embeddings.
* Operations to adapt and modify an embedding (e.g., split_imp, drop_imp, etc.).
* Methods to map data between impurity and embed spaces.

This abstrction simplifies the DMFT loop, making it easy to construct arbitrary embedding scenarios.


## Local Green's function
A central quantity of the DMFT self-consistency condition -- the @ref gloc is defined as
\f[
  [ G_{\mathrm{loc}}^{\sigma} ]_{m m'} = \sum_{\mathbf{k}} P_{m\nu}^{\sigma}(\mathbf{k}) \Big [ (\omega + \mu)\delta_{\nu\nu'} -
  H^{\sigma}_{\nu\nu'}(\mathbf{k}) - [P_{m\nu}^{\sigma}]^{\dagger} \Sigma_{\mathrm{embed}}P_{m'\nu'}^{\sigma}(\mathbf{k}) \Big ]^{-1}
  [P_{m'\nu'}^{\sigma}]^{\dagger},
\f]
where \f$\omega\f$ is a frequency (either real- or Matsubra), \f$\mu\f$ is the chemical potential, \f$H(\mathbf{k})\f$ is the one-body
Hamiltonian, \f$P(\mathbf{k})\f$ are the projectors from the band to the orbital basis, and \f$\Sigma_{\mathrm{embed}}\f$ is the embedded
self-energy.

ModEST computes this efficiently:

* **Woodbury**: Reduces the cost of matrix inversion from cubic in the number of bands to linear.
* **Adaptive Brillouin zone integration**: For tight-binding models, allows you to specify desired integration accuracy, improving predictions
  of transport properties.


## Atomic levels and Hybridization
The hybridization function \f$\Delta(\omega)\f$ is a key input to TRIQS impurity solvers. It describes the coupling of each impurity to the bath:
\f[
  [\Delta^{\sigma}(\omega)]_{mm'} = \omega - \tilde{\varepsilon} - G_{\mathrm{loc}}^{-1}(\omega) - \Sigma_{\mathrm{imp}}(\omega),
\f]
where \f$\tilde{\varepsilon} = \varepsilon - \mu - \Sigma_{\mathrm{DC}}\f$.


## Densities and Chemical Potentials
Calculate the total electronic density from the lattice Green's function and search for the chemical potential \f$\mu\f$ that gives a
target electron count. We provide several algorithms optimized for speed and robustness. Due to the modularity you can also leverage
external root finding routines to write a custom chemical potential search.


## Interaction Hamiltonians
To describe local interactions, ModEST provides tools to construct standard interaction @ref hamiltonian :

* @ref triqs::make_kanamori -- Hubbard-Kanamori with density-density, spin-flip, and pair-hopping terms.
* @ref triqs::make_slater -- full rotationally invariant Slater Hamiltonian.
* @ref triqs::make_density_density -- density-density only Hamiltonian.

All built using TRIQS many-body operators.

This design makes it easier to swap and test different double counting schemes.


## Post-processing
Compute spectral functions and related quantities:

* \f$k\f$-resolved spectral functions \f$A(k, \omega)\f$.
* orbital- and atom-resolved \f$k\f$-summed spectral function \f$A_{mm'}(\omega)\f$.

Plot-ready output to help you analyze and publish results.

## Utilities
Extra tools that make life easier:

* @ref checkpoint - No more boilerplated HDF5 code. The checkpoint object saves DMFT calculation data (Green's functions, self-energies, etc.) in a clean, standardized HDF5 layout -- automatically organized by iteration.
* @ref root
* @ref deg - Analyze and symmetrize degenerate Green's function blocks.
* @ref double_counting - DFT+DMFT requires subtracting a "double counting" term \f$\Sigma_{\mathrm{DC}}\f$ to avoid counting static correlations twice. ModEST abstracts this into a dedicated solver that computes \f$\Sigma_{\mathrm{DC}}\f$ based on direct formulas.
