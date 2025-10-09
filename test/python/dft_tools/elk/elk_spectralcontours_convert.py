import os
import numpy as np
from h5 import HDFArchive
from triqs.utility import h5diff
import triqs.utility.mpi as mpi

from triqs_modest.dft_tools.elk import Converter

# Get current working directory path
cwd = format(os.getcwd())
# Location of test directory
testdir = cwd+'/elk_spectralcontours_convert'
# Change to test directory
os.chdir(testdir)

# Test 1: Default k-mesh
converter = Converter(filename='SrVO3', repacking=True)
converter.hdf_file = 'elk_spectralcontours_convert.out.h5'
converter.convert_dft_input()
converter.convert_contours_input()

# Test 2: User-specified k-mesh - has to be same as used in elk.in
converter_user = Converter(filename='SrVO3', repacking=True)
converter_user.hdf_file = 'elk_spectralcontours_convert.out.h5'
ngrid = np.array([10,10,1], np.int_)
kgrid = np.array([[0.0,0.0,0.0],[1.0,0.0,0.0],[0.0,1.0,0.0],[0.0,0.0,1.0]], np.float64)
converter_user.convert_contours_input(kgrid=kgrid, ngrid=ngrid)

# Compare only converter output (not SumkDFT-computed results)
if mpi.is_master_node():
    with HDFArchive('elk_spectralcontours_convert.out.h5', 'r') as out, \
         HDFArchive('elk_spectralcontours_convert.ref.h5', 'r') as ref:

        converter_groups = ['dft_input', 'dft_contours_input', 'dft_misc_input', 'dft_symmcorr_input']

        for group in converter_groups:
            if group in out and group in ref:
                h5diff.compare(group, out[group], ref[group], 0, 1e-6)
            elif group in out or group in ref:
                h5diff.failures.append(f'Group {group} missing in one file')

        if h5diff.failures:
            print('-'*50)
            print('-'*20 + '  FAILED  ' + '-'*20)
            print('-'*50)
            for failure in h5diff.failures:
                print(failure)
            raise RuntimeError('FAILED')

# Return to cwd
os.chdir(cwd)
