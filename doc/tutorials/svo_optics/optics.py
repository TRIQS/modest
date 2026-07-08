import numpy as np
from h5 import HDFArchive
import triqs.utility.mpi as mpi
from triqs.gfs import BlockGf, MeshReFreq
import triqs_modest as tm

mu, Sigma_w = None, None
if mpi.is_master_node():
    with HDFArchive('dmft_results.h5') as ar:
        mu = ar['mu']
        Sigma_w = ar['Sigma_w_m_dc']

mu, Sigma_w = mpi.bcast(mu), mpi.bcast(Sigma_w)

mesh = Sigma_w.mesh
Om_mesh = np.linspace(0, 0.5, 100)

_, obe = tm.one_body_elements_from_dft_converter("svo_dft_optics.h5", read_velocities=True)
E = tm.make_embedding(obe.C_space)

td = tm.transport_distribution(obe, 
                               mu, 
                               E.embed([Sigma_w]), 
                               Om_mesh, 
                               ["xx"], 
                               broadening=0.0
                               )
res = tm.optical_conductivity(td, beta=40.0)

if mpi.is_master_node():
    with HDFArchive("svo_dmft_optics.h5", "w") as ar:
        ar['Omega'] = Om_mesh
        ar['sigma'] = res.sigma['xx']
