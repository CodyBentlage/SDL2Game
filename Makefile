# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/toasty/Documents/C++Projects/PassionProjects/SDL2Game

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/toasty/Documents/C++Projects/PassionProjects/SDL2Game

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/toasty/Documents/C++Projects/PassionProjects/SDL2Game/CMakeFiles /home/toasty/Documents/C++Projects/PassionProjects/SDL2Game//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/toasty/Documents/C++Projects/PassionProjects/SDL2Game/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named Boomin

# Build rule for target.
Boomin: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 Boomin
.PHONY : Boomin

# fast build rule for target.
Boomin/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/build
.PHONY : Boomin/fast

src/background.o: src/background.c.o
.PHONY : src/background.o

# target to build an object file
src/background.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/background.c.o
.PHONY : src/background.c.o

src/background.i: src/background.c.i
.PHONY : src/background.i

# target to preprocess a source file
src/background.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/background.c.i
.PHONY : src/background.c.i

src/background.s: src/background.c.s
.PHONY : src/background.s

# target to generate assembly for a file
src/background.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/background.c.s
.PHONY : src/background.c.s

src/draw.o: src/draw.c.o
.PHONY : src/draw.o

# target to build an object file
src/draw.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/draw.c.o
.PHONY : src/draw.c.o

src/draw.i: src/draw.c.i
.PHONY : src/draw.i

# target to preprocess a source file
src/draw.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/draw.c.i
.PHONY : src/draw.c.i

src/draw.s: src/draw.c.s
.PHONY : src/draw.s

# target to generate assembly for a file
src/draw.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/draw.c.s
.PHONY : src/draw.c.s

src/highscores.o: src/highscores.c.o
.PHONY : src/highscores.o

# target to build an object file
src/highscores.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/highscores.c.o
.PHONY : src/highscores.c.o

src/highscores.i: src/highscores.c.i
.PHONY : src/highscores.i

# target to preprocess a source file
src/highscores.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/highscores.c.i
.PHONY : src/highscores.c.i

src/highscores.s: src/highscores.c.s
.PHONY : src/highscores.s

# target to generate assembly for a file
src/highscores.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/highscores.c.s
.PHONY : src/highscores.c.s

src/init.o: src/init.c.o
.PHONY : src/init.o

# target to build an object file
src/init.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/init.c.o
.PHONY : src/init.c.o

src/init.i: src/init.c.i
.PHONY : src/init.i

# target to preprocess a source file
src/init.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/init.c.i
.PHONY : src/init.c.i

src/init.s: src/init.c.s
.PHONY : src/init.s

# target to generate assembly for a file
src/init.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/init.c.s
.PHONY : src/init.c.s

src/input.o: src/input.c.o
.PHONY : src/input.o

# target to build an object file
src/input.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/input.c.o
.PHONY : src/input.c.o

src/input.i: src/input.c.i
.PHONY : src/input.i

# target to preprocess a source file
src/input.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/input.c.i
.PHONY : src/input.c.i

src/input.s: src/input.c.s
.PHONY : src/input.s

# target to generate assembly for a file
src/input.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/input.c.s
.PHONY : src/input.c.s

src/main.o: src/main.c.o
.PHONY : src/main.o

# target to build an object file
src/main.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/main.c.o
.PHONY : src/main.c.o

src/main.i: src/main.c.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/main.c.i
.PHONY : src/main.c.i

src/main.s: src/main.c.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/main.c.s
.PHONY : src/main.c.s

src/sound.o: src/sound.c.o
.PHONY : src/sound.o

# target to build an object file
src/sound.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/sound.c.o
.PHONY : src/sound.c.o

src/sound.i: src/sound.c.i
.PHONY : src/sound.i

# target to preprocess a source file
src/sound.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/sound.c.i
.PHONY : src/sound.c.i

src/sound.s: src/sound.c.s
.PHONY : src/sound.s

# target to generate assembly for a file
src/sound.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/sound.c.s
.PHONY : src/sound.c.s

src/stage.o: src/stage.c.o
.PHONY : src/stage.o

# target to build an object file
src/stage.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/stage.c.o
.PHONY : src/stage.c.o

src/stage.i: src/stage.c.i
.PHONY : src/stage.i

# target to preprocess a source file
src/stage.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/stage.c.i
.PHONY : src/stage.c.i

src/stage.s: src/stage.c.s
.PHONY : src/stage.s

# target to generate assembly for a file
src/stage.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/stage.c.s
.PHONY : src/stage.c.s

src/text.o: src/text.c.o
.PHONY : src/text.o

# target to build an object file
src/text.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/text.c.o
.PHONY : src/text.c.o

src/text.i: src/text.c.i
.PHONY : src/text.i

# target to preprocess a source file
src/text.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/text.c.i
.PHONY : src/text.c.i

src/text.s: src/text.c.s
.PHONY : src/text.s

# target to generate assembly for a file
src/text.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/text.c.s
.PHONY : src/text.c.s

src/title.o: src/title.c.o
.PHONY : src/title.o

# target to build an object file
src/title.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/title.c.o
.PHONY : src/title.c.o

src/title.i: src/title.c.i
.PHONY : src/title.i

# target to preprocess a source file
src/title.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/title.c.i
.PHONY : src/title.c.i

src/title.s: src/title.c.s
.PHONY : src/title.s

# target to generate assembly for a file
src/title.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/title.c.s
.PHONY : src/title.c.s

src/util.o: src/util.c.o
.PHONY : src/util.o

# target to build an object file
src/util.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/util.c.o
.PHONY : src/util.c.o

src/util.i: src/util.c.i
.PHONY : src/util.i

# target to preprocess a source file
src/util.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/util.c.i
.PHONY : src/util.c.i

src/util.s: src/util.c.s
.PHONY : src/util.s

# target to generate assembly for a file
src/util.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Boomin.dir/build.make CMakeFiles/Boomin.dir/src/util.c.s
.PHONY : src/util.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... Boomin"
	@echo "... src/background.o"
	@echo "... src/background.i"
	@echo "... src/background.s"
	@echo "... src/draw.o"
	@echo "... src/draw.i"
	@echo "... src/draw.s"
	@echo "... src/highscores.o"
	@echo "... src/highscores.i"
	@echo "... src/highscores.s"
	@echo "... src/init.o"
	@echo "... src/init.i"
	@echo "... src/init.s"
	@echo "... src/input.o"
	@echo "... src/input.i"
	@echo "... src/input.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/sound.o"
	@echo "... src/sound.i"
	@echo "... src/sound.s"
	@echo "... src/stage.o"
	@echo "... src/stage.i"
	@echo "... src/stage.s"
	@echo "... src/text.o"
	@echo "... src/text.i"
	@echo "... src/text.s"
	@echo "... src/title.o"
	@echo "... src/title.i"
	@echo "... src/title.s"
	@echo "... src/util.o"
	@echo "... src/util.i"
	@echo "... src/util.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

