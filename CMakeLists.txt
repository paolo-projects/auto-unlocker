cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project (Unlocker)

#set (BUILD_CURL_EXE OFF)
#set (BUILD_TESTING OFF)
#set (BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
SET(BUILD_SHARED_LIBS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_VERBOSE_MAKEFILE ON)

IF (MSVC)
	# prevent default manifest from being linked
	set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
ENDIF(MSVC)

# include libcurl headers and cmake-project
#include_directories ("${PROJECT_SOURCE_DIR}/curl-7.66.0/include")
#add_subdirectory ( "${PROJECT_SOURCE_DIR}/curl-7.66.0" )

find_package(ZLIB REQUIRED)

if(ZLIB_FOUND)
	message (STATUS "ZLIB found, version ${ZLIB_VERSION_STRING}")
endif()

include_directories (${ZLIB_INCLUDE_DIRS})

find_package(CURL REQUIRED)

if(CURL_FOUND)
	message (STATUS "Curl found, version ${CURL_VERSION_STRING}")
endif()

include_directories(${CURL_INCLUDE_DIRS})

find_package(LibArchive REQUIRED)

if(LibArchive_FOUND)
	message (STATUS "LibArchive found, version ${LibArchive_VERSION}")
endif()

include_directories(${LibArchive_INCLUDE_DIRS})

# include zlib
#include_directories ("${PROJECT_SOURCE_DIR}/libarchive-3.4.0/zlib-1.2.11")

# include libarchive headears
#include_directories ("${PROJECT_SOURCE_DIR}/libarchive-3.4.0/libarchive")

# main include files
include_directories ("${PROJECT_SOURCE_DIR}/include")

# main source files
set (SOURCE_FILES src/unlocker.cpp /
				src/versionparser.cpp /
				src/buildsparser.cpp /
				src/archiveutils.cpp /
				src/netutils.cpp /
				src/debugutils.cpp /
				src/installinfoutils.cpp /
				src/servicestoputils.cpp /
				src/patchutils.cpp /
				src/circularbuffer.cpp /
				)

# if msvc then include manifest for admin privileges
IF (MSVC)
	set (SOURCE_FILES ${SOURCE_FILES} Unlocker.exe.manifest)
	add_compile_definitions( "CURL_STATICLIB" "LIBARCHIVE_STATIC")
ENDIF (MSVC)

add_executable(Unlocker ${SOURCE_FILES})
set_target_properties(Unlocker PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
set_target_properties (Unlocker PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)

target_link_libraries (Unlocker "${ZLIB_LIBRARIES}" -static)
target_link_libraries (Unlocker "${CURL_LIBRARIES}" -static)
target_link_libraries (Unlocker "${LibArchive_LIBRARIES}" -static)

if(MSVC)
	target_link_libraries (Unlocker "ws2_32.lib" "Wldap32.lib")
endif()

#link_directories (${PROJECT_SOURCE_DIR}/libarchive.build/libarchive/Release)

#target_link_libraries (Unlocker libcurl )
#target_link_libraries (Unlocker ${PROJECT_SOURCE_DIR}/libarchive-3.4.0/zlib-1.2.11/build/Release/zlibstatic.lib)
#target_link_libraries (Unlocker ${PROJECT_SOURCE_DIR}/libarchive.build/libarchive/Release/archive_static.lib)