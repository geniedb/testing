#######################################################
# CloudFabricV2 testing MAIN CMakeLists.txt

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 FATAL_ERROR )

PROJECT( Testing )

#SET( BLD_TARGET_SUFFIX d )
#SET( BLD_TARGET_TAG_SUFFIX _DEBUG )

SET( CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules" "${CMAKE_SOURCE_DIR}/CMakeModules" )
message ("++ CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

#----------------------------------
#Boost configuration
set(Boost_ADDITIONAL_VERSIONS "1.51" "1.51.0")
SET( BOOST_USE_STATIC_LIBS OFF)
SET( BOOST_USE_MULTITHREADED ON)
SET( BOOST_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE( Boost 1.36.0 REQUIRED COMPONENTS date_time system thread )
INCLUDE_DIRECTORIES	( ${Boost_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )
IF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	message ("++ Force BOOST to use DLL's")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DBOOST_ALL_DYN_LINK" )
ENDIF()


#----------------------------------
#Protobuf configuration
SET( PROTOBUF_SRC_ROOT_FOLDER $ENV{THIRD_PARTY_ROOT} )
FIND_PACKAGE( GenieProtobuf REQUIRED )
IF(NOT MSVC)
	SET( PROTOBUF_LIBRARIES protobuf )
ENDIF()

#----------------------------------
#Poco libs configuration
IF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	SET( POCO_INCLUDE_DIR $ENV{THIRD_PARTY_ROOT}/include )
	SET( POCO_LIBRARY_DIR $ENV{THIRD_PARTY_ROOT}/lib64 )
ELSE()
	SET( POCO_INCLUDE_DIR /usr/include )
	SET( POCO_LIBRARY_DIR /usr/lib64 )
ENDIF()
SET( POCO_LIBRARIES_RELEASE optimized PocoFoundation optimized PocoUtil )
SET( POCO_LIBRARIES_DEBUG debug PocoFoundationd debug PocoUtild )
SET( POCO_LIBRARIES ${POCO_LIBRARIES_RELEASE} ${POCO_LIBRARIES_DEBUG} )
INCLUDE_DIRECTORIES( ${POCO_INCLUDE_DIR} )
LINK_DIRECTORIES( ${POCO_LIBRARY_DIR} )

#----------------------------------
#ZMQ configuration
IF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	SET( ZMQ_INCLUDE_DIR $ENV{THIRD_PARTY_ROOT}/include )
	SET( ZMQ_LIBRARY_DIR $ENV{THIRD_PARTY_ROOT}/lib64 )
	SET( ZMQ_LIBRARIES_RELEASE optimized libzmqPGM )
	SET( ZMQ_LIBRARIES_DEBUG debug libzmqPGMd )
ELSE()
	SET( ZMQ_INCLUDE_DIR /usr/include )
	SET( ZMQ_LIBRARY_DIR /usr/lib64 )
	SET( ZMQ_LIBRARIES_RELEASE optimized zmq )
	SET( ZMQ_LIBRARIES_DEBUG debug zmq )
ENDIF()
SET( ZMQ_LIBRARIES ${ZMQ_LIBRARIES_RELEASE} ${ZMQ_LIBRARIES_DEBUG} )
INCLUDE_DIRECTORIES( ${ZMQ_INCLUDE_DIR} )
LINK_DIRECTORIES( ${ZMQ_LIBRARY_DIR} )
IF(NOT MSVC)
	SET( ZMQ_LIBRARIES zmq )
ENDIF()

#Compiler configuration
IF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_WARNINGS /EHsc" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
ELSEIF( CMAKE_CXX_COMPILER_ID MATCHES GNU )
#	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic  -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wno-long-long" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wno-long-long" )
ELSEIF( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
#	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic  -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wno-long-long" )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wno-long-long" )
ENDIF()

# Enable GenieDB's UNIV_DEBUG and UNIV_SYNC_DEBUG in debug builds
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUNIV_DEBUG -DUNIV_SYNC_DEBUG")

#----------------------------------
# Macros for collecting source files into a filter structure

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()


MACRO(IMPORT_SUBDIR project dirname)
	FILE(GLOB dir_src
		${CMAKE_CURRENT_SOURCE_DIR}/${project}/${dirname}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/${project}/${dirname}/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/${project}/${dirname}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/${project}/${dirname}/*.hpp
	)
	MESSAGE("creating source group ${dirname}")
	SOURCE_GROUP(${dirname} FILES ${dir_src})
	FILE(TO_CMAKE_PATH ${dirname} native)
	SUBDIRLIST(subdirs ${CMAKE_CURRENT_SOURCE_DIR}/${project}/${native})
	FOREACH(subdir ${subdirs})
		IMPORT_SUBDIR(${project} ${dirname}\\\\${subdir})
	ENDFOREACH(subdir)
ENDMACRO(IMPORT_SUBDIR)

#----------------------------------

IMPORT_SUBDIR( MsgFlood src )

INCLUDE_DIRECTORIES	( ${CMAKE_BINARY_DIR}/Protobuf )

ADD_SUBDIRECTORY ( Protobuf )

FILE ( GLOB_RECURSE MSGFLOOD_FILES MsgFlood/* )
#FILE ( GLOB_RECURSE GETOPT_FILES GetOptSource/* )

SET ( PROTOBUF_SRC_FILES Protobuf/message.pb.cc )
SET_SOURCE_FILES_PROPERTIES ( ${PROTOBUF_SRC_FILES} PROPERTIES GENERATED TRUE)

IF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	INCLUDE_DIRECTORIES	( ${CMAKE_SOURCE_DIR}/GetOptSource )
	FILE ( GLOB_RECURSE GETOPT_SRC_FILES ${CMAKE_SOURCE_DIR}/GetOptSource/*.c )
ENDIF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )

IF( CMAKE_SYSTEM_NAME MATCHES Linux)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
ENDIF( CMAKE_SYSTEM_NAME MATCHES Linux)

ADD_EXECUTABLE(MsgFlood 
	${MSGFLOOD_FILES}
	${PROTOBUF_SRC_FILES}
	${GETOPT_SRC_FILES}
	)

ADD_DEPENDENCIES (MsgFlood Protobuf)

TARGET_LINK_LIBRARIES( MsgFlood 
	${PROTOBUF_LIBRARIES} 
	${ZMQ_LIBRARIES}
	${Boost_LIBRARIES}
	)
LINK_DIRECTORIES( ${PROTOBUF_LIBRARY_PATHS_ALL} )


IF(NOT DEFINED LIBDIR)
	SET(LIBDIR "lib")
ENDIF()
SET(LIBDIR ${LIBDIR} CACHE STRING "lib directory (lib or lib64)")

INSTALL(TARGETS MsgFlood
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIBDIR}
	ARCHIVE DESTINATION ${LIBDIR})
