set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER s390x-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER s390x-linux-gnu-g++)
set(CMAKE_SYSTEM_PROCESSOR s390x)

if(EXISTS /usr/s390x-linux-gnu)
  set(CMAKE_FIND_ROOT_PATH "/usr/s390x-linux-gnu;/usr/lib/s390x-linux-gnu")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
