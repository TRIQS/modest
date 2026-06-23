.. _reference_python:

Python API
**********

The Python API covers everything needed to assemble a realistic DMFT
calculation: building one-body elements from a DFT or tight-binding
input, describing the correlated subspace and its mapping to impurities
through an embedding, running the DMFT loop (local Green's function,
chemical-potential search, hybridization, impurity levels), applying
double counting and charge self-consistency, computing spectral
functions, and checkpointing intermediate state. The pages below are
grouped by *what the user is doing* rather than by C++ wrapping module,
so related functions sit together regardless of which submodule they
live in.

For a derivation of the underlying equations and the conventions used
throughout the code, see :ref:`userguide_theory_and_notation`. For
worked end-to-end examples of these APIs in use, see the
:ref:`tutorials`.

.. toctree::
   :maxdepth: 1

   one_body_elements
   embedding
   interactions
   dmft_loop
   double_counting
   csc
   post_processing
   gf_utils
   obe_utils
   checkpointing
