# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.24

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\tools\CMake\bin\cmake.exe

# The command to remove a file.
RM = C:\tools\CMake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build

# Include any dependencies generated for this target.
include CMakeFiles/mniam_player.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/mniam_player.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/mniam_player.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mniam_player.dir/flags.make

CMakeFiles/mniam_player.dir/main.c.obj: CMakeFiles/mniam_player.dir/flags.make
CMakeFiles/mniam_player.dir/main.c.obj: C:/Users/Mariusz/Desktop/GIT_Repos/mniAM/Project_Files_For_Windows/mniam_player/main.c
CMakeFiles/mniam_player.dir/main.c.obj: CMakeFiles/mniam_player.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/mniam_player.dir/main.c.obj"
	C:\tools\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/mniam_player.dir/main.c.obj -MF CMakeFiles\mniam_player.dir\main.c.obj.d -o CMakeFiles\mniam_player.dir\main.c.obj -c C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\main.c

CMakeFiles/mniam_player.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mniam_player.dir/main.c.i"
	C:\tools\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\main.c > CMakeFiles\mniam_player.dir\main.c.i

CMakeFiles/mniam_player.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mniam_player.dir/main.c.s"
	C:\tools\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\main.c -o CMakeFiles\mniam_player.dir\main.c.s

CMakeFiles/mniam_player.dir/amcom.c.obj: CMakeFiles/mniam_player.dir/flags.make
CMakeFiles/mniam_player.dir/amcom.c.obj: C:/Users/Mariusz/Desktop/GIT_Repos/mniAM/Project_Files_For_Windows/mniam_player/amcom.c
CMakeFiles/mniam_player.dir/amcom.c.obj: CMakeFiles/mniam_player.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/mniam_player.dir/amcom.c.obj"
	C:\tools\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/mniam_player.dir/amcom.c.obj -MF CMakeFiles\mniam_player.dir\amcom.c.obj.d -o CMakeFiles\mniam_player.dir\amcom.c.obj -c C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\amcom.c

CMakeFiles/mniam_player.dir/amcom.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mniam_player.dir/amcom.c.i"
	C:\tools\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\amcom.c > CMakeFiles\mniam_player.dir\amcom.c.i

CMakeFiles/mniam_player.dir/amcom.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mniam_player.dir/amcom.c.s"
	C:\tools\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\amcom.c -o CMakeFiles\mniam_player.dir\amcom.c.s

# Object files for target mniam_player
mniam_player_OBJECTS = \
"CMakeFiles/mniam_player.dir/main.c.obj" \
"CMakeFiles/mniam_player.dir/amcom.c.obj"

# External object files for target mniam_player
mniam_player_EXTERNAL_OBJECTS =

mniam_player.exe: CMakeFiles/mniam_player.dir/main.c.obj
mniam_player.exe: CMakeFiles/mniam_player.dir/amcom.c.obj
mniam_player.exe: CMakeFiles/mniam_player.dir/build.make
mniam_player.exe: CMakeFiles/mniam_player.dir/linklibs.rsp
mniam_player.exe: CMakeFiles/mniam_player.dir/objects1.rsp
mniam_player.exe: CMakeFiles/mniam_player.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable mniam_player.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\mniam_player.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mniam_player.dir/build: mniam_player.exe
.PHONY : CMakeFiles/mniam_player.dir/build

CMakeFiles/mniam_player.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\mniam_player.dir\cmake_clean.cmake
.PHONY : CMakeFiles/mniam_player.dir/clean

CMakeFiles/mniam_player.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build C:\Users\Mariusz\Desktop\GIT_Repos\mniAM\Project_Files_For_Windows\mniam_player\build\CMakeFiles\mniam_player.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mniam_player.dir/depend

