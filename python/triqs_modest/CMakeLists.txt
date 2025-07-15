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

option(Update_Python_Bindings "Optionally update the C++ Python bindings in the source directory using clair-c2py" OFF)
if(Update_Python_Bindings)
  find_program(clair-c2py clair-c2py)
  if(NOT clair-c2py)
    message(FATAL_ERROR "clair-c2py executable not found. Please make sure it is installed and available in your PATH.")
  endif()
endif()

# Build and install any python modules
file(GLOB_RECURSE module_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(module_src ${module_sources})
  get_filename_component(module_name ${module_src} NAME_WE)
  get_filename_component(module_dir ${module_src} DIRECTORY)
  Python_add_library(${module_name} MODULE ${module_src})
  set_target_properties(${module_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${module_dir})
  add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})
  target_link_libraries(${module_name} PRIVATE ${PROJECT_NAME}_c triqs_py c2py::c2py)
  if(Update_Python_Bindings)
    string(REPLACE ".cpp" ".wrap.cxx" wrap_src ${CMAKE_CURRENT_SOURCE_DIR}/${module_src})
    add_custom_command(
      OUTPUT ${wrap_src}
      COMMAND ${clair-c2py} -p ${PROJECT_BINARY_DIR} ${module_src}
      DEPENDS ${module_src}
      DEPENDS ${clair-c2py}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(wrap_gen_${module_name} DEPENDS ${wrap_src})
    add_dependencies(${module_name} wrap_gen_${module_name})
  endif()
  install(TARGETS ${module_name} DESTINATION ${PYTHON_LIB_DEST}/${module_dir})
endforeach()
