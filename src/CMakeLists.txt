if(NOT OpenSpB_MODULES_PATH)
    set(OpenSpB_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
endif(NOT OpenSpB_MODULES_PATH)

message("-- OpenSpB_MODULES_PATH : ${OpenSpB_MODULES_PATH}")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR})

OpenSpB_glob_modules(${OpenSpB_MODULES_PATH})
