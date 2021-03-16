cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project (Unlocker)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/modules)

SET(UNLOCKER_STATIC_LIBS_WIN ON CACHE BOOL "Links statically") # Set to OFF for dynamic linking

IF (MSVC)
	# prevent default manifest from being linked
	set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
ENDIF(MSVC)

find_package(ZLIB REQUIRED)

if(ZLIB_FOUND)
	message (STATUS "ZLib found, version ${ZLIB_VERSION_STRING}")
endif()

include_directories(${ZLIB_INCLUDE_DIRS})

find_package(CURL REQUIRED)

if(CURL_FOUND)
	message (STATUS "Curl found, version ${CURL_VERSION_STRING}")
endif()

include_directories(${CURL_INCLUDE_DIRS})

#find_package(LibArchive REQUIRED)

#if(LibArchive_FOUND)
#	message (STATUS "LibArchive found, version ${LibArchive_VERSION}")
#endif()

#include_directories(${LibArchive_INCLUDE_DIRS})

find_package(LibZip REQUIRED)

include_directories(${LIBZIP_INCLUDE_DIRS})

if(LIBZIP_FOUND)
	message (STATUS "LibZip found")
endif()

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
				src/tar.cpp )

IF (MSVC)
	IF (UNLOCKER_STATIC_LIBS_WIN)
		# Preprocessor definitions needed to avoid name mangling when statically importing libs on MSVC compiler
		# Name mangling is needed if libraries are built dynamically with MSVC
		# Should not be an issue with other compilers
		add_compile_definitions( "CURL_STATICLIB" )
	ENDIF()
ENDIF (MSVC)

add_executable(Unlocker ${SOURCE_FILES} )

set_target_properties(Unlocker PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

if(LINUX)
	target_link_libraries (Unlocker -lpthread)
endif()

target_link_libraries (Unlocker ${ZLIB_LIBRARIES})
target_link_libraries (Unlocker ${CURL_LIBRARIES})
#target_link_libraries (Unlocker ${LibArchive_LIBRARIES})
target_link_libraries (Unlocker ${LIBZIP_LIBRARY})

if(WIN32)
	target_link_libraries (Unlocker ws2_32 Wldap32)
endif()

# Amend manifest to tell Windows that the app needs admin privileges
IF (MSVC)
	IF (CMAKE_MAJOR_VERSION LESS 3)
		MESSAGE(WARNING "CMake version 3.0 or newer is required use build variable TARGET_FILE")
	ELSE()
		ADD_CUSTOM_COMMAND(
			TARGET Unlocker
			POST_BUILD
			COMMAND "mt.exe" -manifest \"${PROJECT_SOURCE_DIR}/Unlocker.exe.manifest\" -outputresource:\"$<TARGET_FILE:Unlocker>\"\;\#1
			COMMENT "Embedding manifest..." 
		)
	ENDIF()
ENDIF(MSVC)

set (TEST_SOURCES tests/test_patch.cpp /
				src/debugutils.cpp /
				src/patchutils.cpp /
				src/tar.cpp )

add_executable( TestPatch ${TEST_SOURCES} )

set_target_properties( TestPatch PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

enable_testing()
add_test(NAME TestPatchTest COMMAND TestPatch "${PROJECT_SOURCE_DIR}")