# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 2.8

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files (x86)\CMake 2.8\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files (x86)\CMake 2.8\bin\cmake.exe" -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = "C:\Program Files (x86)\CMake 2.8\bin\cmake-gui.exe"

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Poruchik-bI\workspace\VStab\sources

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Poruchik-bI\workspace\VStab\build

# Include any dependencies generated for this target.
include stabilization/src/CMakeFiles/VideoStabilization.dir/depend.make

# Include the progress variables for this target.
include stabilization/src/CMakeFiles/VideoStabilization.dir/progress.make

# Include the compile flags for this target's objects.
include stabilization/src/CMakeFiles/VideoStabilization.dir/flags.make

stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj: stabilization/src/CMakeFiles/VideoStabilization.dir/flags.make
stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj: stabilization/src/CMakeFiles/VideoStabilization.dir/includes_CXX.rsp
stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj: C:/Users/Poruchik-bI/workspace/VStab/sources/stabilization/src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report C:\Users\Poruchik-bI\workspace\VStab\build\CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj"
	cd /d C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src && C:\MinGW\bin\g++.exe   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles\VideoStabilization.dir\main.cpp.obj -c C:\Users\Poruchik-bI\workspace\VStab\sources\stabilization\src\main.cpp

stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VideoStabilization.dir/main.cpp.i"
	cd /d C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src && C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -E C:\Users\Poruchik-bI\workspace\VStab\sources\stabilization\src\main.cpp > CMakeFiles\VideoStabilization.dir\main.cpp.i

stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VideoStabilization.dir/main.cpp.s"
	cd /d C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src && C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_FLAGS) -S C:\Users\Poruchik-bI\workspace\VStab\sources\stabilization\src\main.cpp -o CMakeFiles\VideoStabilization.dir\main.cpp.s

stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.requires:
.PHONY : stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.requires

stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.provides: stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.requires
	$(MAKE) -f stabilization\src\CMakeFiles\VideoStabilization.dir\build.make stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.provides.build
.PHONY : stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.provides

stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.provides.build: stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj

# Object files for target VideoStabilization
VideoStabilization_OBJECTS = \
"CMakeFiles/VideoStabilization.dir/main.cpp.obj"

# External object files for target VideoStabilization
VideoStabilization_EXTERNAL_OBJECTS =

stabilization/src/VideoStabilization.exe: stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj
stabilization/src/VideoStabilization.exe: stabilization/libstabilization.a
stabilization/src/VideoStabilization.exe: C:/Program\ Files/boost_1_49_0/stage/lib/libboost_system-mgw47-mt-d-1_49.dll
stabilization/src/VideoStabilization.exe: stabilization/src/CMakeFiles/VideoStabilization.dir/build.make
stabilization/src/VideoStabilization.exe: stabilization/src/CMakeFiles/VideoStabilization.dir/objects1.rsp
stabilization/src/VideoStabilization.exe: stabilization/src/CMakeFiles/VideoStabilization.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable VideoStabilization.exe"
	cd /d C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\VideoStabilization.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
stabilization/src/CMakeFiles/VideoStabilization.dir/build: stabilization/src/VideoStabilization.exe
.PHONY : stabilization/src/CMakeFiles/VideoStabilization.dir/build

stabilization/src/CMakeFiles/VideoStabilization.dir/requires: stabilization/src/CMakeFiles/VideoStabilization.dir/main.cpp.obj.requires
.PHONY : stabilization/src/CMakeFiles/VideoStabilization.dir/requires

stabilization/src/CMakeFiles/VideoStabilization.dir/clean:
	cd /d C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src && $(CMAKE_COMMAND) -P CMakeFiles\VideoStabilization.dir\cmake_clean.cmake
.PHONY : stabilization/src/CMakeFiles/VideoStabilization.dir/clean

stabilization/src/CMakeFiles/VideoStabilization.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Poruchik-bI\workspace\VStab\sources C:\Users\Poruchik-bI\workspace\VStab\sources\stabilization\src C:\Users\Poruchik-bI\workspace\VStab\build C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src C:\Users\Poruchik-bI\workspace\VStab\build\stabilization\src\CMakeFiles\VideoStabilization.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : stabilization/src/CMakeFiles/VideoStabilization.dir/depend

