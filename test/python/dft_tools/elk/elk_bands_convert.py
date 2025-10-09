import os
from h5 import HDFArchive
from triqs.utility import h5diff
import triqs.utility.mpi as mpi

from triqs_modest.dft_tools.elk import Converter

# Get current working directory path
cwd = format(os.getcwd())
# Location of test directory
testdir = cwd+'/elk_bands_convert'
# Change to test directory
os.chdir(testdir)

# Run converter
converter = Converter(filename='SrVO3', repacking=True)
converter.hdf_file = 'elk_bands_convert.out.h5'
converter.convert_dft_input()
converter.convert_bands_input()

# Compare only converter output (not SumkDFT-computed results)
if mpi.is_master_node():
    # Compare dft_input, dft_bands_input, dft_misc_input, dft_symmcorr_input
    with HDFArchive('elk_bands_convert.out.h5', 'r') as out, \
         HDFArchive('elk_bands_convert.ref.h5', 'r') as ref:

        converter_groups = ['dft_input', 'dft_bands_input', 'dft_misc_input', 'dft_symmcorr_input']

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
