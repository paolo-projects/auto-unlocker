FIND_PATH(LIBZIP_INCLUDE_DIR
  zip.h
  "$ENV{LIB_DIR}/include"
  "$ENV{INCLUDE}"
  /usr/local/include
  /usr/include
)

FIND_PATH(LIBZIP_CONF_INCLUDE_DIR
  zipconf.h
  "$ENV{LIB_DIR}/include"
  "$ENV{LIB_DIR}/lib/libzip/include"
  "$ENV{LIB}/lib/libzip/include"
  /usr/local/lib/libzip/include
  /usr/lib/libzip/include
  /usr/local/include
  /usr/include
  "$ENV{INCLUDE}"
)

FIND_LIBRARY(LIBZIP_LIBRARY NAMES zip PATHS "$ENV{LIB_DIR}/lib" "$ENV{LIB}" /usr/local/lib /usr/lib )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibZip DEFAULT_MSG
                                  LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR LIBZIP_CONF_INCLUDE_DIR)

SET(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR} ${LIBZIP_CONF_INCLUDE_DIR})
MARK_AS_ADVANCED(LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR LIBZIP_CONF_INCLUDE_DIR LIBZIP_INCLUDE_DIRS)

IF (LIBZIP_FOUND)
  MESSAGE(STATUS "Found libzip: ${LIBZIP_LIBRARY}")
ELSE (LIPZIP_FOUND)
  MESSAGE(FATAL_ERROR "Could not find libzip")
ENDIF (LIBZIP_FOUND)