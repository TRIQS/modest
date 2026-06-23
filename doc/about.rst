.. _about:

About ModEST
************

What ModEST is
==============

**Mod** ular **E** lectronic **S** tructure **T** oolkit (ModEST) is the electronic-structure extension pack to the
`TRIQS <https://triqs.github.io>`_ library. It provides the tooling needed to
build *generic quantum-embedding calculations* — DMFT and its extensions — in
direct connection to electronic-structure methods such as density-functional
theory (DFT) and many-body perturbation theory (MBPT).

ModEST is aimed at two audiences: electronic-structure practitioners who want to
plug a DFT (or GW) backend into a DMFT loop, and DMFT practitioners who want a
uniform interface across different DFT codes, tight-binding models, and embedding
scenarios. 

What ModEST does
================

ModEST is organized around three primary use cases of quantum embedding calculations:

1. **Tight-binding** — a tight-binding model is derived from a DFT band
   structure (typically through Wannier90), and a DMFT calculation is built
   directly on top of that dispersion. This is the lightest-weight entry point
   and is one flavor of dynamical mean-field calculations in combination with electronic structure.

2. **Charge self-consistent DFT+DMFT (CSC)** — ModEST exposes interfaces to
   several DFT codes (VASP, Quantum ESPRESSO, Wien2k, Elk, AbInit, Wannier90),
   making it possible to write charge self-consistent DFT+DMFT implementations
   in which the electronic density is updated by the DMFT impurity solution and
   fed back into the DFT cycle. 

3. **Library mode** — ModEST is fundamentally a library that other
   projects can consume. A notable example is
   `CoQui <https://github.com/AbInitioQHub/coqui/>`_, which targets GW+EDMFT in
   conjunction with TRIQS.

Across all three modes the same primitives are reused: load a one-body-elements object, 
describe the correlated subspace and its mapping to impurities with an embedding, 
and compute k-summed observables (local Green's function, charge
self-consistency correction). Double counting, interaction Hamiltonians,
spectral-function post-processing, and HDF5 checkpointing complete the
picture.

To see ModEST's building blocks used end-to-end, continue to the
:ref:`User guide <userguide>`.
