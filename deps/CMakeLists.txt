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

# -- dftkit --
CPMAddPackage(
  NAME dftkit
  GITHUB_REPOSITORY triqs/dftkit
  GIT_TAG unstable
  EXCLUDE_FROM_ALL NO
)
