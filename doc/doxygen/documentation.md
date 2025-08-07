@page documentation API Documentation

[TOC]

The Modular Electronic Structure Toolkit (ModEST) provides the necessary tooling to perform DMFT-type calculations in the context of electronic structure including density-functional theory (DFT) and many-body perturbation theory (MBPT).

Modularity is achieved through four key abstractions
* one-body elements,
* embedding,
* k-summation

These abstractions let you describe complex DMFT problems -- from simple single-site DMFT to multi-impurity, spin-orbit, or cluster DMFT --in a clean and modular way.

Efficiency and performance are achieved via a modern C++ implementation, and all functionality is fully exposed in Python, making it possible to prototype and run production calculations in either language.

If you are looking for a specific function, class, etc., try using the search bar in the top left corner.

--- 

## One-body elements
@ref one_body_elements (obe for short) contains all of the one-body data that is typically converted from a density-functional theory (DFT) calculation or Wannier90 calculation. The one-body elements struct contains four components (@ref obe_components) :

- @ref local_space defines the local correlated subspace to be solved within DMFT.
- @ref band_dispersion contains the band dispersion \f$\varepsilon(k, \nu, \sigma)\f$  where \f$\varepsilon\f$ is the energy at momentum k, band index \f$\sigma\f$, and spin \f$\sigma\f$.
- @ref downfolding_projector contains the projector which downfolds from Bloch space 𝓑 (indexed by \f$\nu\f$) to the correlated space (index by \f$m\f$).    
- @ref ibz_symmetry_ops contains the operations to symmetrize observables over the full Brillouin zone.

To make it easy to build this object, ModEST provides factory functions that convert raw DFT data (from codes like VASP, Wien2k, or Wannier90) into fully prepared OBE objects ready for DMFT.


## Embedding
The **embedding** object is the central controller of the DMFT problem: it defines how the correlated subspace (\f$\mathcal{C}\f$) is partitioned into impurityies and handles the bookkeeping needed to map back and forth between the lattice and the impurities.

Typical use cases covered by embedding:

- **Equivalent atoms**: use the same impurity model (i.e. same \f$n_{\mathrm{imp}}\f$) for different blocks $\alpha$.
- **Magnetic order**: just reverse the spin for some atoms, \f$\tau(\sigma) = 1-\sigma\f$
- **Splitting impurity models** : Depending on the block structure of the problem, the corresponding impurity model connected to atom $a$ can be *split*. Different impurity solvers can then be used for each of these split impurity models. The self-energies from both models map back to the a single atom $a$. Or a trivial solver is used to obtain zero self-energy (*drop*).
- **cluster DMFT**: re-group a subset of atoms $a$ into a larger *super atom* which will be treated as a cluster DMFT problem.

The embedding object provides:
* @ref embedding_factories -- convenient ways to construct common embeddings.
* @ref embedding_ops -- operations to adapt and modify an embedding (e.g., split, drop, etc.).
* @ref embedding_methods -- methods to map data between impurity and embed spaces.

This abstrction simplifies the DMFT loop, making it easy to construct arbitrary embedding scenarios.

## Local Green's function
A central quantity of the DMFT self-consistency condition -- the local Green's function defined as
 $$ [ G_{\mathrm{loc}}^{\sigma} ]_{m m'} = \sum_{\mathbf{k}} P_{m\nu}^{\sigma}(\mathbf{k}) \Big  [ (\omega + \mu)\delta_{\nu\nu'} - H^{\sigma}_{\nu\nu'}(\mathbf{k}) - [P_{m\nu}^{\sigma}]^{\dagger}\Sigma_{\mathrm{embed}}P_{m'\nu'}^{\sigma}(\mathbf{k}) \Big ]^{-1} [P_{m'\nu'}^{\sigma}]^{\dagger},$$
 where \f$\omega\f$ is a frequency (either real- or Matsubra), \f$\mu\f$ is the chemical potential, \f$H(\mathbf{k})\f$ is the one-body Hamiltonian, \f$P(\mathbf{k})\f$ are the projectors from the band to the orbital basis, and \f$\Sigma_{\mathrm{embed}}\f$ is the embedded self-energy.

 ModEST computes this efficiently:

 * **Woodbury**: reduces the cost of matrix inversion from cubic in the number of bands to linear.
 * **Adaptive Brillouin zone integration**: for tight-binding models, allows you to specify desired integration accuracy, improving predictions of transport properties.

## Hybridization function
The hybridization function \f$\Delta(\omega)\f$ is a key input to TRIQS impurity solvers. It describes the coupling of each impurity to the bath:

$$ [\Delta^{\sigma}(\omega)]_{mm'} = \omega - \tilde{\varepsilon} - G_{\mathrm{loc}}^{-1}(\omega) - \Sigma_{\mathrm{imp}}(\omega),$$

where \f$\tilde{\varepsilon} = \varepsilon - \mu - \Sigma_{\mathrm{DC}}\f$.


## Densities and Chemical Potentials
Calculate the total electronic density from the lattice Green's function and search for the chemical potential \f$\mu\f$ that gives a target electorn count. We provide several algorithms optimized for speed and robusness. Due to the modularity you can also leverage exteranl root finding routines to write a custom chemical potential search.

## Interaction Hamiltonians
To describe local interactions, ModEST provides tools to construct standard interaction Hamiltonians:
* @ref hubb_kan -- Hubbard-Kanamori with density-density, spin-flip, and pair-hopping terms.
* @ref slater -- full rotationally invariant Slater Hamiltonian.
* @ref den_den -- density-density only Hamiltonian.

All built using TRIQS many-body operators.


This design makes it easier to swap and test different double counting schemes.

## Post-processing
Compute spectral functions and related quantities:
* \f$k\f$-resolved spectral functions \f$A(k, \omega)\f$.
* orbital- and atom-resolved \f$k\f$-summed spectral function \f$A_{mm'}(\omega)\f$.

Plot-ready output to help you analyze and publish results.

## Utilities
Extra tools that make life easier:

### File I/O
No more boilerplated HDF5 code. 

The checkpoint object saved DMFT calculation data (Green's functions, self-energies, etc.) in a clean, standardized HDF5 layout -- automatically organized by iteration.

### Root finders
@ref root

### Green's function symmetries
@ref deg
analyze and symmetrize degenerate Green's function blocks.

### Double counting
@ref double_counting
DFT+DMFT requires subtracting a "double counting" term \f$\Sigma_{\mathrm{DC}}\f$ to avoid counting static correlations twice. ModEST abstracts this into a dedicated solver that computes \f$\Sigma_{\mathrm{DC}}\f$ based on dirrect formulas.