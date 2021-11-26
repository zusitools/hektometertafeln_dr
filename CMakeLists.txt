cmake_minimum_required(VERSION 3.1)
project(hektometertafeln)

set(CMAKE_CXX_STANDARD 20)

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

set (SOURCES
  hekto_builder.cpp
  dll.cpp
  version.rc
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-1.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-2.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/hekto-dr-3.cpp
)

set_source_files_properties(version.rc PROPERTIES LANGUAGE RC)

add_library(hektometertafeln_DR SHARED ${SOURCES})
set_target_properties(hektometertafeln_DR PROPERTIES PREFIX "" LINKER_LANGUAGE C)
target_include_directories(hektometertafeln_DR PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hektometertafeln_DR PRIVATE Boost::boost)

if (WIN32)
  target_link_libraries(hektometertafeln_DR PUBLIC shlwapi)
endif()

target_compile_definitions(hektometertafeln_DR PRIVATE -DDLL_EXPORTS)
if(MINGW)
  target_link_libraries(hektometertafeln_DR PRIVATE -Wl,--add-stdcall-alias)
endif()
install(TARGETS hektometertafeln_DR DESTINATION _InstSetup/lib/milepost)

add_executable(hektometertafeln_DR_test test.cpp)
target_link_libraries(hektometertafeln_DR_test PRIVATE hektometertafeln_DR)
