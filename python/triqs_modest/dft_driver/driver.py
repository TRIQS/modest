import triqs.utility.mpi as mpi

from triqs_modest import one_body_elements_from_dft_converter

class DftDriver(object):
    def __init__(self, DftSpecificDriver):
        required_methods = ['run_initial_stage', 'run_update_stage']
        for m in required_methods:
            if not hasattr(DftSpecificDriver, m) or not callable(getattr(DftSpecificDriver, m)):
                raise TypeError(f"{DftSpecificDriver.__name__} must define a callable method {m}")

        self.driver = DftSpecificDriver

    def one_body_elements_from_dft(self, threshold=1e-5, diagonalize_hloc=False, **kwargs) :
        
        # DFT code specific initial stage
        status = self.driver.run_initial_stage(**kwargs)
        
        return one_body_elements_from_dft_converter(f"{self.driver.seedname}.h5", 
                                threshold=threshold, 
                                diagonalize_hloc=diagonalize_hloc
                                )

    def update_one_body_elements_with_charge_correction(self, N_k, Eint_m_dc, threshold=1e-5, diagonalize_hloc=False, **kwargs):
        
        # DFT code specific update stage
        status = self.driver.run_update_stage(N_k, Eint_m_dc, **kwargs)

        return one_body_elements_from_dft_converter(f"{self.driver.seedname}.h5", 
                                  threshold=threshold, 
                                  diagonalize_hloc=diagonalize_hloc
                                  )
