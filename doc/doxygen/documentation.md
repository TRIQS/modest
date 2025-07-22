@page documentation API Documentation

[TOC]

The Modular Electronic Structure Toolkit (modEST) provides the necessary tooling to perform DMFT-type calculations in the context of electronic structure including density-functional theory (DFT) and many-body perturbation theory (MBPT).

Modularity is achieved through four key abstractions
* one-body elements,
* embedding,
* impurities,
* k-summation
which are all described in detail below.

Efficiency and performance are achieved via a low-level C++ implementation of the library that is entirely exposed in Python. Note that all of the functionality can be used in either language C++ or Python.

The following provides a detailed reference documentation grouped into logical units.

If you are looking for a specific function, class, etc., try using the search bar in the top left corner.

--- 

## Abstractions in modEST

### One-body elements

@ref one_body_elements (obe for short) contains all of the one-body data that is typically converted from a density-functional theory (DFT) calculation or Wannier90 calculation. The one-body elements struct contains four components (@ref obe_components) :

- @ref local_space defines the local correlated subspace to be solved within DMFT.
- @ref band_dispersion contains the band dispersion ε(k, ν, σ)  where ε is the energy at momentum k, band index ν, and spin σ.
- @ref downfolding_projector contains the projector which downfolds from Bloch space 𝓑 (indexed by ν) to the correlated space (index by m).    
- @ref ibz_symmetry_ops contains the operations to symmetrize observables over the full Brillouin zone.

We provide factory functions (@ref obe_factories) to construct different obes for different contexts (calculations or postprocessing) from DFT data prepared using one of our [DFT converters](https://triqs.github.io/dft_tools/latest/_python_api/triqs_dft_tools.converters.html).

### Embedding
