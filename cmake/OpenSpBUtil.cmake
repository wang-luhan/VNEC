# clears all passed variables
macro(OpenSpB_clear_vars)
  foreach(_var ${ARGN})
    unset(${_var} CACHE)
  endforeach()
endmacro()

# stable & safe duplicates removal macro
macro(OpenSpB_list_unique __lst)
  if(${__lst})
    list(REMOVE_DUPLICATES ${__lst})
  endif()
endmacro()

# safe list sorting macro
macro(ocv_list_sort __lst)
  if(${__lst})
    list(SORT ${__lst})
  endif()
endmacro()

# get absolute path with symlinks resolved
macro(OpenSpB_get_real_path VAR PATHSTR)
  if(CMAKE_VERSION VERSION_LESS 2.8)
    get_filename_component(${VAR} "${PATHSTR}" ABSOLUTE)
  else()
    get_filename_component(${VAR} "${PATHSTR}" REALPATH)
  endif()
endmacro()

# gets and removes the first element from list
macro(OpenSpB_list_pop_front LST VAR)
  if(${LST})
    list(GET ${LST} 0 ${VAR})
    list(REMOVE_AT ${LST} 0)
  else()
    set(${VAR} "")
  endif()
endmacro()

# remove all matching elements from the list
macro(OpenSpB_list_filterout lst regex)
  foreach(item ${${lst}})
    if(item MATCHES "${regex}")
      list(REMOVE_ITEM ${lst} "${item}")
  endif()
  endforeach()
endmacro()

# adds include directories in such way that directories from the OpenSpB source tree go first
function(OpenSpB_include_directories)
    set(__add_before "")
    foreach(dir ${ARGN})
        get_filename_component(__abs_dir "${dir}" ABSOLUTE)

        if("${__abs_dir}" MATCHES "^${OpenSpB_SOURCE_DIR}" OR "${__abs_dir}" MATCHES "^${OpenSpB_BINARY_DIR}")
            list(APPEND __add_before "${dir}")
        else()
            INCLUDE_DIRECTORIES(AFTER SYSTEM "${dir}")
        endif()
    endforeach()
    INCLUDE_DIRECTORIES(BEFORE ${__add_before})
endfunction()

# convert list of paths to full paths
macro(OpenSpB_convert_to_full_paths VAR)
    if(${VAR})
        set(__tmp "")
        foreach(path ${${VAR}})
            get_filename_component(${VAR} "${path}" ABSOLUTE)
            list(APPEND __tmp "${${VAR}}")
        endforeach()
        set(${VAR} ${__tmp})
        unset(__tmp)
    endif()
endmacro()


# Provides an option that the user can optionally select.
# Can accept condition to control when option is available for user.
# Usage:
#   option(<option_variable> "help string describing the option" <initial value or boolean expression> [IF <condition>])
macro(OpenSpB_OPTION variable description value)
    set(__value ${value})
    set(__condition "")
    set(__varname "__value")

    foreach(arg ${ARGN})
        if(arg STREQUAL "IF" OR arg STREQUAL "if")
            set(__varname "__condition")
        else()
            list(APPEND ${__varname} ${arg})
        endif()
    endforeach()
    unset(__varname)

    if("${__condition}" STREQUAL "")
        set(__condition 2 GREATER 1)
    endif()

    if(${__condition})
        if("${__value}" MATCHES ";")
            if(${__value})
                option(${variable} "${description}" ON)
            else()
                option(${variable} "${description}" OFF)
            endif()
        elseif(DEFINED ${__value})
            if(${__value})
                option(${variable} "${description}" ON)
            else()
                option(${variable} "${description}" ON)
                option(${variable} "${description}" OFF)
            endif()
        else()
            option(${variable} "${description}" ${__value})
        endif()
    else()
        unset(${variable} CACHE)
    endif()
    unset(__condition)
    unset(__value)
endmacro()

