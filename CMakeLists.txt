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

add_executable(blufi
  main.cpp
  blufiscanner.cpp
  blufiscanner.h
  bluficlient.cpp
  bluficlient.h
  blufiframecoder.cpp
  blufiframecoder.h
  bluficlientframecoderadapter.cpp
  bluficlientframecoderadapter.h
  blufiapplication.cpp
  blufiapplication.h
  cpp.hint
  ${TS_FILES}
)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Bluetooth OpenSSL::Crypto)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
