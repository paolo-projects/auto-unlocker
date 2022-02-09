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


find_package(CURL REQUIRED)

if(CURL_FOUND)
	message (STATUS "Curl found, version ${CURL_VERSION_STRING}")
endif()

find_package(LibZip REQUIRED)

include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
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
				src/archive.cpp /
				src/network.cpp /
				src/debug.cpp /
				src/installinfo.cpp /
				src/winservices.cpp /
				src/patcher.cpp /
				src/tar.cpp /
				src/main.cpp /
				src/ziparchive.cpp /
				src/toolsdownloader.cpp)

IF (MSVC)
	IF (UNLOCKER_STATIC_LIBS_WIN)
		# Preprocessor definitions needed to avoid name mangling when statically importing libs on MSVC compiler
		# Name mangling is needed if libraries are built dynamically with MSVC
		# Should not be an issue with other compilers
		add_compile_definitions( "CURL_STATICLIB" )
	ENDIF()
ENDIF (MSVC)

add_executable(Unlocker ${SOURCE_FILES})

# Support skipping file offsets when tar contains files larger than ~2 Gig
target_compile_definitions(Unlocker PUBLIC _FILE_OFFSET_BITS=64)

set_target_properties(Unlocker PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

target_link_libraries (Unlocker ${ZLIB_LIBRARIES})
target_link_libraries (Unlocker ${CURL_LIBRARIES})
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

# Main test

set (TEST_SOURCES tests/test_patch.cpp /
				src/debug.cpp /
				src/patcher.cpp /
				src/network.cpp /
				src/versionparser.cpp /
				src/buildsparser.cpp /
				src/archive.cpp /
				src/installinfo.cpp /
				src/winservices.cpp /
				src/tar.cpp /
				src/unlocker.cpp /
				src/ziparchive.cpp /
				src/toolsdownloader.cpp)

add_executable( TestPatch ${TEST_SOURCES}  "include/unlocker.h")

target_compile_definitions(TestPatch PUBLIC _FILE_OFFSET_BITS=64)

include_directories(TestPatch ${ZLIB_INCLUDE_DIRS})
include_directories(TestPatch ${CURL_INCLUDE_DIRS})
include_directories(TestPatch ${LIBZIP_INCLUDE_DIRS})
target_link_libraries (TestPatch ${ZLIB_LIBRARIES})
target_link_libraries (TestPatch ${CURL_LIBRARIES})
target_link_libraries (TestPatch ${LIBZIP_LIBRARY})

if(WIN32)
	target_link_libraries (TestPatch ws2_32 Wldap32)
endif()

set_target_properties( TestPatch PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

enable_testing()
add_test(NAME TestPatchTest COMMAND TestPatch "${PROJECT_SOURCE_DIR}")

# Tar test

set (TESTTAR_SOURCES tests/test_tar.cpp /
				src/debug.cpp /
				src/patcher.cpp /
				src/versionparser.cpp /
				src/buildsparser.cpp /
				src/archive.cpp /
				src/installinfo.cpp /
				src/winservices.cpp /
				src/tar.cpp /
				src/ziparchive.cpp)

add_executable( TestTar ${TESTTAR_SOURCES}  "include/unlocker.h")

target_compile_definitions(TestTar PUBLIC _FILE_OFFSET_BITS=64)

include_directories(TestTar ${ZLIB_INCLUDE_DIRS})
include_directories(TestTar ${CURL_INCLUDE_DIRS})
include_directories(TestTar ${LIBZIP_INCLUDE_DIRS})
target_link_libraries (TestTar ${ZLIB_LIBRARIES})
target_link_libraries (TestTar ${LIBZIP_LIBRARY})

set_target_properties( TestTar PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

enable_testing()
add_test(NAME TestTarTest COMMAND TestTar "${PROJECT_SOURCE_DIR}")

# Zip test

set (TESTZIP_SOURCES tests/test_zip.cpp /
				src/debug.cpp /
				src/patcher.cpp /
				src/versionparser.cpp /
				src/buildsparser.cpp /
				src/archive.cpp /
				src/installinfo.cpp /
				src/winservices.cpp /
				src/tar.cpp /
				src/ziparchive.cpp /
				src/toolsdownloader.cpp)

add_executable( TestZip ${TESTZIP_SOURCES}  "include/unlocker.h")

target_compile_definitions(TestZip PUBLIC _FILE_OFFSET_BITS=64)

include_directories(TestZip ${ZLIB_INCLUDE_DIRS})
include_directories(TestZip ${CURL_INCLUDE_DIRS})
include_directories(TestZip ${LIBZIP_INCLUDE_DIRS})
target_link_libraries (TestZip ${ZLIB_LIBRARIES})
target_link_libraries (TestZip ${LIBZIP_LIBRARY})

set_target_properties( TestZip PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)

enable_testing()
add_test(NAME TestZipTest COMMAND TestZip "${PROJECT_SOURCE_DIR}")