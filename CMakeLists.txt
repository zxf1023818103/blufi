cmake_minimum_required(VERSION 3.14)

project(blufi LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools Bluetooth)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools Bluetooth)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

set(TS_FILES blufi_zh_CN.ts)

add_library(blufiframecoder
    blufiframecoder.cpp
    blufiframecoder.h
)
target_link_libraries(blufiframecoder Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Bluetooth)

add_executable(bluficlient
  bluficlientmain.cpp
  blufiscanner.cpp
  blufiscanner.h
  bluficlient.cpp
  bluficlient.h
  bluficlientframecoderadapter.cpp
  bluficlientframecoderadapter.h
  bluficlientapplication.cpp
  bluficlientapplication.h
  cpp.hint
  ${TS_FILES}
)
target_link_libraries(bluficlient blufiframecoder Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Bluetooth OpenSSL::Crypto)

add_executable(blufiserver
  blufiservermain.cpp
  blufiserver.cpp
  blufiserver.h
  blufiserverframecoderadapter.cpp
  blufiserverframecoderadapter.h
  blufiserverapplication.cpp
  blufiserverapplication.h
  cpp.hint
  ${TS_FILES}
)
target_link_libraries(blufiserver blufiframecoder Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Bluetooth OpenSSL::Crypto)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

install(TARGETS bluficlient blufiserver)
