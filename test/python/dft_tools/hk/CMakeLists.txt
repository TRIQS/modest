# HK Converter Tests

# Copy input files
file(COPY hk_convert_hamiltonian.hk DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add test
add_test(NAME Py_hk_convert
         COMMAND ${TRIQS_PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/hk_convert.py
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set_property(TEST Py_hk_convert APPEND PROPERTY ENVIRONMENT
             PYTHONPATH=${PROJECT_BINARY_DIR}/python:$ENV{PYTHONPATH} ${SANITIZER_RT_PRELOAD})
