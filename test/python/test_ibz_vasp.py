"""
Integration test: DMFT on the irreducible Brillouin zone (IBZ) for VASP input.

The dftkit VASP converter can emit either the unfolded full k-grid (use_ibz=False)
or the irreducible BZ with the correlated-shell symmetry operations stored in
dft_symmcorr_input (use_ibz=True). When modest reads the IBZ archive it weights
the k-sum by the IBZ multiplicities and symmetrizes the resulting local quantity
over the space group, which must reproduce the full-grid result.

This test loads both archives for two systems and checks that the local Green's
function computed by modest agrees:

  * SrVO3   - single V, cubic O_h, t2g (l=2)            -> ~1e-7
  * AFM NiO - two equivalent Ni (full d, l=2) + two O (p, l=1),
              non-cubic cell, spin-averaged              -> ~1e-5

The looser NiO tolerance reflects VASP's intrinsic projector precision at
degenerate bands (the full-grid projectors are only symmetric to ~1e-6); the
symmetrized IBZ result is, if anything, more symmetry-correct.
"""

import unittest
import numpy as np

from triqs.gfs import MeshImFreq
import triqs_modest as M
from triqs_modest import one_body_elements_from_dft_converter

BETA = 20.0
N_IW = 80


def gloc_blocks(filename, mu):
    """Local Green's function (per impurity block) from a converter archive."""
    target_density, obe = one_body_elements_from_dft_converter(filename)
    E = M.make_embedding(obe.C_space)
    mesh = MeshImFreq(beta=BETA, statistic="Fermion", n_iw=N_IW)
    G = E.extract(M.local_gf.gloc(mesh, obe, mu))
    return [g[0].data.copy() for g in G]


class TestIBZVasp(unittest.TestCase):

    def _compare(self, full_h5, ibz_h5, mu, tol):
        g_full = gloc_blocks(full_h5, mu)
        g_ibz = gloc_blocks(ibz_h5, mu)
        self.assertEqual(len(g_full), len(g_ibz),
                         "different number of impurity blocks")
        diff = max(np.max(np.abs(a - b)) for a, b in zip(g_full, g_ibz))
        self.assertLess(diff, tol,
                        f"IBZ Gloc deviates from full grid by {diff:.2e} (> {tol:.0e})")

    def test_svo_ibz_matches_full(self):
        # cubic, single correlated atom, t2g
        self._compare("svo_vasp_full.ref.h5", "svo_vasp_ibz.ref.h5",
                      mu=0.0, tol=1e-7)

    def test_nio_afm_ibz_matches_full(self):
        # non-cubic, two equivalent Ni (full d) + two O (p), spin-averaged
        self._compare("nio_afm_vasp_full.ref.h5", "nio_afm_vasp_ibz.ref.h5",
                      mu=0.3, tol=1e-5)


if __name__ == '__main__':
    unittest.main(verbosity=2)
