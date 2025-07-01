cmake_minimum_required(VERSION 3.15)

set(TARGET FrameTimingTest)
project(${TARGET} LANGUAGES C CXX)

add_executable(${TARGET})

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

include(GNUInstallDirs)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 2.0.10 REQUIRED)

target_sources(${TARGET} PRIVATE
  src/FrameTimingTest.cpp
)


add_subdirectory(deps/TimeUtils)


target_compile_definitions(${TARGET} PRIVATE
  _USE_MATH_DEFINES
)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${TARGET} PUBLIC -Wno-narrowing)
endif()

set_target_properties(${TARGET} PROPERTIES
  WIN32_EXECUTABLE FALSE
)

target_include_directories(${TARGET} PRIVATE
  ${SDL2_INCLUDE_DIR}

#  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


if (${BUILD_SHARED_LIBS})
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${TARGET} PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
  endif()

else()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${TARGET} PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
  else()
    target_link_libraries(${TARGET} PUBLIC
      -static
      -static-libgcc
      -static-libstdc++
    )
  endif()
endif()

target_link_libraries(${TARGET} PUBLIC
  SDL2::Main
  TimeUtils::TimeUtils
)

if (WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${TARGET} PROPERTIES
      WIN32_EXECUTABLE FALSE
    )
  else()
      target_link_libraries(${TARGET} PUBLIC
      -mconsole
    )
  endif()

  file(
    COPY scripts/1run.bat
    DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  )
else()
  file(
    COPY scripts/1run.sh
    DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
  )
endif()

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
