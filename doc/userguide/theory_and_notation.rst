.. _userguide_theory_and_notation:

Theory and notation
*******************

This page collects the notations and equations behind the DFT + DMFT
implementation in ModEST and the general organization of that implementation.

.. note::

   Status: in progress.

Notation and basic equations
============================

Bloch, Wannier and correlated spaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The DFT + DMFT equations involve three different spaces, following (mostly)
the notations of S. Beck *et al.*:

* The *(reduced) Bloch space* :math:`\mathcal{B}` (denoted :math:`\mathcal{R}`
  in Beck *et al.*) contains bands of dispersion
  :math:`\varepsilon_{\nu}^{\sigma}(\mathbf{k})` in some window of energy.
  Here :math:`\nu` is the band index (:math:`0 \leq \nu < N_\nu^{\mathbf{k}}`)
  and :math:`\mathbf{k}` is a point in the Brillouin zone. We define
  :math:`N_\nu \equiv \max_{\mathbf{k}} N_\nu^{\mathbf{k}}`.

* The *Wannier space* :math:`\mathcal{W}` is spanned by Wannier functions
  constructed from :math:`\mathcal{B}`.

* The *correlated space* :math:`\mathcal{C} \subseteq \mathcal{W}` contains
  :math:`M` Wannier orbitals and is the subspace in which the self-energy
  is approximated by the embedding. :math:`\mathcal{C}` is spanned by Wannier
  functions at several atoms/sites with index :math:`a` at position
  :math:`R_a` and orbital/Wannier index :math:`m_a`. It is indexed by a
  *composite index* :math:`m = (a, m_a)`, with :math:`0 \leq m \leq M-1`
  and :math:`M = \sum_{a} \max(m_a)`. The main equations are written with
  the composite index :math:`m` because the :math:`m = (a, m_a)`
  decomposition is in general not appropriate for embeddings.

The σ index
~~~~~~~~~~~

The :math:`\sigma` index is a general *block-diagonal index*. In simple
cases it is the spin index, but not always.

* In "spin (non-)polarized" computations, :math:`\sigma` is the spin index.
* In spin-orbit or Nambu computations, the spin index is merged with
  :math:`m` and :math:`\nu`, so :math:`\sigma = 0` (i.e. a single value of
  the index, equivalent to no index at all).

Projectors
~~~~~~~~~~

The projectors :math:`P_{m\nu}^{\sigma}(\mathbf{k})` connect the Bloch
space :math:`\mathcal{B}` to :math:`\mathcal{C}`. They are obtained from
DFT codes or Wannier90, and are defined by

.. math::

   P_{(a, m_a)\,\nu}^{\sigma}(\mathbf{k}) \equiv
   e^{-i \mathbf{k} \cdot R_a}\,
   \langle \chi_{m_a}^{R_a\, \sigma} | \psi_{\nu}^{\sigma}(\mathbf{k}) \rangle,

where :math:`|\chi_{m_a}^{R_a\, \sigma}\rangle` is a Wannier function
localized at atom :math:`a` with index :math:`m_a` at position :math:`R_a`,
and :math:`|\psi_{\nu}^{\sigma}(\mathbf{k})\rangle` is the Kohn–Sham
wavefunction. The relation between the Wannier and Bloch functions is

.. math::

   |\chi_{m_a}^{R_a\, \sigma}\rangle =
   \sum_{\mathbf{k}, \nu} e^{-i \mathbf{k} \cdot R_a}
   \bigl(P_{(a, m_a)\,\nu}^{\sigma}(\mathbf{k})\bigr)^{*}
   |\psi_{\nu}^{\sigma}(\mathbf{k})\rangle.

**Properties.**

* *Basis change in* :math:`\mathcal{C}` *space.* Under a unitary
  transformation :math:`U` the projector transforms as

  .. math::
     :label: eq_ChangeBasisP

     P_{m\nu}^{\prime\, \sigma}(\mathbf{k}) =
     U^{\dagger}_{m, m'}\, P_{m'\nu}^{\sigma}(\mathbf{k}).

* *Partial unitarity.* In general :math:`P` is not unitary because
  :math:`N_\nu^{\mathbf{k}} > M`. However, if the Wannier functions are
  reorthonormalized with respect to the truncated band basis, then

  .. math::
     :label: eq_PPdag1

     \sum_{\nu} P_{m\nu}^{\sigma}(\mathbf{k})\,
     P_{\nu m'}^{\dagger\, \sigma}(\mathbf{k}) = \delta_{m m'}.

DMFT equations
~~~~~~~~~~~~~~

The Green's function in the Bloch space is

.. math::
   :label: eq_lattG

   \bigl[G_{\mathcal{B}}^{\sigma}\bigr]^{-1}_{\nu\nu'}(\mathbf{k}, \omega)
   = \bigl(\omega + \mu - \varepsilon_{\nu}^{\sigma}(\mathbf{k})\bigr)
   \delta_{\nu\nu'}
   - \bigl[\Sigma_{\mathcal{B}}^{\sigma}\bigr]_{\nu\nu'}(\mathbf{k}, \omega),

where :math:`\Sigma_{\mathcal{B}}^{\sigma}` is the upfolded self-energy,
:math:`\mu` is the chemical potential, and :math:`\omega` is the frequency
(real or Matsubara). Additional terms such as a magnetic field :math:`h`
can be added.

The upfolded self-energy :math:`\Sigma_{\mathcal{B}}^{\sigma}` is obtained
by upfolding the embedded self-energy from :math:`\mathcal{C}`:

.. math::
   :label: eq_lattSigma

   \bigl[\Sigma_{\mathcal{B}}^{\sigma}\bigr]_{\nu\nu'}(\mathbf{k}, \omega)
   = P_{\nu m}^{\dagger\, \sigma}(\mathbf{k})\,
     \bigl[\Sigma_{\mathcal{C}}^{\sigma}(\omega)\bigr]_{m m'}\,
     P_{m' \nu'}^{\sigma}(\mathbf{k}),

.. math::

   \Sigma_{\mathcal{C}}^{\sigma}(\omega) \equiv
   \Sigma_{\mathrm{embed}}^{\sigma}(\omega),

where :math:`\Sigma_{\mathrm{embed}}` is the embedded self-energy — in
simple cases just the impurity self-energy :math:`\Sigma_{\mathrm{imp}}`,
in general built from several impurity self-energies (see below) — and
:math:`\Sigma_{\mathrm{DC}}` is the double-counting correction.

The local Green's function in the correlated space :math:`\mathcal{C}` is
the projection of :math:`G_{\mathcal{B}}^{\sigma}` onto :math:`\mathcal{C}`
with :math:`P`, summed over the Brillouin zone:

.. math::
   :label: eq_locG

   \bigl[G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}(\omega)\bigr]_{m m'}
   = \sum_{\mathbf{k}, \nu \nu'} P_{m\nu}^{\sigma}(\mathbf{k})\,
     \bigl[G_{\mathcal{B}}^{\sigma}\bigr]_{\nu\nu'}(\mathbf{k}, \omega)\,
     P_{\nu' m'}^{\dagger\, \sigma}(\mathbf{k}).

The sum over the BZ is normalized, i.e. :math:`\sum_{\mathbf{k}} 1 = 1`.

Projected equations in the correlated space
-------------------------------------------

In general :math:`N_\nu > M`, often :math:`N_\nu \gg M`, so the self-energy
in :eq:`eq_lattSigma` is a low-rank matrix. It is therefore both possible
and advantageous to perform all operations directly in :math:`\mathcal{C}`
rather than in :math:`\mathcal{B}`.

We define the *projected bare (Kohn–Sham) Green's function* in
:math:`\mathcal{C}` as

.. math::
   :label: def_G0W

   \bigl[G_{\mathcal{C}, 0}^{\sigma}(\mathbf{k}, \omega)\bigr]_{m m'}
   \equiv \sum_{\nu} P_{m\nu}^{\sigma}(\mathbf{k})\,
     \frac{1}{\omega + \mu - \varepsilon_{\nu}^{\sigma}(\mathbf{k})}\,
     P_{\nu m'}^{\dagger\, \sigma}(\mathbf{k}).

Using the Woodbury identity (see :ref:`appendix_woodbury`), :eq:`eq_locG`
can be rewritten as a matrix equation in :math:`\mathcal{C}`:

.. math::
   :label: eq_DysonW

   G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}(\omega)
   = \sum_{\mathbf{k}}
   \bigl[
     G_{\mathcal{C}, 0}^{\sigma}(\mathbf{k}, \omega)^{-1}
     - \Sigma_{\mathcal{C}}^{\sigma}(\omega)
   \bigr]^{-1}
   = \sum_{\mathbf{k}}
   \bigl[
     1 - G_{\mathcal{C}, 0}^{\sigma}(\mathbf{k}, \omega)\,
     \Sigma_{\mathcal{C}}^{\sigma}(\omega)
   \bigr]^{-1}\,
   G_{\mathcal{C}, 0}^{\sigma}(\mathbf{k}, \omega).

This formula is general — it holds *even if* :math:`P` *is not unitary*.
The inversions are performed in :math:`\mathcal{C}`, which is very
advantageous when :math:`N_\nu \gg M`: the cost is :math:`O(N_\nu)` for
the assembly of :math:`G_{\mathcal{C}, 0}` instead of :math:`O(N_\nu^3)`
in the original formulation :eq:`eq_locG`. Note however that the
inversions are done in the *whole* :math:`\mathcal{C}` space — the block
structure is dictated by :math:`G_{\mathcal{C}, 0}` (via symmetries) and
not by the atom structure (:math:`a` index) nor by the (more refined)
block structure of :math:`\Sigma_{\mathcal{C}}`, which is a *choice* of
embedding. See :ref:`block_structure_correlated` for the block structure
in :math:`\mathcal{C}`.

In the special case when :math:`P` is unitary (isolated bands),

.. math::

   \bigl[G_{\mathcal{C}, 0}^{\sigma}(\mathbf{k}, \omega)^{-1}\bigr]_{m m'}
   = (\omega + \mu)\delta_{m m'}
   - \sum_{\nu} P_{m\nu}^{\sigma}(\mathbf{k})\,
     \varepsilon_{\nu}(\mathbf{k})\,
     P_{\nu m'}^{\dagger\, \sigma}(\mathbf{k}),

and we recover a "model" in the correlated space :math:`\mathcal{C}` with
a bare propagator parametrized by an effective dispersion
:math:`\bar{\varepsilon} \equiv P \varepsilon P^{\dagger}`.

In the general (non-unitary) case the open question is the form of
:math:`G_{\mathcal{C}, 0}^{\sigma}(\mathbf{k}, \omega)^{-1}` and in
particular how to interpolate it in :math:`\mathbf{k}` — e.g. whether a
low-rank decomposition in :math:`(\omega, \mathbf{k})` still exists.
This would enable adaptive :math:`k`-sums in the general case, bypassing
the need to interpolate the projectors :math:`P` directly. Any
:math:`\mathbf{k}`-dependent phase ambiguity in :math:`P` is eliminated
in :math:`G_{\mathcal{C}, 0}`.

Number of electrons
~~~~~~~~~~~~~~~~~~~

The density is defined by

.. math::
   :label: eq_deflattdensity

   n^{\sigma}(\mathbf{k}) \equiv
   \frac{1}{\beta} \sum_{n} \sum_{\nu}
   \bigl[G_{\mathcal{B}}^{\sigma}\bigr]_{\nu\nu}(\mathbf{k}, i\omega_n)\,
   e^{i \omega_n 0^{+}}.

It can also be computed in :math:`\mathcal{C}` (see
:ref:`appendix_woodbury`). The trace of :math:`G_{\mathcal{B}}^{\sigma}`
can be rewritten as a correction to the trace of the Kohn–Sham Green's
function :math:`G_{\mathrm{KS}}^{\sigma}`:

.. math::
   :label: eq_TrGcvv

   \mathrm{Tr}_{\nu}\, G_{\mathcal{B}}^{\sigma}(\mathbf{k}, i\omega_n)
   = \mathrm{Tr}_{\nu}\, G_{\mathrm{KS}}^{\sigma}(\mathbf{k}, i\omega_n)
   - \mathrm{Tr}\Bigl(
     \Sigma_{\mathcal{C}}^{\sigma}(i\omega_n)\,
     \bigl[1 - G_{\mathcal{C}, 0}(\mathbf{k}, i\omega_n)\,
            \Sigma_{\mathcal{C}}^{\sigma}(i\omega_n)\bigr]^{-1}\,
     \partial_{\mu} G_{\mathcal{C}, 0}(\mathbf{k}, i\omega_n)
   \Bigr),

where the second trace is taken in :math:`\mathcal{C}` and

.. math::
   :label: def_GKS

   G_{\mathrm{KS};\,\nu\nu'}^{\sigma}(\mathbf{k}, \omega) \equiv
   \frac{1}{\omega + \mu - \varepsilon_{\nu}^{\sigma}(\mathbf{k})}
   \delta_{\nu\nu'}.

This yields

.. math::
   :label: eq_densityW

   n^{\sigma}(\mathbf{k}) = n_{\mathrm{KS}}^{\sigma}(\mathbf{k})
   - \frac{1}{\beta} \sum_{n} e^{i \omega_n 0^{+}}
   \mathrm{Tr}\Bigl(
     \Sigma_{\mathcal{C}}^{\sigma}(i\omega_n)\,
     \bigl[1 - G_{\mathcal{C}, 0}(\mathbf{k}, i\omega_n)\,
            \Sigma_{\mathcal{C}}^{\sigma}(i\omega_n)\bigr]^{-1}\,
     \partial_{\mu} G_{\mathcal{C}, 0}(\mathbf{k}, i\omega_n)
   \Bigr),

with the bare Kohn–Sham density

.. math::

   n_{\mathrm{KS}}^{\sigma}(\mathbf{k}) \equiv
   \sum_{\nu} n_{F}\bigl(\varepsilon_{\nu}^{\sigma}(\mathbf{k}) - \mu\bigr),

where :math:`n_F` is the Fermi function.

Remarks:

* This computation is performed many times while adjusting :math:`\mu` in
  the DMFT loop, so it is important to optimize.
* The correction term is a difference of Green's functions
  (:math:`G_{\mathcal{B}} - G_{\mathrm{KS}}`); it can therefore be
  evaluated on the frequency DLR mesh, transformed to imaginary time, and
  its density read off at :math:`\tau = 0^{-}`.
* As before, once :math:`G_{\mathcal{C}, 0}` is computed, the whole
  calculation lives in :math:`\mathcal{C}`.
* In general :math:`\sum_{\mathbf{k}} n^{\sigma}(\mathbf{k}) \neq
  \mathrm{Tr}\, G_{\mathrm{loc}}` unless :math:`P` is unitary
  (:math:`P^{\dagger} P = 1`), as follows from :eq:`eq_locG` or
  :eq:`eq_TrGcvv`.

.. _block_structure_correlated:

Block structure of the correlated space
---------------------------------------

Definitions
~~~~~~~~~~~

The projected bare Green's function :math:`G_{\mathcal{C}, 0}^{\sigma}`
and the self-energy :math:`\Sigma_{\mathcal{C}}^{\sigma}` carry a block
structure in :math:`\mathcal{C}`. The block structure of
:math:`G_{\mathcal{C}, 0}` (if any) is given by symmetry (irreps); we do
not consider it yet at this stage. The block structure of
:math:`\Sigma_{\mathcal{C}}^{\sigma}` reflects a *choice* of embedding:
the self-energy can have blocks corresponding to atoms, larger blocks
corresponding to clusters, or smaller blocks when each atom's orbitals
are decomposed by symmetry.

:math:`\mathcal{C}` can be decomposed in many different ways: its
composite index :math:`m` admits various subdivisions
:math:`m = (\gamma, m_{\gamma})` with
:math:`\sum_{\gamma} \max(m_{\gamma}) = M`. The block decomposition
associated with the self-energy is called the *embedded decomposition*
and denoted :math:`m = (\alpha, m_{\alpha})`. Examples:

* *Atomic decomposition:* :math:`m = (a, m_a)`, with :math:`a` labeling
  atoms and :math:`m_a` the orbital of atom :math:`a`.
* *Embedded decomposition for single-site DMFT with symmetry:* each atom
  is decomposed into smaller blocks given by its irreps.
* *Embedded decomposition for cluster DMFT:* several atoms in the unit
  cell are grouped into a cluster.

Clusters spanning atoms in the unit cell amount to choosing a different
decomposition of :math:`\mathcal{C}`. For clusters spanning atoms in
*different* unit cells, the one-body problem is transformed into a
superlattice and a proper decomposition in the supercell is used.

Basis preparation
~~~~~~~~~~~~~~~~~

The strategy is to decompose :math:`\mathcal{C}` using a basis adapted
to embedding. Each block of the embedded self-energy is then mapped to
the corresponding impurity-model self-energy (see :ref:`embedding`).

The projectors come from the DFT code or Wannier90 in some global
coordinate system of the crystal. Two basis transformations are required
before performing the embedding:

#. A coordinate-system rotation :math:`R^{a}_{m_a, m_a'}` from the global
   coordinate system into the local coordinate system of atom (site)
   :math:`a`. This rotation ensures that equivalent atoms have the same
   self-energy in the new basis and can therefore be solved by the same
   impurity model.

#. Optionally, a second rotation :math:`U` that infers the irreps from
   the local Hamiltonian. At this stage the proper irrep information is
   not retrieved from the electronic-structure code, so we examine the
   local non-interacting Hamiltonian

   .. math::

      [H_{\mathrm{loc}}^{0}]_{m_a m_a'}^{a, \sigma} \equiv
      \sum_{\mathbf{k}} P_{(a, m_a)\,\nu}^{\sigma}(\mathbf{k})\,
      \varepsilon_{\nu\nu'}^{\sigma}(\mathbf{k})\,
      \bigl[P_{(a', m_a')\,\nu'}^{\sigma}(\mathbf{k})\bigr]^{\dagger}.

   Its block structure is inferred up to a user-defined threshold by
   finding a permutation of the orbitals :math:`m_a` that renders
   :math:`H_{\mathrm{loc}}^{0}` block-diagonal. Optionally, these smaller
   blocks of the local non-interacting Hamiltonian can be diagonalized
   (to reduce the off-diagonal elements of the impurity-model
   hybridization functions :math:`\Delta_{m m'}^{\sigma}`).

The Wannier basis is therefore transformed as in :eq:`eq_ChangeBasisP`:

.. math::

   P_{m\nu}^{\sigma}(\mathbf{k}) \leftarrow (R U)^{\dagger}_{m, m'}\,
   P_{m'\nu}^{\sigma}(\mathbf{k}).

See :ref:`appendix_ptilde` for the full derivation.

.. note::

   In previous codes, all rotations were performed during the embedding.
   Here we instead prepare the basis up front to absorb them, which
   makes the embedding itself much simpler — effectively, we work in a
   "local coordinate" basis in :math:`\mathcal{C}`.

.. _embedding:

Embedding
---------

The embedded self-energy is constructed from :math:`n_{\mathrm{imp}}`
impurity-model self-energies. We denote by
:math:`[\Sigma_{\mathrm{imp}}]^{n_{\mathrm{imp}}, \tau}` the self-energy
of impurity model :math:`n_{\mathrm{imp}}` with block index :math:`\tau`.
The embedding is

.. math::
   :label: eq_embSigma

   [\Sigma_{\mathrm{embed}}]^{\sigma}_{(\alpha, m_{\alpha}),
                                       (\alpha, m_{\alpha}')}
   = [\Sigma_{\mathrm{imp}}]^{n_{\mathrm{imp}}(\alpha),\, \tau(\sigma)}_{
       (\gamma(\alpha), m_{\alpha}),\, (\gamma(\alpha), m_{\alpha}')}.

In words, the self-energy of block :math:`(\alpha, \sigma)` is taken
from impurity self-energy :math:`n_{\mathrm{imp}}(\alpha)` in block
:math:`(\gamma(\alpha), \tau(\sigma))`. This requires that the embedding
blocks :math:`\alpha` and the impurity blocks :math:`\gamma` have the
same dimensions. The embedding is therefore fully characterized by a
function

.. math::
   :label: eq_embSigmaFunction

   \psi : (\alpha, \sigma) \;\longrightarrow\;
          (n_{\mathrm{imp}}, \gamma, \tau).

This covers, in particular:

* **Equivalent atoms** — use the same impurity model (same
  :math:`n_{\mathrm{imp}}`) for different blocks :math:`\alpha`.
* **Antiferromagnetism** — reverse the spin for some atoms,
  :math:`\tau(\sigma) = 1 - \sigma`.
* **Splitting impurity models** — depending on the block structure, the
  impurity model connected to atom :math:`a` can be *split* and solved
  with different impurity solvers, with self-energies mapped back to
  atom :math:`a`. A trivial solver can also be used to obtain a zero
  self-energy ("drop").
* **Cluster DMFT** — regroup a subset of atoms :math:`a` into a larger
  "super atom" treated as a cluster DMFT problem.
* **Arbitrary magnetic orders** (spirals, …).

All rotations (e.g. local-to-global coordinate systems) have been
absorbed into the projector :math:`P` during basis preparation, which is
what makes the embedding itself simple: first prepare the basis adapted
to the embedding, then perform the embedding.

Appendices
==========

.. _appendix_woodbury:

Derivation of the projected equations in the correlated space
-------------------------------------------------------------

Derivation of the local Green's function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We use the *Woodbury formula*: if :math:`D` is an invertible
:math:`n \times n` matrix (here diagonal, but not necessarily so), and
:math:`U`, :math:`V` are :math:`n \times p` and :math:`p \times n`
matrices respectively (:math:`p < n`), then

.. math::

   (D - U V)^{-1} = D^{-1}
   + D^{-1} U \bigl(1 - V D^{-1} U\bigr)^{-1} V D^{-1}.

The inversion is now performed on a :math:`p \times p` matrix.

Consider the upfolded self-energy :eq:`eq_lattG`. Working at fixed
:math:`\omega, \mathbf{k}, \sigma` (and dropping these labels below),
denote :math:`\Sigma = \Sigma_{\mathcal{C}}^{\sigma}` and define

.. math::

   D_{\nu\nu'} \equiv (\omega + \mu - \varepsilon_{\nu}^{\sigma}(\mathbf{k}))
   \delta_{\nu\nu'}.

Writing the lattice Green's function as :math:`G_{\mathrm{latt}}` for
clarity,

.. math::

   G_{\mathrm{latt}} &= (D - P^{\dagger} \Sigma P)^{-1}, \\
   G_{\mathrm{loc}}  &= P\, G_{\mathrm{latt}}\, P^{\dagger}, \\
   G_{\mathcal{C}, 0} &= P\, D^{-1}\, P^{\dagger}.

With :math:`U = P^{\dagger}`, :math:`V = \Sigma P` and
:math:`Y = G_{\mathcal{C}, 0}`, the Woodbury identity gives

.. math::
   :label: app1_Glattred

   G_{\mathrm{latt}} = D^{-1}
   + D^{-1} P^{\dagger} (1 - \Sigma Y)^{-1} \Sigma P\, D^{-1}.

Hence

.. math::

   G_{\mathrm{loc}} &= P\, G_{\mathrm{latt}}\, P^{\dagger}\\
                    &= Y + Y (1 - \Sigma Y)^{-1} \Sigma Y\\
                    &= Y (1 - \Sigma Y)^{-1}\\
                    &= (Y^{-1} - \Sigma)^{-1},

which is :eq:`eq_DysonW`.

Derivation of the projected density
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Rewrite :eq:`eq_deflattdensity` as

.. math::

   n = \mathrm{Tr}\, G_{\mathrm{latt}},

where the trace runs over the band index :math:`\nu` and Matsubara
frequencies. Taking the trace of :eq:`app1_Glattred`,

.. math::

   n &= \mathrm{Tr}\, G_{\mathrm{latt}}\\
     &= \mathrm{Tr}\, D^{-1}
        + \mathrm{Tr}\bigl(D^{-1} P^{\dagger} (1 - \Sigma Y)^{-1}
                           \Sigma P\, D^{-1}\bigr)\\
     &= \mathrm{Tr}\, D^{-1}
        + \mathrm{Tr}\bigl((1 - \Sigma Y)^{-1} \Sigma P\, D^{-2} P^{\dagger}\bigr)\\
     &= \mathrm{Tr}\, D^{-1}
        + \mathrm{Tr}\bigl((1 - \Sigma Y)^{-1} \Sigma\, (-\partial_{\mu} Y)\bigr)\\
     &= \mathrm{Tr}\, D^{-1}
        + \mathrm{Tr}\bigl(\Sigma\, (1 - Y \Sigma)^{-1}\, (-\partial_{\mu} Y)\bigr),

which is :eq:`eq_densityW`.


Integration in the irreducible Brillouin zone wedge
---------------------------------------------------

For efficiency, the one-body calculation can be performed on the
irreducible Brillouin zone (IBZ). To recover observables like the local
Green's function defined in :eq:`eq_locG`, the result must be
*symmetrized* after summing only on the IBZ. For any observable
:math:`\mathcal{O}`, the unsymmetrized quantity is

.. math::

   [\mathcal{O}^{\sigma}_{m m'}]_{\mathrm{unsymm}}
   = \sum_{\mathbf{k} \in \mathrm{IBZ}} \sum_{\nu\nu'}
     P_{m, \nu}^{\sigma}(\mathbf{k})\,
     O_{\nu\nu'}^{\sigma}(\mathbf{k})\,
     \bigl[P_{m'\nu'}^{\sigma}(\mathbf{k})\bigr]^{\dagger}.

Symmetrization applies all operations :math:`\mathcal{S}` of the
crystallographic space group :math:`\mathcal{G}`:

.. math::

   [\mathcal{O}^{\sigma}_{(a m_a),\, (a' m_a')}]_{\mathrm{symm}}^{\mathcal{G}}
   = \sum_{\mathcal{S} \in \mathcal{G}} \sum_{n_a n_a'}
     \mathcal{D}_{m_a n_a}(\mathcal{S})\,
     \bigl[(\mathcal{O}^{\mathcal{S}^{-1} a,\, \mathcal{S}^{-1} \sigma})_{n_a n_a'}\bigr]_{\mathrm{unsymm}}\,
     \mathcal{D}(\mathcal{S}^{-1})_{n_a' m_a'}.

.. _appendix_ptilde:

Derivation of the fused projector
---------------------------------

In what follows all indices are suppressed; quantities live in either the
Wannier space (indices :math:`m, m'`) or the Bloch space (indices
:math:`\nu, \nu'`) depending on context. Starting from the impurity
self-energies :math:`\Sigma_{\mathrm{imp}}`, the embedded self-energy is

.. math::

   \Sigma^{\mathrm{local}}_{\mathrm{embed}} = U\, \Sigma_{\mathrm{imp}}\, U^{\dagger}.

The OBE projectors are defined in the global coordinate system of the
crystal, so :math:`\Sigma_{\mathrm{embed}}` must be rotated:

.. math::

   \Sigma^{\mathrm{global}}_{\mathrm{embed}}
   = R\, \Sigma^{\mathrm{local}}_{\mathrm{embed}}\, R^{\dagger}.

The embedded self-energy is then upfolded,

.. math::

   \Sigma_{\mathrm{lattice}} = P^{\dagger}\, \Sigma^{\mathrm{global}}_{\mathrm{embed}}\, P,

the local Green's function is obtained from the lattice Green's function,

.. math::

   G_{\mathrm{loc}}^{\mathrm{global}} = \sum_{k} P\, G_{\mathrm{latt}}\, P^{\dagger},

rotated to the local frame,

.. math::

   G_{\mathrm{loc}}^{\mathrm{local}}
   = R^{\dagger}\, G^{\mathrm{global}}_{\mathrm{loc}}\, R,

and finally rotated to the impurity-solver basis,

.. math::

   G_{\mathrm{loc}}^{\mathrm{solver}}
   = U^{\dagger}\, G^{\mathrm{local}}_{\mathrm{loc}}\, U.

All of these operations can be fused into a single projector

.. math::

   \tilde{P} \equiv U^{\dagger} R^{\dagger} P.

For problems requiring symmetrization, the :math:`Q` matrices must
likewise be rotated by :math:`R U`.
