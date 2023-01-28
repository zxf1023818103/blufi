set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER powerpc64le-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER powerpc64le-linux-gnu-g++)
set(CMAKE_SYSTEM_PROCESSOR ppc64el)

if(EXISTS /usr/powerpc64le-linux-gnu)
  set(ENV{OPENSSL_ROOT_DIR} "/usr/powerpc64le-linux-gnu;/usr/lib/powerpc64le-linux-gnu")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
