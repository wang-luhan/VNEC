# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wangluhan/code/IPDPS/looynu/VNEC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wangluhan/code/IPDPS/looynu/VNEC/build

# Utility rule file for merge_SpB_core_and_SpB_operation.

# Include any custom commands dependencies for this target.
include CMakeFiles/merge_SpB_core_and_SpB_operation.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/merge_SpB_core_and_SpB_operation.dir/progress.make

CMakeFiles/merge_SpB_core_and_SpB_operation: libopensparseblas.a

libopensparseblas.a: lib/libOpenSpB_core.so.1.0.0
libopensparseblas.a: lib/libOpenSpB_core.so.1.0.0
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/wangluhan/code/IPDPS/looynu/VNEC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating libopensparseblas.a"
	rm -f ./lib/libopensparseblas.a
	ar crsT ./lib/libopensparseblas.a /home/wangluhan/code/IPDPS/looynu/VNEC/build/lib/libOpenSpB_core.so.1.0.0 /home/wangluhan/code/IPDPS/looynu/VNEC/build/lib/libOpenSpB_operation.so.1.0.0

merge_SpB_core_and_SpB_operation: CMakeFiles/merge_SpB_core_and_SpB_operation
merge_SpB_core_and_SpB_operation: libopensparseblas.a
merge_SpB_core_and_SpB_operation: CMakeFiles/merge_SpB_core_and_SpB_operation.dir/build.make
.PHONY : merge_SpB_core_and_SpB_operation

# Rule to build all files generated by this target.
CMakeFiles/merge_SpB_core_and_SpB_operation.dir/build: merge_SpB_core_and_SpB_operation
.PHONY : CMakeFiles/merge_SpB_core_and_SpB_operation.dir/build

CMakeFiles/merge_SpB_core_and_SpB_operation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/merge_SpB_core_and_SpB_operation.dir/cmake_clean.cmake
.PHONY : CMakeFiles/merge_SpB_core_and_SpB_operation.dir/clean

CMakeFiles/merge_SpB_core_and_SpB_operation.dir/depend:
	cd /home/wangluhan/code/IPDPS/looynu/VNEC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wangluhan/code/IPDPS/looynu/VNEC /home/wangluhan/code/IPDPS/looynu/VNEC /home/wangluhan/code/IPDPS/looynu/VNEC/build /home/wangluhan/code/IPDPS/looynu/VNEC/build /home/wangluhan/code/IPDPS/looynu/VNEC/build/CMakeFiles/merge_SpB_core_and_SpB_operation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/merge_SpB_core_and_SpB_operation.dir/depend

