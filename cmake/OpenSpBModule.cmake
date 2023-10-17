# clean modules info which needs to be recalculated
set(OpenSpB_MODULES_PUBLIC         "" CACHE INTERNAL "List of OpenSpB modules marked for export")
set(OpenSpB_MODULES_BUILD          "" CACHE INTERNAL "List of OpenSpB modules included into the build")
set(OpenSpB_MODULES_DISABLED_USER  "" CACHE INTERNAL "List of OpenSpB modules explicitly disabled by user")
set(OpenSpB_MODULES_DISABLED_AUTO  "" CACHE INTERNAL "List of OpenSpB modules implicitly disabled due to dependencies")
set(OpenSpB_MODULES_DISABLED_FORCE "" CACHE INTERNAL "List of OpenSpB modules which can not be build in current configuration")

# adds dependencies to OpenSpB module
# Usage:
#   add_dependencies(OpenSpB_<name> [REQUIRED] [<list of dependencies>] [OPTIONAL <list of modules>])
# Notes:
# * <list of dependencies> - can include full names of modules or full pathes to shared/static libraries or cmake targets

macro(OpenSpB_add_dependencies full_modname)
    #we don't clean the dependencies here to allow this macro several times for every module
    foreach(d "REQUIRED" ${ARGN})
        if(d STREQUAL "REQUIRED")
            set(__depsvar OpenSpB_MODULE_${full_modname}_REQ_DEPS)
        elseif(d STREQUAL "OPTIONAL")
            set(__depsvar OpenSpB_MODULE_${full_modname}_OPT_DEPS)
        else()
            list(APPEND ${__depsvar} "OpenSpB_${d}")
        endif()
    endforeach()
    unset(__depsvar)

    OpenSpB_list_unique(OpenSpB_MODULE_${full_modname}_REQ_DEPS)
    OpenSpB_list_unique(OpenSpB_MODULE_${full_modname}_OPT_DEPS)

    set(OpenSpB_MODULE_${full_modname}_REQ_DEPS ${OpenSpB_MODULE_${full_modname}_REQ_DEPS} CACHE INTERNAL "Required dependencies of ${full_modname} module")
    set(OpenSpB_MODULE_${full_modname}_OPT_DEPS ${OpenSpB_MODULE_${full_modname}_OPT_DEPS} CACHE INTERNAL "Optional dependencies of ${full_modname} module")

endmacro()

# declare new OpenSpB module in current folder
# Usage:
#   OpenSpB_add_module(<name> [INTERNAL|BINDINGS] [REQUIRED] [<list of dependencies>] [OPTIONAL <list of optional dependencies>])
# Example:
#   OpenSpB_add_module(yaom INTERNAL OpenSpB_core OpenSpB_highgui OpenSpB_flann OPTIONAL OpenSpB_gpu)
macro(OpenSpB_add_module _name)
    string(TOLOWER "${_name}" name)
    set(the_module     OpenSpB_${name})
    set(the_module_res ${name})

    # the first pass - collect modules info, the second pass - create targets
    if(OpenSpB_INITIAL_PASS)
        #guard agains redefinition
        if(";${OpenSpB_MODULES_BUILD};${OpenSpB_MODULES_DISABLED_USER};" MATCHES ";${the_module};")
            message(FATAL_ERROR "Redefinition of the ${the_module} module.
            at:                    ${CMAKE_CURRENT_SOURCE_DIR}
            previously defined at: ${OpenSpB_MODULE_${the_module}_LOCATION}
            ")
        endif()

        if(NOT DEFINED the_description)
            set(the_description "The ${name} OpenSpB module")
        endif()

        if(NOT DEFINED BUILD_${the_module}_INIT)
            set(BUILD_${the_module}_INIT ON)
        endif()


        # create option to enable/disable this module
        option(BUILD_${the_module} "Include ${the_module} module into the OpenSpB build" ${BUILD_${the_module}_INIT})

        # remember the module details
        set(OpenSpB_MODULE_${the_module}_DESCRIPTION "${the_description}"          CACHE INTERNAL "Brief description of ${the_module} module")
        set(OpenSpB_MODULE_${the_module}_LOCATION    "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${the_module} module sources")

        # parse list of dependencies
        if("${ARGV1}" STREQUAL "INTERNAL" OR "${ARGV1}" STREQUAL "BINDINGS")
            set(OpenSpB_MODULE_${the_module}_CLASS "${ARGV1}" CACHE INTERNAL "The cathegory of the module")
            set(__OpenSpB_argn__ ${ARGN})
            list(REMOVE_AT __OpenSpB_argn__ 0)
            OpenSpB_add_dependencies(${the_module} ${__OpenSpB_argn__})
            unset(__OpenSpB_argn__)
        else()
            set(OpenSpB_MODULE_${the_module}_CLASS "PUBLIC" CACHE INTERNAL "The cathegory of the module")
            OpenSpB_add_dependencies(${the_module} ${ARGN})
            if(BUILD_${the_module})
                set(OpenSpB_MODULES_PUBLIC ${OpenSpB_MODULES_PUBLIC} "${the_module}" CACHE INTERNAL "List of OpenSpB modules marked for export")
            endif()
        endif()

        if(BUILD_${the_module})
            set(OpenSpB_MODULES_BUILD ${OpenSpB_MODULES_BUILD} "${the_module}" CACHE INTERNAL "List of OpenSpB modules included into the build")
        else()
            set(OpenSpB_MODULES_DISABLED_USER ${OpenSpB_MODULES_DISABLED_USER} "${the_module}" CACHE INTERNAL "List of OpenSpB modules explicitly disabled by user")
        endif()

        return()
    else(OpenSpB_INITIAL_PASS) #
        if(NOT BUILD_${the_module})
            return() # extra protection from redefinition
        endif()
        project(${the_module})
    endif(OpenSpB_INITIAL_PASS)
endmacro()

# excludes module from current configuration
macro(OpenSpB_disable_module module)
    set(__modname ${module})
    if(NOT __modname MATCHES "^OpenSpB_")
        set(__modname OpenSpB_${module})
    endif()

    list(APPEND OpenSpB_MODULES_DISABLED_FORCE "${__modname}")
    set(HAVE_${__modname} OFF CACHE INTERNAL "Module ${__modname} can not be built in current configuration")
    set(OpenSpB_MODULE_${__modname}_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Location of ${__modname} module sources")
    set(OpenSpB_MODULES_DISABLED_FORCE "${OpenSpB_MODULES_DISABLED_FORCE}" CACHE INTERNAL "List of OpenSpB modules which can not be build in current configuration")
    if(BUILD_${__modname})
        # touch variable controlling build of the module to suppress "unused variable" CMake warning
    endif()
    unset(__modname)
    return() # leave the current folder
endmacro()

macro(__OpenSpB_module_turn_off the_module)
  list(REMOVE_ITEM OpenSpB_MODULES_DISABLED_AUTO "${the_module}")
  list(APPEND      OpenSpB_MODULES_DISABLED_AUTO "${the_module}")
  list(REMOVE_ITEM OpenSpB_MODULES_BUILD "${the_module}")
  list(REMOVE_ITEM OpenSpB_MODULES_PUBLIC "${the_module}")
  set(HAVE_${the_module} OFF CACHE INTERNAL "Module ${the_module} can not be built in current configuration")
endmacro()

macro(__OpenSpB_flatten_module_required_dependencies the_module)
    set(__flattened_deps "")
    set(__resolved_deps "")
    set(__req_depends ${OpenSpB_MODULE_${the_module}_REQ_DEPS})

    while(__req_depends)
        OpenSpB_list_pop_front(__req_depends __dep)

        #depends on self
        if(__dep STREQUAL the_module)
            __OpenSpB_module_turn_off(${the_module})
            break()

            #depends on disabled module
        elseif(";${OpenSpB_MODULES_DISABLED_USER};${OpenSpB_MODULES_DISABLED_AUTO};" MATCHES ";${__dep};")

            __OpenSpB_module_turn_off(${the_module}) # depends on disabled module
            #list(APPEND __flattened_deps "${__dep}")
            break() #TODO

            #depends on build-enabled module
        elseif(";${OpenSpB_MODULES_BUILD};" MATCHES ";${__dep};")

            if(";${__resolved_deps};" MATCHES ";${__dep};")
                list(APPEND __flattened_deps "${__dep}") # all dependencies of this module are already resolved
            else()
                # put all required subdependencies before this dependency and mark it as resolved
                list(APPEND __resolved_deps "${__dep}")
                list(INSERT __req_depends 0 ${OpenSpB_MODULE_${__dep}_REQ_DEPS} ${__dep})
            endif()

        elseif(__dep MATCHES "^OpenSpB_")
            __OpenSpB_module_turn_off(${the_module}) # depends on missing module
            message(WARNING "Unknown \"${__dep}\" module is listened in the dependencies of \"${the_module}\" module")
            break()
        else()
            # skip non-modules
        endif()
    endwhile()

    if(__flattened_deps)
        list(REMOVE_DUPLICATES __flattened_deps)
        set(OpenSpB_MODULE_${the_module}_DEPS ${__flattened_deps})
    else()
        set(OpenSpB_MODULE_${the_module}_DEPS "")
    endif()

    OpenSpB_clear_vars(__resolved_deps __flattened_deps __req_depends __dep)
endmacro()

macro(__OpenSpB_flatten_module_optional_dependencies the_module)
  set(__flattened_deps "")
  set(__resolved_deps "")
  set(__opt_depends ${OpenSpB_MODULE_${the_module}_REQ_DEPS} ${OpenSpB_MODULE_${the_module}_OPT_DEPS})

  while(__opt_depends)
    OpenSpB_list_pop_front(__opt_depends __dep)
    if(__dep STREQUAL the_module)
        __OpenSpB_module_turn_off(${the_module})
      break()

    elseif(";${OpenSpB_MODULES_BUILD};" MATCHES ";${__dep};")
      if(";${__resolved_deps};" MATCHES ";${__dep};")
        list(APPEND __flattened_deps "${__dep}") # all dependencies of this module are already resolved
      else()
        # put all subdependencies before this dependency and mark it as resolved
        list(APPEND __resolved_deps "${__dep}")
        list(INSERT __opt_depends 0 ${OpenSpB_MODULE_${__dep}_REQ_DEPS} ${OpenSpB_MODULE_${__dep}_OPT_DEPS} ${__dep})
      endif()
    else()
      # skip non-modules or missing modules
    endif()
  endwhile()

  if(__flattened_deps)
    list(REMOVE_DUPLICATES __flattened_deps)
    set(OpenSpB_MODULE_${the_module}_DEPS ${__flattened_deps})
  else()
    set(OpenSpB_MODULE_${the_module}_DEPS "")
  endif()

  OpenSpB_clear_vars(__resolved_deps __flattened_deps __opt_depends __dep)
endmacro()

macro(__OpenSpB_flatten_module_dependencies)

    foreach(m ${OpenSpB_MODULES_DISABLED_USER})
        set(HAVE_${m} OFF CACHE INTERNAL "Module ${m} will not be built in current configuration")
    endforeach()

    foreach(m ${OpenSpB_MODULES_BUILD})
        set(HAVE_${m} ON CACHE INTERNAL "Module ${m} will be built in current configuration")
        __OpenSpB_flatten_module_required_dependencies(${m})
        set(OpenSpB_MODULE_${m}_DEPS ${OpenSpB_MODULE_${m}_DEPS} CACHE INTERNAL "Flattened required dependencies of ${m} module")
    endforeach()

    foreach(m ${OpenSpB_MODULES_BUILD})
        __OpenSpB_flatten_module_optional_dependencies(${m})

        # save dependencies from other modules
        set(OpenSpB_MODULE_${m}_DEPS ${OpenSpB_MODULE_${m}_DEPS} CACHE INTERNAL "Flattened dependencies of ${m} module")
        # save extra dependencies
        set(OpenSpB_MODULE_${m}_DEPS_EXT ${OpenSpB_MODULE_${m}_REQ_DEPS} ${OpenSpB_MODULE_${m}_OPT_DEPS})
        if(OpenSpB_MODULE_${m}_DEPS_EXT AND OpenSpB_MODULE_${m}_DEPS)
            list(REMOVE_ITEM OpenSpB_MODULE_${m}_DEPS_EXT ${OpenSpB_MODULE_${m}_DEPS})
        endif()

        set(OpenSpB_MODULE_${m}_DEPS_EXT ${OpenSpB_MODULE_${m}_DEPS_EXT} CACHE INTERNAL "Extra dependencies of ${m} module")
    endforeach()

    # order modules by dependencies
    set(OpenSpB_MODULES_BUILD_ "")
    foreach(m ${OpenSpB_MODULES_BUILD})
        list(APPEND OpenSpB_MODULES_BUILD_ ${OpenSpB_MODULE_${m}_DEPS} ${m})
    endforeach()
    OpenSpB_list_unique(OpenSpB_MODULES_BUILD_)

    set(OpenSpB_MODULES_PUBLIC        ${OpenSpB_MODULES_PUBLIC}        CACHE INTERNAL "List of OpenSpB modules marked for export")
    set(OpenSpB_MODULES_BUILD         ${OpenSpB_MODULES_BUILD_}        CACHE INTERNAL "List of OpenSpB modules included into the build")
    set(OpenSpB_MODULES_DISABLED_AUTO ${OpenSpB_MODULES_DISABLED_AUTO} CACHE INTERNAL "List of OpenSpB modules implicitly disabled due to dependencies")
endmacro()
# collect modules from specified directories
# NB: must be called only once!
macro(OpenSpB_glob_modules)
    if(DEFINED OpenSpB_INITIAL_PASS)
        message(FATAL_ERROR "Perf_IPP has already loaded its modules. Calling OpenSpB_glob_modules second time is not allowed.")
    endif()

    # set(__directories_observed "")

    # collect modules
    set(OpenSpB_INITIAL_PASS ON)

    foreach(__path ${ARGN})
        OpenSpB_get_real_path(__path "${__path}")
        list(FIND __directories_observed "${__path}" __pathIdx)
        if(__pathIdx GREATER -1)
            message(FATAL_ERROR "The directory ${__path} is observed for OpenSpB modules second time.")
        endif()

        list(APPEND __directories_observed "${__path}")
        message("-- Source code path = ${__path}")

        file(GLOB __OpenSpBmodules RELATIVE "${__path}" "${__path}/*")

        if(__OpenSpBmodules)
            list(SORT __OpenSpBmodules)
            foreach(mod ${__OpenSpBmodules})

                OpenSpB_get_real_path(__modpath "${__path}/${mod}")
                if(EXISTS "${__modpath}/CMakeLists.txt")

                    list(FIND __directories_observed "${__modpath}" __pathIdx)
                    if(__pathIdx GREATER -1)
                        message(FATAL_ERROR "The module from ${__modpath} is already loaded.")
                    endif()
                    list(APPEND __directories_observed "${__modpath}")

                    add_subdirectory("${__modpath}" "${CMAKE_CURRENT_BINARY_DIR}/${mod}/.${mod}")

                endif()
            endforeach()
        endif()
    endforeach()

    OpenSpB_clear_vars(__OpenSpBmodules __directories_observed __path __modpath __pathIdx)

    # resolve dependencies
     __OpenSpB_flatten_module_dependencies()

    # create modules
    set(OpenSpB_INITIAL_PASS OFF PARENT_SCOPE)
    set(OpenSpB_INITIAL_PASS OFF)

    foreach(m ${OpenSpB_MODULES_BUILD})
        if(m MATCHES "^OpenSpB_")
            string(REGEX REPLACE "^OpenSpB_" "" __shortname "${m}")
            add_subdirectory("${OpenSpB_MODULE_${m}_LOCATION}" "${CMAKE_CURRENT_BINARY_DIR}/${__shortname}")
        endif()
    endforeach()
    unset(__shortname)
endmacro()

# setup include paths for the list of passed modules
macro(OpenSpB_include_modules)
    foreach(d ${ARGN})
        if(d MATCHES "^OpenSpB_" AND HAVE_${d})
            if (EXISTS "${OpenSpB_MODULE_${d}_LOCATION}/include")
                OpenSpB_include_directories("${OpenSpB_MODULE_${d}_LOCATION}/include")
            endif()
        elseif(EXISTS "${d}")
            OpenSpB_include_directories("${d}")
        endif()
    endforeach()
endmacro()

# setup include path for OpenSpB headers for specified module
# OpenSpB_module_include_directories(<extra include directories/extra include modules>)
macro(OpenSpB_module_include_directories)
    OpenSpB_include_directories("${OpenSpB_MODULE_${the_module}_LOCATION}/include"
        "${OpenSpB_MODULE_${the_module}_LOCATION}/src"
        "${CMAKE_CURRENT_BINARY_DIR}" # for precompiled headers
        )
    OpenSpB_include_modules(${OpenSpB_MODULE_${the_module}_DEPS} ${ARGN})
endmacro()

# sets header and source files for the current module
# NB: all files specified as headers will be installed
# Usage:
# OpenSpB_set_module_sources([HEADERS] <list of files> [SOURCES] <list of files>)
macro(OpenSpB_set_module_sources)
    set(OpenSpB_MODULE_${the_module}_HEADERS "")
    set(OpenSpB_MODULE_${the_module}_SOURCES "")

    foreach(f "HEADERS" ${ARGN})
        if(f STREQUAL "HEADERS" OR f STREQUAL "SOURCES")
            set(__filesvar "OpenSpB_MODULE_${the_module}_${f}")
        else()
            list(APPEND ${__filesvar} "${f}")
        endif()
    endforeach()

    # the hacky way to embeed any files into the OpenSpB without modification of its build system. This part will be rewritten
    #if(COMMAND OpenSpB_get_module_external_sources)
    #  OpenSpB_get_module_external_sources()
    #endif()

    # use full paths for module to be independent from the module location
    OpenSpB_convert_to_full_paths(OpenSpB_MODULE_${the_module}_HEADERS)

    set(OpenSpB_MODULE_${the_module}_HEADERS ${OpenSpB_MODULE_${the_module}_HEADERS} CACHE INTERNAL "List of header files for ${the_module}")
    set(OpenSpB_MODULE_${the_module}_SOURCES ${OpenSpB_MODULE_${the_module}_SOURCES} CACHE INTERNAL "List of source files for ${the_module}")
endmacro()

# creates OpenSpB module in current folder
# creates new target, configures standard dependencies, compilers flags, install rules
# Usage:
#   OpenSpB_create_module(<extra link dependencies>)
#   OpenSpB_create_module(SKIP_LINK)
macro(OpenSpB_create_module)
    ADD_LIBRARY(${the_module} ${OpenSpB_MODULE_TYPE} ${OpenSpB_MODULE_${the_module}_HEADERS} ${OpenSpB_MODULE_${the_module}_SOURCES})

    if(NOT "${ARGN}" STREQUAL "SKIP_LINK")
        TARGET_LINK_LIBRARIES(${the_module} ${OpenSpB_MODULE_${the_module}_DEPS} ${OpenSpB_MODULE_${the_module}_DEPS_EXT} ${OpenSpB_LINKER_LIBS} ${ARGN})
    endif()

    if(ENABLE_SOLUTION_FOLDERS)
        SET_TARGET_PROPERTIES(${the_module} PROPERTIES FOLDER "modules")
    endif()

    SET_TARGET_PROPERTIES(${the_module} PROPERTIES
        OUTPUT_NAME "${the_module}"
        DEBUG_POSTFIX "${OpenSpB_DEBUG_POSTFIX}"
        ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
        INSTALL_NAME_DIR lib
        )

    # For dynamic link numbering convenions
    if(NOT ANDROID)
        # Android SDK build scripts can include only .so files into final .apk
        # As result we should not set version properties for Android
        SET_TARGET_PROPERTIES(${the_module} PROPERTIES
            VERSION   ${OpenSpB_LIBVERSION}
            )
    endif()

    if(BUILD_SHARED_LIBS)
        if(MSVC)
            SET_TARGET_PROPERTIES(${the_module} PROPERTIES DEFINE_SYMBOL CVAPI_EXPORTS)
        else()
            ADD_DEFINITIONS(-DCVAPI_EXPORTS)
        endif()
    endif()

    if(MSVC)
        if(CMAKE_CROSSCOMPILING)
            SET_TARGET_PROPERTIES(${the_module} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:secchk")
        endif()
        SET_TARGET_PROPERTIES(${the_module} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libc /DEBUG")
    endif()

    #FILE(REMOVE_RECURSE ${OpenSpB_LIB_INSTALL_PATH})
    #FILE(REMOVE_RECURSE ${OpenSpB_INCLUDE_INSTALL_PATH})

#    INSTALL(TARGETS ${the_module}
#        RUNTIME DESTINATION ${OpenSpB_RUNTIME_INSTALL_PATH}
#        LIBRARY DESTINATION ${OpenSpB_LIB_INSTALL_PATH}
#        ARCHIVE DESTINATION ${OpenSpB_LIB_INSTALL_PATH}
#        COMPONENT main
#            )

    # only "public" headers need to be installed
    if(OpenSpB_MODULE_${the_module}_HEADERS AND ";${OpenSpB_MODULES_PUBLIC};" MATCHES ";${the_module};")
        foreach(hdr ${OpenSpB_MODULE_${the_module}_HEADERS})
            if(hdr MATCHES "([^/])+.h(..)?$")
                INSTALL(FILES ${hdr} DESTINATION "${OpenSpB_INCLUDE_INSTALL_PATH}/${the_module_res}" COMPONENT main)
            endif()
        endforeach()
    endif()
endmacro()

# ensures that all passed modules are available
# sets OpenSpB_DEPENDENCIES_FOUND variable to TRUE/FALSE
macro(OpenSpB_check_dependencies)
    set(OpenSpB_DEPENDENCIES_FOUND TRUE)
    foreach(d ${ARGN})
        if(d MATCHES "^OpenSpB_[^ ]+$" AND NOT HAVE_${d})
            set(OpenSpB_DEPENDENCIES_FOUND FALSE)
            break()
        endif()
    endforeach()
endmacro()

# auxiliary macro to parse arguments of OpenSpB_add_accuracy_tests and OpenSpB_add_perf_tests commands
macro(__OpenSpB_parse_test_sources tests_type)

    set(OpenSpB_${tests_type}_${the_module}_SOURCES "")
    set(OpenSpB_${tests_type}_${the_module}_DEPS "")
    set(__file_group_name    "")
    set(__file_group_sources "")

    foreach(arg "DEPENDS_ON" ${ARGN} "FILES")
        if(arg STREQUAL "FILES")
            set(__currentvar "__file_group_sources")
            if(__file_group_name AND __file_group_sources)
                SOURCE_GROUP("${__file_group_name}" FILES ${__file_group_sources})
                list(APPEND OpenSpB_${tests_type}_${the_module}_SOURCES ${__file_group_sources})
            endif()
            set(__file_group_name "")
            set(__file_group_sources "")
        elseif(arg STREQUAL "DEPENDS_ON")
            set(__currentvar "OpenSpB_TEST_${the_module}_DEPS")
        elseif("${__currentvar}" STREQUAL "__file_group_sources" AND NOT __file_group_name)
            set(__file_group_name "${arg}")
        else()
            list(APPEND ${__currentvar} "${arg}")
        endif()
    endforeach()
    unset(__file_group_name)
    unset(__file_group_sources)
    unset(__currentvar)
endmacro()

# this is a command for adding OpenSpB performance tests to the module
# OpenSpB_add_perf_tests(<extra_dependencies>)
function(OpenSpB_add_perf_tests)
    set(perf_path "${CMAKE_CURRENT_SOURCE_DIR}/perf")
    set(test_utilities_path "${CMAKE_SOURCE_DIR}/utilities")
    if(BUILD_PERF_TESTS AND EXISTS "${perf_path}")
        __OpenSpB_parse_test_sources(PERF ${ARGN})

        set(perf_deps ${the_module} ${OpenSpB_PERF_${the_module}_DEPS})
        OpenSpB_check_dependencies(${perf_deps})

        if(OpenSpB_DEPENDENCIES_FOUND)
            set(the_target "OpenSpB_perf_${name}")

            OpenSpB_include_directories(${OpenSpB_TEST_INCLUDE_DIRECTORIES})

            if(NOT OpenSpB_PERF_${the_module}_SOURCES)
                file(GLOB perf_srcs "${perf_path}/*.cpp" "${test_path}/*.c" "${test_utilities_path}/*.cpp")
                file(GLOB perf_hdrs "${perf_path}/*.hpp" "${perf_path}/*.h")
                SOURCE_GROUP("Src" FILES ${perf_srcs})
                SOURCE_GROUP("Include" FILES ${perf_hdrs})
                set(OpenSpB_PERF_${the_module}_SOURCES ${perf_srcs} ${perf_hdrs})
            endif()

            ADD_EXECUTABLE(${the_target} ${OpenSpB_PERF_${the_module}_SOURCES})
            TARGET_LINK_LIBRARIES(${the_target} ${OpenSpB_MODULE_${the_module}_DEPS} ${perf_deps} ${OpenSpB_LINKER_LIBS} ${OpenSpB_LINKER_TEST_LIBS} GTest::GTest GTest::Main)

            # Additional target properties
            SET_TARGET_PROPERTIES(${the_target} PROPERTIES
                DEBUG_POSTFIX "${OpenSpB_DEBUG_POSTFIX}"
                RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
                )

            if(ENABLE_SOLUTION_FOLDERS)
                SET_TARGET_PROPERTIES(${the_target} PROPERTIES FOLDER "tests_performance")
            endif()

            #OpenSpB_add_precompiled_headers(${the_target}) TODO

            #TODO
            #if (PYTHON_EXECUTABLE)
            #    add_dependencies(perf ${the_target})
            #endif()
        else(OpenSpB_DEPENDENCIES_FOUND)
            # TODO: warn about unsatisfied dependencies
        endif(OpenSpB_DEPENDENCIES_FOUND)
    endif()
endfunction()

# this is a command for adding OpenSpB accuracy/regression tests to the module
# OpenSpB_add_accuracy_tests([FILES <source group name> <list of sources>] [DEPENDS_ON] <list of extra dependencies>)
function(OpenSpB_add_accuracy_tests)
    set(test_path "${CMAKE_CURRENT_SOURCE_DIR}/test")
    set(test_utilities_path "${CMAKE_SOURCE_DIR}/utilities")
    if(BUILD_TESTS AND EXISTS "${test_path}")
        __OpenSpB_parse_test_sources(TEST ${ARGN})

        set(test_deps ${the_module} ${OpenSpB_TEST_${the_module}_DEPS})
        OpenSpB_check_dependencies(${test_deps})

        if(OpenSpB_DEPENDENCIES_FOUND)
            set(the_target "OpenSpB_test_${name}")

            OpenSpB_include_directories(${OpenSpB_TEST_INCLUDE_DIRECTORIES} ${test_utilities_path})

            if(NOT OpenSpB_TEST_${the_module}_SOURCES)
                file(GLOB test_srcs "${test_path}/*.cpp" "${test_path}/*.c" "${test_utilities_path}/*.cpp")
                file(GLOB test_hdrs "${test_path}/*.hpp" "${test_path}/*.h")
                source_group("Src" FILES ${test_srcs})
                source_group("Include" FILES ${test_hdrs})
                set(OpenSpB_TEST_${the_module}_SOURCES ${test_srcs} ${test_hdrs})
            endif()

            LINK_DIRECTORIES(${test_utilities_path})
            ADD_EXECUTABLE(${the_target} ${OpenSpB_TEST_${the_module}_SOURCES})
            TARGET_LINK_LIBRARIES(${the_target} ${test_deps} ${OpenSpB_LINKER_LIBS} ${OpenSpB_LINKER_TEST_LIBS} GTest::GTest GTest::Main)

            # Additional target properties
            SET_TARGET_PROPERTIES(${the_target} PROPERTIES
                DEBUG_POSTFIX "${OpenSpB_DEBUG_POSTFIX}"
                RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
                )

            if(ENABLE_SOLUTION_FOLDERS)
                SET_TARGET_PROPERTIES(${the_target} PROPERTIES FOLDER "tests_accuracy")
            endif()

        else(OpenSpB_DEPENDENCIES_FOUND)
            # TODO: warn about unsatisfied dependencies
        endif(OpenSpB_DEPENDENCIES_FOUND)
    endif()
endfunction()

# finds and sets headers and sources for the standard OpenSpB module
# Usage:
# OpenSpB_glob_module_sources(<extra sources&headers in the same format as used in OpenSpB_set_module_sources>)
macro(OpenSpB_glob_module_sources)
  file(GLOB_RECURSE lib_srcs "src/*.c" "src/*.cpp")
  file(GLOB_RECURSE lib_int_hdrs "src/*.hpp" "src/*.h")
  file(GLOB lib_hdrs "include/${name}/*.hpp" "include/${name}/*.h")

  source_group("Src" FILES ${lib_srcs} ${lib_int_hdrs})
  source_group("Include" FILES ${lib_hdrs})

  OpenSpB_set_module_sources(${ARGN} HEADERS ${lib_hdrs} SOURCES ${lib_srcs} ${lib_int_hdrs})
endmacro()
# short command for adding simple OpenSpB module
# see OpenSpB_add_module for argument details
# Usage:
# OpenSpB_define_module(module_name  [INTERNAL] [REQUIRED] [<list of dependencies>] [OPTIONAL <list of optional dependencies>])
macro(OpenSpB_define_module module_name)
  OpenSpB_add_module(${module_name} ${ARGN})
  OpenSpB_glob_module_sources()
  OpenSpB_module_include_directories()
  OpenSpB_create_module()

  if(BUILD_TESTS)
      OpenSpB_add_accuracy_tests()
  endif()
  if(BUILD_PERF_TESTS)
      OpenSpB_add_perf_tests()
  endif()
endmacro()
