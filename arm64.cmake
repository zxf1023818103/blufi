set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_SYSTEM_PROCESSOR arm64)

if(EXISTS /usr/aarch64-linux-gnu)
  set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu;/usr/lib/aarch64-linux-gnu")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
