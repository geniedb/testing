# Locate and configure the Google Protocol Buffers library.
#
# The following variables can be set and are optional:
#
#   PROTOBUF_SRC_ROOT_FOLDER - When compiling with MSVC, if this cache variable is set
#                              the protobuf-default VS project build locations
#                              (vsprojects/Debug & vsprojects/Release) will be searched
#                              for libraries and binaries.
#
#   PROTOBUF_IMPORT_DIRS     - List of additional directories to be searched for
#                              imported .proto files. (New in CMake 2.8.8)
#
# Defines the following variables:
#
#   PROTOBUF_FOUND - Found the Google Protocol Buffers library (libprotobuf & header files)
#   PROTOBUF_INCLUDE_DIRS - Include directories for Google Protocol Buffers
#
#   PROTOBUF_LIBRARIES - The protobuf libraries (names)
#   PROTOBUF_LIBRARY_PATHS - The protobuf library paths
#   PROTOBUF_PROTOC_LIBRARIES - The protoc libraries (names)
#   PROTOBUF_PROTOC_LIBRARY_PATHS - The protoc library paths
#   PROTOBUF_LITE_LIBRARIES - The protobuf-lite libraries (names)
#   PROTOBUF_LITE_LIBRARY_PATHS - The protobuf-lite library paths
#
# The following cache variables are also available to set or use:
#   PROTOBUF_LIBRARY - The protobuf library name
#   PROTOBUF_LIBRARY_PATH - The protobuf library path
#   PROTOBUF_PROTOC_LIBRARY   - The protoc library name
#   PROTOBUF_PROTOC_LIBRARY_PATH   - The protoc library path
#   PROTOBUF_LITE_LIBRARY   - The protobuf lite library name
#   PROTOBUF_LITE_LIBRARY_PATH   - The protobuf lite library path
#   PROTOBUF_LIBRARY_DEBUG - The protobuf library name (debug)
#   PROTOBUF_LIBRARY_PATH_DEBUG - The protobuf library path (debug)
#   PROTOBUF_PROTOC_LIBRARY_DEBUG   - The protoc library name (debug)
#   PROTOBUF_PROTOC_LIBRARY_PATH_DEBUG   - The protoc library path (debug)
#   PROTOBUF_LITE_LIBRARY_DEBUG - The protobuf lite library name (debug)
#   PROTOBUF_LITE_LIBRARY_PATH_DEBUG - The protobuf lite library path (debug)
#
#   PROTOBUF_INCLUDE_DIR - The include directory for protocol buffers
#   PROTOBUF_PROTOC_EXECUTABLE - The protoc compiler
#
#  ====================================================================
#  Example:
#
#   find_package(Protobuf REQUIRED)
#   include_directories(${PROTOBUF_INCLUDE_DIRS})
#
#   include_directories(${CMAKE_CURRENT_BINARY_DIR})
#   PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS foo.proto)
#   add_executable(bar bar.cc ${PROTO_SRCS} ${PROTO_HDRS})
#   target_link_libraries(bar ${PROTOBUF_LIBRARIES})
#
# NOTE: You may need to link against pthreads, depending
#       on the platform.
#
# NOTE: The PROTOBUF_GENERATE_CPP macro & add_executable() or add_library()
#       calls only work properly within the same directory.
#
#  ====================================================================
#
# PROTOBUF_GENERATE_CPP (public function)
#   SRCS = Variable to define with autogenerated
#          source files
#   HDRS = Variable to define with autogenerated
#          header files
#   ARGN = proto files
#
#  ====================================================================


#=============================================================================
# Copyright 2009 Kitware, Inc.
# Copyright 2009-2011 Philip Lowman <philip@yhbt.com>
# Copyright 2008 Esben Mose Hansen, Ange Optimization ApS
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

function(PROTOBUF_GENERATE_CPP SRCS HDRS)
  if(NOT ARGN)
    message(SEND_ERROR "Error: PROTOBUF_GENERATE_CPP() called without any proto files")
    return()
  endif()

  if(PROTOBUF_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF_IMPORT_DIRS)
    foreach(DIR ${PROTOBUF_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  endif()

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
    list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc"
             "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h"
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out  ${CMAKE_CURRENT_BINARY_DIR} ${_protobuf_include_path} ${ABS_FIL}
      DEPENDS ${ABS_FIL}
      COMMENT "Running C++ protocol buffer compiler on ${FIL}"
      VERBATIM )
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

# Internal function: search for normal library as well as a debug one
#    if the debug one is specified also include debug/optimized keywords
#    in *_LIBRARIES variable
function(_protobuf_find_libraries name filename)
   find_library(${name}_LIBRARY_FULLPATH
       NAMES ${filename}
       PATHS ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/Release ${PROTOBUF_SRC_ROOT_FOLDER})
   #message ("*********** ${name}_LIBRARY_FULLPATH=${${name}_LIBRARY_FULLPATH}")
   mark_as_advanced(${name}_LIBRARY)
   if(${name}_LIBRARY_FULLPATH)
      set( ${name}_LIBRARY ${${name}_LIBRARY_FULLPATH} PARENT_SCOPE)
      get_filename_component(${name}_LIBRARY ${${name}_LIBRARY_FULLPATH} NAME_WE)
      get_filename_component(${name}_LIBRARY_PATH ${${name}_LIBRARY_FULLPATH} PATH)
   endif()

   find_library(${name}_LIBRARY_FULLPATH_DEBUG
	   NAMES ${filename}d ${filename}
	   PATHS ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/Debug)
   if(NOT ${name}_LIBRARY_FULLPATH_DEBUG)
	   find_library(${name}_LIBRARY_FULLPATH_DEBUG
		   NAMES ${filename}d
		   PATHS ${PROTOBUF_SRC_ROOT_FOLDER})
   endif()
   if(${name}_LIBRARY_FULLPATH_DEBUG)
      get_filename_component(${name}_LIBRARY_DEBUG ${${name}_LIBRARY_FULLPATH_DEBUG} NAME_WE)
      get_filename_component(${name}_LIBRARY_PATH_DEBUG ${${name}_LIBRARY_FULLPATH_DEBUG} PATH)
   endif()
   mark_as_advanced(${name}_LIBRARY_DEBUG)

   if(NOT ${name}_LIBRARY_DEBUG)
      # There is no debug library
      set(${name}_LIBRARY_DEBUG ${${name}_LIBRARY} PARENT_SCOPE)
      set(${name}_LIBRARIES     ${${name}_LIBRARY} PARENT_SCOPE)
      set(${name}_LIBRARY_PATHS ${${name}_LIBRARY_PATH} PARENT_SCOPE)
   else()
      # There IS a debug library
      set(${name}_LIBRARIES
          optimized ${${name}_LIBRARY}
          debug     ${${name}_LIBRARY_DEBUG}
          PARENT_SCOPE
      )
      set(${name}_LIBRARY_PATHS
          optimized ${${name}_LIBRARY_PATH}
          debug     ${${name}_LIBRARY_PATH_DEBUG}
          PARENT_SCOPE
      )
      set(${name}_LIBRARY_PATHS_ALL
          ${${name}_LIBRARY_PATH}
          ${${name}_LIBRARY_PATH_DEBUG}
          PARENT_SCOPE
      )
   endif()
endfunction()

#
# Main.
#

# By default have PROTOBUF_GENERATE_CPP macro pass -I to protoc
# for each directory where a proto file is referenced.
if(NOT DEFINED PROTOBUF_GENERATE_CPP_APPEND_PATH)
  set(PROTOBUF_GENERATE_CPP_APPEND_PATH TRUE)
endif()


# Google's provided vcproj files generate libraries with a "lib"
# prefix on Windows
if(MSVC)
    set(PROTOBUF_ORIG_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES}")
    set(CMAKE_FIND_LIBRARY_PREFIXES "lib" "")

    find_path(PROTOBUF_SRC_ROOT_FOLDER protobuf.pc.in)
endif()

# The Protobuf library
_protobuf_find_libraries(PROTOBUF protobuf)
#DOC "The Google Protocol Buffers RELEASE Library"
#message ("*************** PROTOBUF=${PROTOBUF}")

_protobuf_find_libraries(PROTOBUF_LITE protobuf-lite)

# The Protobuf Protoc Library
_protobuf_find_libraries(PROTOBUF_PROTOC protoc)

# Restore original find library prefixes
if(MSVC)
    set(CMAKE_FIND_LIBRARY_PREFIXES "${PROTOBUF_ORIG_FIND_LIBRARY_PREFIXES}")
endif()


# Find the include directory
find_path(PROTOBUF_INCLUDE_DIR
    google/protobuf/service.h
    PATHS ${PROTOBUF_SRC_ROOT_FOLDER}/src
)
mark_as_advanced(PROTOBUF_INCLUDE_DIR)

# Find the protoc Executable
find_program(PROTOBUF_PROTOC_EXECUTABLE
    NAMES protoc
    DOC "The Google Protocol Buffers Compiler"
    PATHS
    ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/Release
    ${PROTOBUF_SRC_ROOT_FOLDER}/vsprojects/Debug
)
mark_as_advanced(PROTOBUF_PROTOC_EXECUTABLE)

IF(POLICY CMP0017)
	CMAKE_POLICY(SET CMP0017 NEW)
ENDIF()
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PROTOBUF DEFAULT_MSG
    PROTOBUF_LIBRARY PROTOBUF_INCLUDE_DIR)

if(PROTOBUF_FOUND)
    set(PROTOBUF_INCLUDE_DIRS ${PROTOBUF_INCLUDE_DIR})
endif()
