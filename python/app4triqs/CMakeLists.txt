# Configure the version
configure_file(version.py.in version.py)

# All Python files. Copy them in the build dir to have a complete package for the tests.
file(GLOB_RECURSE python_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
foreach(file ${python_sources})
  configure_file(${file} ${file} COPYONLY)
endforeach()

# Install python files to proper location
set(PYTHON_LIB_DEST ${TRIQS_PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${TRIQS_PYTHON_LIB_DEST_ROOT} FILES_MATCHING PATTERN "*.py")

option(Update_Python_Bindings "Use Clair python bindings generators" OFF)
if(Update_Python_Bindings)
  find_package(clair REQUIRED)
endif()

# Build and install any python modules
file(GLOB_RECURSE modules RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(mod ${modules})
  string(REPLACE ".cpp" "" mod ${mod})
  get_filename_component(module_name ${mod} NAME)
  get_filename_component(module_dir ${mod} DIRECTORY)
  Python_add_library(${module_name} MODULE ${mod}.$<IF:$<BOOL:${Update_Python_Bindings}>,cpp,wrap.cxx>)
  set_target_properties(${module_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${module_dir})
  add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})
  target_link_libraries(${module_name} PRIVATE ${PROJECT_NAME}_c triqs_py c2py::c2py)
  if(Update_Python_Bindings)
    target_link_libraries(${module_name} PRIVATE clair::c2py_plugin)
    set_target_properties(${module_name} PROPERTIES CXX_COMPILER_LAUNCHER "") # Avoid ccache or similar
    add_custom_command(TARGET ${module_name} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${PROJECT_BINARY_DIR}/${module_name}.wrap.?xx
      ${CMAKE_CURRENT_SOURCE_DIR}/${module_dir}
    )
  endif()
  install(TARGETS ${module_name} DESTINATION ${PYTHON_LIB_DEST}/${module_dir})
endforeach()
