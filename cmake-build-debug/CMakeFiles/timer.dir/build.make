# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/devtools/clion-2019.1.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /usr/local/devtools/clion-2019.1.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bizehao/CLionProjects/timer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bizehao/CLionProjects/timer/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/timer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/timer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/timer.dir/flags.make

CMakeFiles/timer.dir/test.cpp.o: CMakeFiles/timer.dir/flags.make
CMakeFiles/timer.dir/test.cpp.o: ../test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bizehao/CLionProjects/timer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/timer.dir/test.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/timer.dir/test.cpp.o -c /home/bizehao/CLionProjects/timer/test.cpp

CMakeFiles/timer.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/timer.dir/test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bizehao/CLionProjects/timer/test.cpp > CMakeFiles/timer.dir/test.cpp.i

CMakeFiles/timer.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/timer.dir/test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bizehao/CLionProjects/timer/test.cpp -o CMakeFiles/timer.dir/test.cpp.s

# Object files for target timer
timer_OBJECTS = \
"CMakeFiles/timer.dir/test.cpp.o"

# External object files for target timer
timer_EXTERNAL_OBJECTS =

timer: CMakeFiles/timer.dir/test.cpp.o
timer: CMakeFiles/timer.dir/build.make
timer: CMakeFiles/timer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bizehao/CLionProjects/timer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable timer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/timer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/timer.dir/build: timer

.PHONY : CMakeFiles/timer.dir/build

CMakeFiles/timer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/timer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/timer.dir/clean

CMakeFiles/timer.dir/depend:
	cd /home/bizehao/CLionProjects/timer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bizehao/CLionProjects/timer /home/bizehao/CLionProjects/timer /home/bizehao/CLionProjects/timer/cmake-build-debug /home/bizehao/CLionProjects/timer/cmake-build-debug /home/bizehao/CLionProjects/timer/cmake-build-debug/CMakeFiles/timer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/timer.dir/depend

