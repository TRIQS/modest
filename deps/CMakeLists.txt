include(get_cpm.cmake)

# -- c2py --
if(PythonSupport OR (NOT IS_SUBPROJECT AND Build_Documentation))
  CPMAddPackage("gh:flatironinstitute/c2py#unstable")
endif()

# -- GTest --
CPMAddPackage("gh:google/googletest#main")

# -- TLRanges --
CPMAddPackage(
  URI "gh:TartanLlama/ranges#main"
  OPTIONS "RANGES_BUILD_TESTS OFF"
)
# Install the tl/ ranges headers consumed by our public headers (defs.hpp, ibz_symmetry_ops.hpp)
install(DIRECTORY ${ranges_SOURCE_DIR}/include/ DESTINATION include)
