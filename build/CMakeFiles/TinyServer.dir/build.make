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
include CMakeFiles/TinyServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TinyServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TinyServer.dir/flags.make

CMakeFiles/TinyServer.dir/src/fiber.cpp.o: CMakeFiles/TinyServer.dir/flags.make
CMakeFiles/TinyServer.dir/src/fiber.cpp.o: ../src/fiber.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TinyServer.dir/src/fiber.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"src/fiber.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyServer.dir/src/fiber.cpp.o -c /mnt/c/Users/QQ/Desktop/TinyServer/src/fiber.cpp

CMakeFiles/TinyServer.dir/src/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyServer.dir/src/fiber.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/fiber.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/QQ/Desktop/TinyServer/src/fiber.cpp > CMakeFiles/TinyServer.dir/src/fiber.cpp.i

CMakeFiles/TinyServer.dir/src/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyServer.dir/src/fiber.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/fiber.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/QQ/Desktop/TinyServer/src/fiber.cpp -o CMakeFiles/TinyServer.dir/src/fiber.cpp.s

CMakeFiles/TinyServer.dir/src/log.cpp.o: CMakeFiles/TinyServer.dir/flags.make
CMakeFiles/TinyServer.dir/src/log.cpp.o: ../src/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/TinyServer.dir/src/log.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"src/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyServer.dir/src/log.cpp.o -c /mnt/c/Users/QQ/Desktop/TinyServer/src/log.cpp

CMakeFiles/TinyServer.dir/src/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyServer.dir/src/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/QQ/Desktop/TinyServer/src/log.cpp > CMakeFiles/TinyServer.dir/src/log.cpp.i

CMakeFiles/TinyServer.dir/src/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyServer.dir/src/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/log.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/QQ/Desktop/TinyServer/src/log.cpp -o CMakeFiles/TinyServer.dir/src/log.cpp.s

CMakeFiles/TinyServer.dir/src/util.cpp.o: CMakeFiles/TinyServer.dir/flags.make
CMakeFiles/TinyServer.dir/src/util.cpp.o: ../src/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/TinyServer.dir/src/util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"src/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyServer.dir/src/util.cpp.o -c /mnt/c/Users/QQ/Desktop/TinyServer/src/util.cpp

CMakeFiles/TinyServer.dir/src/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyServer.dir/src/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/QQ/Desktop/TinyServer/src/util.cpp > CMakeFiles/TinyServer.dir/src/util.cpp.i

CMakeFiles/TinyServer.dir/src/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyServer.dir/src/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/util.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/QQ/Desktop/TinyServer/src/util.cpp -o CMakeFiles/TinyServer.dir/src/util.cpp.s

CMakeFiles/TinyServer.dir/src/config.cpp.o: CMakeFiles/TinyServer.dir/flags.make
CMakeFiles/TinyServer.dir/src/config.cpp.o: ../src/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/TinyServer.dir/src/config.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"src/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyServer.dir/src/config.cpp.o -c /mnt/c/Users/QQ/Desktop/TinyServer/src/config.cpp

CMakeFiles/TinyServer.dir/src/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyServer.dir/src/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/QQ/Desktop/TinyServer/src/config.cpp > CMakeFiles/TinyServer.dir/src/config.cpp.i

CMakeFiles/TinyServer.dir/src/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyServer.dir/src/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/config.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/QQ/Desktop/TinyServer/src/config.cpp -o CMakeFiles/TinyServer.dir/src/config.cpp.s

CMakeFiles/TinyServer.dir/src/thread.cpp.o: CMakeFiles/TinyServer.dir/flags.make
CMakeFiles/TinyServer.dir/src/thread.cpp.o: ../src/thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/TinyServer.dir/src/thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"src/thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TinyServer.dir/src/thread.cpp.o -c /mnt/c/Users/QQ/Desktop/TinyServer/src/thread.cpp

CMakeFiles/TinyServer.dir/src/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TinyServer.dir/src/thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/c/Users/QQ/Desktop/TinyServer/src/thread.cpp > CMakeFiles/TinyServer.dir/src/thread.cpp.i

CMakeFiles/TinyServer.dir/src/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TinyServer.dir/src/thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"src/thread.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/c/Users/QQ/Desktop/TinyServer/src/thread.cpp -o CMakeFiles/TinyServer.dir/src/thread.cpp.s

# Object files for target TinyServer
TinyServer_OBJECTS = \
"CMakeFiles/TinyServer.dir/src/fiber.cpp.o" \
"CMakeFiles/TinyServer.dir/src/log.cpp.o" \
"CMakeFiles/TinyServer.dir/src/util.cpp.o" \
"CMakeFiles/TinyServer.dir/src/config.cpp.o" \
"CMakeFiles/TinyServer.dir/src/thread.cpp.o"

# External object files for target TinyServer
TinyServer_EXTERNAL_OBJECTS =

../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/src/fiber.cpp.o
../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/src/log.cpp.o
../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/src/util.cpp.o
../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/src/config.cpp.o
../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/src/thread.cpp.o
../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/build.make
../lib/libTinyServer.so: CMakeFiles/TinyServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library ../lib/libTinyServer.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TinyServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TinyServer.dir/build: ../lib/libTinyServer.so

.PHONY : CMakeFiles/TinyServer.dir/build

CMakeFiles/TinyServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TinyServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TinyServer.dir/clean

CMakeFiles/TinyServer.dir/depend:
	cd /mnt/c/Users/QQ/Desktop/TinyServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Users/QQ/Desktop/TinyServer /mnt/c/Users/QQ/Desktop/TinyServer /mnt/c/Users/QQ/Desktop/TinyServer/build /mnt/c/Users/QQ/Desktop/TinyServer/build /mnt/c/Users/QQ/Desktop/TinyServer/build/CMakeFiles/TinyServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TinyServer.dir/depend

