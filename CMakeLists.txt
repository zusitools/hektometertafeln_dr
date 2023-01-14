cmake_minimum_required(VERSION 3.1)
project(hektometertafeln)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffile-prefix-map=${CMAKE_SOURCE_DIR}=CMAKE_SOURCE_DIR")

find_program(IWYU_PATH NAMES include-what-you-use)
if (IWYU_PATH)
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
endif()

find_package(Boost)

function(generate_template in nr out)
  add_custom_command(
      OUTPUT ${out}
      COMMAND echo "#include <string>" > ${out}
      COMMAND echo -n "namespace templates { std::string template${nr} = R\"(" >> ${out}
      COMMAND cat < ${in} >> ${out}
      COMMAND echo ")\"; }" >> ${out}
      DEPENDS ${in}
      VERBATIM
  )
endfunction(generate_template)

generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets/hekto-dr-1.ls3 1 ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-1.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets/hekto-dr-2.ls3 2 ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-2.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets/hekto-dr-3.ls3 3 ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-3.cpp)

generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets-lgv/hekto-lgv-1.ls3 1 ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-1.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets-lgv/hekto-lgv-2.ls3 2 ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-2.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets-lgv/hekto-lgv-3.ls3 3 ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-3.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets-lgv/hekto-lgv-mast-1.ls3 _mast1 ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-mast-1.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets-lgv/hekto-lgv-mast-2.ls3 _mast2 ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-mast-2.cpp)
generate_template(${CMAKE_CURRENT_SOURCE_DIR}/assets-lgv/hekto-lgv-mast-3.ls3 _mast3 ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-mast-3.cpp)

add_library(hektometertafeln_DR SHARED
  hekto_builder.cpp
  dll.cpp
  version.rc
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-1.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-2.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-3.cpp)
add_library(Borne_kilometrique_LGV SHARED
  hekto_builder.cpp
  dll.cpp
  version.rc
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-1.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-2.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-3.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-mast-1.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-mast-2.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-lgv-mast-3.cpp)

set_source_files_properties(version.rc PROPERTIES LANGUAGE RC)
target_compile_definitions(Borne_kilometrique_LGV PRIVATE -DLGV)

foreach (_name hektometertafeln_DR Borne_kilometrique_LGV)
  set_target_properties(${_name} PROPERTIES PREFIX "" LINKER_LANGUAGE C)
  if (WIN32 AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
    set_target_properties(${_name} PROPERTIES SUFFIX ".64.dll")
  endif()
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${_name} PRIVATE Boost::boost)

  if (WIN32)
    target_link_libraries(${_name} PUBLIC shlwapi)
  endif()

  target_compile_definitions(${_name} PRIVATE -DDLL_EXPORTS)
  if(MINGW)
    target_link_libraries(${_name} PRIVATE -Wl,--add-stdcall-alias)
  endif()
  install(TARGETS ${_name} DESTINATION _InstSetup/lib/milepost)

  add_executable(${_name}_test test.cpp)
  target_link_libraries(${_name}_test PRIVATE ${_name})
endforeach()

target_compile_definitions(Borne_kilometrique_LGV_test PRIVATE -DLGV)
