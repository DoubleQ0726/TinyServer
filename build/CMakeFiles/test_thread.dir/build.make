# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Users/QQ/Desktop/TinyServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Users/QQ/Desktop/TinyServer/build

# Include any dependencies generated for this target.
include CMakeFiles/test_thread.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_thread.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_thread.dir/flags.make

CMakeFiles/test_thread.dir/tests/test_thread.cpp.o: CMakeFiles/test_thread.dir/flags.make
CMakeFiles/test_thread.dir/tests/test_thread.cpp.o: ../tests/test_thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_thread.dir/tests/test_thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"tests/test_thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_thread.dir/tests/test_thread.cpp.o -c /mnt/c/Users/QQ/Desktop/TinyServer/tests/test_thread.cpp

CMakeFiles/test_thread.dir/tests/test_thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_thread.dir/tests/test_thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/QQ/Desktop/TinyServer/tests/test_thread.cpp > CMakeFiles/test_thread.dir/tests/test_thread.cpp.i

CMakeFiles/test_thread.dir/tests/test_thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_thread.dir/tests/test_thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/test_thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/QQ/Desktop/TinyServer/tests/test_thread.cpp -o CMakeFiles/test_thread.dir/tests/test_thread.cpp.s

# Object files for target test_thread
test_thread_OBJECTS = \
"CMakeFiles/test_thread.dir/tests/test_thread.cpp.o"

# External object files for target test_thread
test_thread_EXTERNAL_OBJECTS =

../bin/test_thread: CMakeFiles/test_thread.dir/tests/test_thread.cpp.o
../bin/test_thread: CMakeFiles/test_thread.dir/build.make
../bin/test_thread: ../lib/libTinyServer.so
../bin/test_thread: CMakeFiles/test_thread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/test_thread"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_thread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_thread.dir/build: ../bin/test_thread

.PHONY : CMakeFiles/test_thread.dir/build

CMakeFiles/test_thread.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_thread.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_thread.dir/clean

CMakeFiles/test_thread.dir/depend:
	cd /mnt/c/Users/QQ/Desktop/TinyServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/QQ/Desktop/TinyServer /mnt/c/Users/QQ/Desktop/TinyServer /mnt/c/Users/QQ/Desktop/TinyServer/build /mnt/c/Users/QQ/Desktop/TinyServer/build /mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles/test_thread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_thread.dir/depend

