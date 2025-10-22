# Some tests require large file storage (LFS) input files
option(Enable_LFS_Tests "Enable tests that require Git LFS data files (68MB)" OFF)

# Validate Git LFS prerequisites if LFS tests are enabled
if(Enable_LFS_Tests)
  # Check if git-lfs is installed
  find_program(GIT_LFS_EXECUTABLE git-lfs)

  if(NOT GIT_LFS_EXECUTABLE)
    message(FATAL_ERROR
      "Enable_LFS_Tests=ON but Git LFS is not installed.\n"
      "Install Git LFS: https://git-lfs.github.com/\n"
      "Then run: git lfs install && git lfs pull\n"
      "Or disable with: -DEnable_LFS_Tests=OFF")
  endif()

  # Check if LFS files have been pulled (vs just pointer files)
  # We only need to check one representative file - if it's pulled, all are pulled
  set(TEST_LFS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/c++/ref_data_lfs/svo_t2g_tb.dat")
  file(SIZE "${TEST_LFS_FILE}" LFS_FILE_SIZE)

  if(LFS_FILE_SIZE LESS 1000)  # LFS pointer files are ~130 bytes, real files are much larger
    message(FATAL_ERROR
      "Enable_LFS_Tests=ON but LFS files have not been pulled (only pointer files present).\n"
      "Run: git lfs install && git lfs pull\n"
      "This will download ~68MB of test data.\n"
      "Or disable with: -DEnable_LFS_Tests=OFF")
  endif()

  message(STATUS "Git LFS files verified. LFS tests enabled.")
endif()

add_subdirectory(c++)

if(PythonSupport)
 add_subdirectory(python)
endif()
