set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_SYSTEM_PROCESSOR armhf)

#if(EXISTS /usr/arm-linux-gnueabihf)
#  set(CMAKE_FIND_ROOT_PATH "/usr/arm-linux-gnueabihf;/usr/lib/arm-linux-gnueabihf")
#  set(ENV{OPENSSL_ROOT_PATH} /usr/arm-linux-gnueabihf)
#endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
