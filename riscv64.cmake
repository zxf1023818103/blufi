set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER riscv64-linux-gnu-g++)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

if(EXISTS /usr/riscv64-linux-gnu)
  set(CMAKE_FIND_ROOT_PATH "/usr/riscv64-linux-gnu;/usr/lib/riscv64-linux-gnu")
  set(ENV{OPENSSL_ROOT_PATH} ${CMAKE_FIND_ROOT_PATH})
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
