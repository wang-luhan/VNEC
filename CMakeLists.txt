cmake_minimum_required(VERSION 3.5)

project(OpenSpB CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CPP_STANDARD 11)

# ----------------------------------------------------------------------------
# inclue required cmake files:
# ----------------------------------------------------------------------------
include(cmake/OpenSpBUtil.cmake)
include(cmake/OpenSpBModule.cmake)

string(TIMESTAMP BUILDDATE "%Y.%m.%d")
set(OpenSpB_VERSION_MAJOR 1)       # Add a feature or a new library. For example, increase the value of this parameter when adding the OMP.
set(OpenSpB_VERSION_MINOR 0)       # Increases when an interface is added.
set(OpenSpB_VERSION_PATCH 0)       # Increases when bugs are fixed
set(OpenSpB_VERSION_BUILDDATE \"${BUILDDATE}\")  # Last build time
set(OpenSpB_LIBVERSION "${OpenSpB_VERSION_MAJOR}.${OpenSpB_VERSION_MINOR}.${OpenSpB_VERSION_PATCH}")

# 打印编译选项
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------------------------------------------------------
# config the output path of the library:
# ----------------------------------------------------------------------------

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for application")
set(LIBRARY_OUTPUT_PATH    "${CMAKE_BINARY_DIR}/lib")

message("-- The output path of lib file is        ${LIBRARY_OUTPUT_PATH}")
message("-- The output path of executable file is ${EXECUTABLE_OUTPUT_PATH}")

# ----------------------------------------------------------------------------
# OpenSpB cmake options
# ----------------------------------------------------------------------------
OpenSpB_OPTION(BUILD_SHARED_LIBS      "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)"  ON)
OpenSpB_OPTION(BUILD_TESTS            "Build accuracy & regression tests"                                   ON)
OpenSpB_OPTION(BUILD_PERF_TESTS       "Build performance tests"                                             ON)
OpenSpB_OPTION(BUILD_WITH_OMP         "Build with OpenMP"                                                   ON)
OpenSpB_OPTION(BUILD_ON_ARM           "Build on arm platform"                                               ON)
OpenSpB_OPTION(BUILD_ON_X86           "Build on x86 platform"                                               OFF)
OpenSpB_OPTION(BUILD_WITH_DEBUG_INFO  "Build with debug information"                                        ON)
OpenSpB_OPTION(BUILD_WITH_MEMORY_CHCKE "Build with -fsanitize=address"                                      OFF)

# GNU Scientific Library 支持
find_package(GSL REQUIRED)

if(BUILD_WITH_MEMORY_CHCKE)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
endif()

if(BUILD_WITH_OMP)
    find_package(OpenMP)
	set (OpenMP_C_FLAGS "${OpenMP_C_FLAGS} -DWITH_OMP")
else()
    set (OpenMP_C_FLAGS "")
endif()

if(BUILD_ON_ARM)
    set(OpenSpB_USE_NEON "-DARM_NEON")
else()
    set(OpenSpB_USE_NEON "")
endif()

if(BUILD_ON_X86)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DX86_SIMD -mavx -march=skylake-avx512 -mtune=skylake-avx512")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenSpB_WITH_OMP} ${OpenSpB_USE_NEON}")

find_package(GTest)
# find_package(jemalloc REQUIRED)
find_package(PkgConfig REQUIRED)
# pkg_check_modules (JEMALLOC jemalloc)
# pkg_search_module(JEMALLOC REQUIRED jemalloc)

set(MAIN_PATH ${PROJECT_SOURCE_DIR})
configure_file(utilities/env.h.in ${PROJECT_SOURCE_DIR}/utilities/env.h)

#---------------------------------------------------------------------
# config build original makefile project
#--------------------------------------------------------------------------

#---------------------------------------------------------------------
# config linker libs
#--------------------------------------------------------------------------
SET(INTEL_ITT_PATH "/opt/intel/oneapi/vtune/latest/")
find_library(ITT_LIB ittnotify ${INTEL_ITT_PATH}sdk/lib64 NO_DEFAULT_PATH)

if( ITT_LIB AND (CMAKE_C_COMPILER_ID STREQUAL "Intel") AND (CMAKE_CXX_COMPILER_ID STREQUAL "Intel"))
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_INTEL_ITT -diag-disable=10441 ")
    SET(OpenSpB_TEST_INCLUDE_DIRECTORIES ${INTEL_ITT_PATH}/sdk/include)
    SET(OpenSpB_LINKER_TEST_LIBS ${ITT_LIB})
    include_directories(${INTEL_ITT_PATH}/sdk/include)
    message("-- Building with Intel Instrumentation and Tracing Technology")
else()
    set(ITT_LIB "")
    message("-- Building without Intel Instrumentation and Tracing Technology")
endif()
#---------------------------------------------------------------------
# config build option
#--------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -Wall -O0")
	message("-- Building type: Debug")
else(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -Wall")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND BUILD_ON_ARM)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.5" OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.8.5")
	        message("-- Insufficient gcc version:${CMAKE_CXX_COMPILER_VERSION}")
	        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8-a+crc+crypto+fp+simd -mtune=cortex-a53 -std=c11")
        else()
	        message("-- Insufficient gcc version:${CMAKE_CXX_COMPILER_VERSION}")
	        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv8.2-a+crc+crypto+fp16+simd -mtune=cortex-a72")
        endif()
    endif()
	message("-- Building type: Release")

    if(BUILD_WITH_DEBUG_INFO)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -rdynamic")
        message("-- Building with debug information")
    else()
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
    endif()
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
message("-- C_FLAGS:   ${CMAKE_C_FLAGS}")
message("-- CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# ----------------------------------------------------------------------------
# add_modules
# ----------------------------------------------------------------------------
if(BUILD_SHARED_LIBS)
    SET(OpenSpB_MODULE_TYPE SHARED)
else()
    SET(OpenSpB_MODULE_TYPE STATIC)
endif()

ADD_SUBDIRECTORY(src)

#show OpenSpB modules information
string(REPLACE "OpenSpB_" "" OpenSpB_MODULES_BUILD_ST          "${OpenSpB_MODULES_BUILD}")
string(REPLACE "OpenSpB_" "" OpenSpB_MODULES_DISABLED_USER_ST  "${OpenSpB_MODULES_DISABLED_USER}")
string(REPLACE "OpenSpB_" "" OpenSpB_MODULES_DISABLED_FORCE_ST "${pORTOCL_MODULES_DISABLED_FORCE}")

set(OpenSpB_MODULES_DISABLED_AUTO_ST "")
foreach(m ${OpenSpB_MODULES_DISABLED_AUTO})
    set(__mdeps "")
    foreach(d ${OpenSpB_MODULE_${m}_DEPS})
        if(d MATCHES "^OpenSpB_" AND NOT HAVE_${d})
            list(APPEND __mdeps ${d})
        endif()
    endforeach()
    if(__mdeps)
        list(APPEND OpenSpB_MODULES_DISABLED_AUTO_ST "${m}(deps: ${__mdeps})")
    else()
        list(APPEND OpenSpB_MODULES_DISABLED_AUTO_ST "${m}")
    endif()
endforeach()

string(REPLACE "OpenSpB_" "" OpenSpB_MODULES_DISABLED_AUTO_ST  "${OpenSpB_MODULES_DISABLED_AUTO_ST}")

message("===================The modules build information as follwos:")
message("    Public modules:         ${OpenSpB_MODULES_PUBLIC}"        )
message("    To be built:            ${OpenSpB_MODULES_BUILD}"         )


add_custom_command(OUTPUT libopensparseblas.a
COMMAND rm -f ./lib/libopensparseblas.a
COMMAND ar crsT ./lib/libopensparseblas.a $<TARGET_FILE:OpenSpB_core> $<TARGET_FILE:OpenSpB_operation>
DEPENDS OpenSpB_core OpenSpB_core)
add_custom_target(merge_SpB_core_and_SpB_operation ALL DEPENDS libopensparseblas.a)

add_custom_command(OUTPUT libopensparseblas.so
COMMAND rm -f ./lib/libopensparseblas.so
COMMAND ld -shared -o ./lib/libopensparseblas.so $<TARGET_FILE:OpenSpB_core> $<TARGET_FILE:OpenSpB_operation>
DEPENDS OpenSpB_core OpenSpB_core)


add_custom_target(merge_SpB_core_and_SpB_operation_so ALL DEPENDS libopensparseblas.so)

add_custom_target (
    run
    @echo "[make] testing..." &
    ./bin/OpenSpB_test_core &
    ./bin/OpenSpB_test_matrix &
    ./bin/OpenSpB_test_algorithm
    )


