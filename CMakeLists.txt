cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project (Unlocker)

SET(UNLOCKER_STATIC_LIBS_WIN OFF CACHE BOOL "Links statically")

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

find_package(LibArchive REQUIRED)

if(LibArchive_FOUND)
	message (STATUS "LibArchive found, version ${LibArchive_VERSION}")
endif()

include_directories(${LibArchive_INCLUDE_DIRS})

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
				)

# if msvc then include manifest for admin privileges
IF (MSVC)
	set (SOURCE_FILES ${SOURCE_FILES} Unlocker.exe.manifest)
	IF (UNLOCKER_STATIC_LIBS_WIN)
		add_compile_definitions( "CURL_STATICLIB" "LIBARCHIVE_STATIC")
	ENDIF()
ENDIF (MSVC)

add_executable(Unlocker ${SOURCE_FILES})
set_target_properties(Unlocker PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS ON)
set_target_properties (Unlocker PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)

target_link_libraries (Unlocker "${ZLIB_LIBRARIES}")
target_link_libraries (Unlocker "${CURL_LIBRARIES}")
target_link_libraries (Unlocker "${LibArchive_LIBRARIES}")

if(MSVC)
	target_link_libraries (Unlocker "ws2_32.lib" "Wldap32.lib")
endif()

