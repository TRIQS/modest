# DFT Tools Converter Tests

# Copy all h5 reference files to binary dir
file(GLOB_RECURSE all_h5_ref_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h5)
foreach(file ${all_h5_ref_files})
  configure_file(${file} ${file} COPYONLY)
endforeach()

# Add subdirectories for each converter
add_subdirectory(wien2k)
add_subdirectory(hk)
add_subdirectory(elk)
add_subdirectory(vasp)

# Wannier90 tests require LFS files
if(Enable_LFS_Tests)
  add_subdirectory(wannier90)
endif()
