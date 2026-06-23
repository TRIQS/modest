**ModEST** is a TRIQS application written in C++ with a Python interface, designed as a toolkit that extends the modular nature of TRIQS
with an API specifically tailored for developing quantum embedding calculations in connection with realistic electronic structure codes. Some fo the key features include:

* Generic representation of DFT data (one-body elements).
* Powerful embedding description to articulate generic DMFT embedding scenarios.
* High-performant implementations of key functions: local Green's function, chemical potential search, lattice density, charge-density correction, etc.

The supported DFT codes are: Wien2k, VASP, Quantum ESPRESSO, Elk, AbInit, and Wannier90. Quantum ESPRESSO and AbInit are interfaced via Wannier90. For more details on the specific electronic interfaces, please see [TRIQS/dftkit](https://github.com/TRIQS/dftkit).