# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/d0a2fe3d-1c3e-4b60-877e-72e014856c80/jetson_multimedia_api/samples/vd_test/decode_tx2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/d0a2fe3d-1c3e-4b60-877e-72e014856c80/jetson_multimedia_api/samples/vd_test/decode_tx2

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /media/d0a2fe3d-1c3e-4b60-877e-72e014856c80/jetson_multimedia_api/samples/vd_test/decode_tx2/CMakeFiles /media/d0a2fe3d-1c3e-4b60-877e-72e014856c80/jetson_multimedia_api/samples/vd_test/decode_tx2/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /media/d0a2fe3d-1c3e-4b60-877e-72e014856c80/jetson_multimedia_api/samples/vd_test/decode_tx2/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named decode_tx2

# Build rule for target.
decode_tx2: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 decode_tx2
.PHONY : decode_tx2

# fast build rule for target.
decode_tx2/fast:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/build
.PHONY : decode_tx2/fast

NvApplicationProfiler.o: NvApplicationProfiler.cpp.o

.PHONY : NvApplicationProfiler.o

# target to build an object file
NvApplicationProfiler.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvApplicationProfiler.cpp.o
.PHONY : NvApplicationProfiler.cpp.o

NvApplicationProfiler.i: NvApplicationProfiler.cpp.i

.PHONY : NvApplicationProfiler.i

# target to preprocess a source file
NvApplicationProfiler.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvApplicationProfiler.cpp.i
.PHONY : NvApplicationProfiler.cpp.i

NvApplicationProfiler.s: NvApplicationProfiler.cpp.s

.PHONY : NvApplicationProfiler.s

# target to generate assembly for a file
NvApplicationProfiler.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvApplicationProfiler.cpp.s
.PHONY : NvApplicationProfiler.cpp.s

NvBuffer.o: NvBuffer.cpp.o

.PHONY : NvBuffer.o

# target to build an object file
NvBuffer.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvBuffer.cpp.o
.PHONY : NvBuffer.cpp.o

NvBuffer.i: NvBuffer.cpp.i

.PHONY : NvBuffer.i

# target to preprocess a source file
NvBuffer.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvBuffer.cpp.i
.PHONY : NvBuffer.cpp.i

NvBuffer.s: NvBuffer.cpp.s

.PHONY : NvBuffer.s

# target to generate assembly for a file
NvBuffer.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvBuffer.cpp.s
.PHONY : NvBuffer.cpp.s

NvEglRenderer.o: NvEglRenderer.cpp.o

.PHONY : NvEglRenderer.o

# target to build an object file
NvEglRenderer.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvEglRenderer.cpp.o
.PHONY : NvEglRenderer.cpp.o

NvEglRenderer.i: NvEglRenderer.cpp.i

.PHONY : NvEglRenderer.i

# target to preprocess a source file
NvEglRenderer.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvEglRenderer.cpp.i
.PHONY : NvEglRenderer.cpp.i

NvEglRenderer.s: NvEglRenderer.cpp.s

.PHONY : NvEglRenderer.s

# target to generate assembly for a file
NvEglRenderer.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvEglRenderer.cpp.s
.PHONY : NvEglRenderer.cpp.s

NvElement.o: NvElement.cpp.o

.PHONY : NvElement.o

# target to build an object file
NvElement.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvElement.cpp.o
.PHONY : NvElement.cpp.o

NvElement.i: NvElement.cpp.i

.PHONY : NvElement.i

# target to preprocess a source file
NvElement.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvElement.cpp.i
.PHONY : NvElement.cpp.i

NvElement.s: NvElement.cpp.s

.PHONY : NvElement.s

# target to generate assembly for a file
NvElement.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvElement.cpp.s
.PHONY : NvElement.cpp.s

NvElementProfiler.o: NvElementProfiler.cpp.o

.PHONY : NvElementProfiler.o

# target to build an object file
NvElementProfiler.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvElementProfiler.cpp.o
.PHONY : NvElementProfiler.cpp.o

NvElementProfiler.i: NvElementProfiler.cpp.i

.PHONY : NvElementProfiler.i

# target to preprocess a source file
NvElementProfiler.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvElementProfiler.cpp.i
.PHONY : NvElementProfiler.cpp.i

NvElementProfiler.s: NvElementProfiler.cpp.s

.PHONY : NvElementProfiler.s

# target to generate assembly for a file
NvElementProfiler.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvElementProfiler.cpp.s
.PHONY : NvElementProfiler.cpp.s

NvLogging.o: NvLogging.cpp.o

.PHONY : NvLogging.o

# target to build an object file
NvLogging.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvLogging.cpp.o
.PHONY : NvLogging.cpp.o

NvLogging.i: NvLogging.cpp.i

.PHONY : NvLogging.i

# target to preprocess a source file
NvLogging.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvLogging.cpp.i
.PHONY : NvLogging.cpp.i

NvLogging.s: NvLogging.cpp.s

.PHONY : NvLogging.s

# target to generate assembly for a file
NvLogging.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvLogging.cpp.s
.PHONY : NvLogging.cpp.s

NvUtils.o: NvUtils.cpp.o

.PHONY : NvUtils.o

# target to build an object file
NvUtils.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvUtils.cpp.o
.PHONY : NvUtils.cpp.o

NvUtils.i: NvUtils.cpp.i

.PHONY : NvUtils.i

# target to preprocess a source file
NvUtils.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvUtils.cpp.i
.PHONY : NvUtils.cpp.i

NvUtils.s: NvUtils.cpp.s

.PHONY : NvUtils.s

# target to generate assembly for a file
NvUtils.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvUtils.cpp.s
.PHONY : NvUtils.cpp.s

NvV4l2Element.o: NvV4l2Element.cpp.o

.PHONY : NvV4l2Element.o

# target to build an object file
NvV4l2Element.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvV4l2Element.cpp.o
.PHONY : NvV4l2Element.cpp.o

NvV4l2Element.i: NvV4l2Element.cpp.i

.PHONY : NvV4l2Element.i

# target to preprocess a source file
NvV4l2Element.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvV4l2Element.cpp.i
.PHONY : NvV4l2Element.cpp.i

NvV4l2Element.s: NvV4l2Element.cpp.s

.PHONY : NvV4l2Element.s

# target to generate assembly for a file
NvV4l2Element.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvV4l2Element.cpp.s
.PHONY : NvV4l2Element.cpp.s

NvV4l2ElementPlane.o: NvV4l2ElementPlane.cpp.o

.PHONY : NvV4l2ElementPlane.o

# target to build an object file
NvV4l2ElementPlane.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvV4l2ElementPlane.cpp.o
.PHONY : NvV4l2ElementPlane.cpp.o

NvV4l2ElementPlane.i: NvV4l2ElementPlane.cpp.i

.PHONY : NvV4l2ElementPlane.i

# target to preprocess a source file
NvV4l2ElementPlane.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvV4l2ElementPlane.cpp.i
.PHONY : NvV4l2ElementPlane.cpp.i

NvV4l2ElementPlane.s: NvV4l2ElementPlane.cpp.s

.PHONY : NvV4l2ElementPlane.s

# target to generate assembly for a file
NvV4l2ElementPlane.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvV4l2ElementPlane.cpp.s
.PHONY : NvV4l2ElementPlane.cpp.s

NvVideoConverter.o: NvVideoConverter.cpp.o

.PHONY : NvVideoConverter.o

# target to build an object file
NvVideoConverter.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoConverter.cpp.o
.PHONY : NvVideoConverter.cpp.o

NvVideoConverter.i: NvVideoConverter.cpp.i

.PHONY : NvVideoConverter.i

# target to preprocess a source file
NvVideoConverter.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoConverter.cpp.i
.PHONY : NvVideoConverter.cpp.i

NvVideoConverter.s: NvVideoConverter.cpp.s

.PHONY : NvVideoConverter.s

# target to generate assembly for a file
NvVideoConverter.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoConverter.cpp.s
.PHONY : NvVideoConverter.cpp.s

NvVideoDecoder.o: NvVideoDecoder.cpp.o

.PHONY : NvVideoDecoder.o

# target to build an object file
NvVideoDecoder.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoDecoder.cpp.o
.PHONY : NvVideoDecoder.cpp.o

NvVideoDecoder.i: NvVideoDecoder.cpp.i

.PHONY : NvVideoDecoder.i

# target to preprocess a source file
NvVideoDecoder.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoDecoder.cpp.i
.PHONY : NvVideoDecoder.cpp.i

NvVideoDecoder.s: NvVideoDecoder.cpp.s

.PHONY : NvVideoDecoder.s

# target to generate assembly for a file
NvVideoDecoder.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoDecoder.cpp.s
.PHONY : NvVideoDecoder.cpp.s

NvVideoEncoder.o: NvVideoEncoder.cpp.o

.PHONY : NvVideoEncoder.o

# target to build an object file
NvVideoEncoder.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoEncoder.cpp.o
.PHONY : NvVideoEncoder.cpp.o

NvVideoEncoder.i: NvVideoEncoder.cpp.i

.PHONY : NvVideoEncoder.i

# target to preprocess a source file
NvVideoEncoder.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoEncoder.cpp.i
.PHONY : NvVideoEncoder.cpp.i

NvVideoEncoder.s: NvVideoEncoder.cpp.s

.PHONY : NvVideoEncoder.s

# target to generate assembly for a file
NvVideoEncoder.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/NvVideoEncoder.cpp.s
.PHONY : NvVideoEncoder.cpp.s

decoder_h264.o: decoder_h264.cpp.o

.PHONY : decoder_h264.o

# target to build an object file
decoder_h264.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/decoder_h264.cpp.o
.PHONY : decoder_h264.cpp.o

decoder_h264.i: decoder_h264.cpp.i

.PHONY : decoder_h264.i

# target to preprocess a source file
decoder_h264.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/decoder_h264.cpp.i
.PHONY : decoder_h264.cpp.i

decoder_h264.s: decoder_h264.cpp.s

.PHONY : decoder_h264.s

# target to generate assembly for a file
decoder_h264.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/decoder_h264.cpp.s
.PHONY : decoder_h264.cpp.s

main.o: main.cpp.o

.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i

.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s

.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) -f CMakeFiles/decode_tx2.dir/build.make CMakeFiles/decode_tx2.dir/main.cpp.s
.PHONY : main.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... decode_tx2"
	@echo "... edit_cache"
	@echo "... NvApplicationProfiler.o"
	@echo "... NvApplicationProfiler.i"
	@echo "... NvApplicationProfiler.s"
	@echo "... NvBuffer.o"
	@echo "... NvBuffer.i"
	@echo "... NvBuffer.s"
	@echo "... NvEglRenderer.o"
	@echo "... NvEglRenderer.i"
	@echo "... NvEglRenderer.s"
	@echo "... NvElement.o"
	@echo "... NvElement.i"
	@echo "... NvElement.s"
	@echo "... NvElementProfiler.o"
	@echo "... NvElementProfiler.i"
	@echo "... NvElementProfiler.s"
	@echo "... NvLogging.o"
	@echo "... NvLogging.i"
	@echo "... NvLogging.s"
	@echo "... NvUtils.o"
	@echo "... NvUtils.i"
	@echo "... NvUtils.s"
	@echo "... NvV4l2Element.o"
	@echo "... NvV4l2Element.i"
	@echo "... NvV4l2Element.s"
	@echo "... NvV4l2ElementPlane.o"
	@echo "... NvV4l2ElementPlane.i"
	@echo "... NvV4l2ElementPlane.s"
	@echo "... NvVideoConverter.o"
	@echo "... NvVideoConverter.i"
	@echo "... NvVideoConverter.s"
	@echo "... NvVideoDecoder.o"
	@echo "... NvVideoDecoder.i"
	@echo "... NvVideoDecoder.s"
	@echo "... NvVideoEncoder.o"
	@echo "... NvVideoEncoder.i"
	@echo "... NvVideoEncoder.s"
	@echo "... decoder_h264.o"
	@echo "... decoder_h264.i"
	@echo "... decoder_h264.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

