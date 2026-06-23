.. _reference/python/embedding:

Embedding
*********

The **embedding** object is the central controller of the DMFT problem:
it defines how the correlated subspace :math:`\mathcal{C}` is partitioned
into impurities and handles the bookkeeping needed to map back and forth
between the lattice and the impurities at every DMFT iteration.

Mapping from impurities to :math:`\mathcal{C}`
==============================================

An embedding is specified by a function

.. math::

   \psi : (\alpha, \sigma) \;\longrightarrow\;
          (n_{\mathrm{imp}}, \gamma, \tau),

that assigns to each embedded block :math:`(\alpha, \sigma)` in
:math:`\mathcal{C}` an impurity model :math:`n_{\mathrm{imp}}`, an
impurity block :math:`\gamma`, and a block-diagonal index
:math:`\tau`. The corresponding embedded self-energy is then

.. math::

   [\Sigma_{\mathrm{embed}}]^{\sigma}_{(\alpha, m_{\alpha}),
                                       (\alpha, m_{\alpha}')}
   = [\Sigma_{\mathrm{imp}}]^{n_{\mathrm{imp}}(\alpha),\,\tau(\sigma)}_{
       (\gamma(\alpha), m_{\alpha}),\,(\gamma(\alpha), m_{\alpha}')}.

See :ref:`userguide_theory_and_notation` for the full notation and
derivation.

Typical use cases covered by the embedding object:

* **Equivalent atoms** — use the same impurity model (same
  :math:`n_{\mathrm{imp}}`) for different blocks :math:`\alpha`.
* **Magnetic order** — reverse the spin for some atoms,
  :math:`\tau(\sigma) = 1 - \sigma`.
* **Splitting impurity models** — depending on the block structure, the
  impurity model connected to atom :math:`a` can be *split*. Different
  impurity solvers can then be used for each of these split impurity
  models, with the self-energies mapped back to a single atom :math:`a`.
  A trivial solver can also be used to obtain a zero self-energy
  (``drop_imp``).
* **Cluster DMFT** — regroup a subset of atoms :math:`a` into a larger
  super-atom treated as a cluster DMFT problem.

The embedding object provides:

* Factory functions to conveniently construct common embeddings.
* Operations to adapt and modify an embedding (e.g. ``split_imp``,
  ``drop_imp``).
* Methods to map data between impurity and embed spaces.

This abstraction simplifies the DMFT loop, making it easy to construct
arbitrary embedding scenarios.

.. autosummary::

   triqs_modest.embedding.Embedding
   triqs_modest.embedding.make_embedding
