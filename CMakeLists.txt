cmake_minimum_required(VERSION 3.14)

project(blufi LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} log --pretty=format:'%h' -n 1
                OUTPUT_VARIABLE GIT_REV
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                ERROR_QUIET)

if ("${GIT_REV}" STREQUAL "")
  set(GIT_REV "N/A")
  set(GIT_DIFF "")
  set(GIT_TAG "N/A")
  set(GIT_BRANCH "N/A")
else()
  execute_process(
    COMMAND ${GIT_EXECUTABLE} diff --quiet --exit-code
    OUTPUT_VARIABLE GIT_DIFF
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_DIFF_RETURN)
  if (${GIT_DIFF_RETURN} EQUAL 0)
    set(GIT_DIFF "+")
  endif()

  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --exact-match --tags
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH)

  string(STRIP "${GIT_REV}" GIT_REV)
  string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
  string(STRIP "${GIT_DIFF}" GIT_DIFF)
  string(STRIP "${GIT_TAG}" GIT_TAG)
  string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

add_compile_definitions(GIT_REV="${GIT_REV}")
add_compile_definitions(GIT_BRANCH="${GIT_BRANCH}")

if (NOT "${GIT_TAG}" STREQUAL "")
  add_compile_definitions(GIT_TAG="${GIT_TAG}")
  set(CPACK_PACKAGE_VERSION "${GIT_TAG}")
else()
  set(CPACK_PACKAGE_VERSION "0${GIT_DIFF}git.${GIT_REV}")
endif()

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

if(WIN32)
  include(GNUInstallDirs)
  install(TARGETS bluficlient blufiserver RUNTIME_DEPENDENCY_SET ${PROJECT_NAME}_runtime_dependencies)
  install(RUNTIME_DEPENDENCY_SET ${PROJECT_NAME}_runtime_dependencies PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-" POST_EXCLUDE_REGEXES ".*system32/.*\\.dll")
endif()

set(CPACK_PACKAGE_NAME blufi)
set(CPACK_PACKAGE_DESCRIPTION "Configure Wi-Fi network for IoT device via Bluetooth channel through Blufi protocol.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Blufi Client and Server Applications")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README.md)
set(CPACK_PACKAGE_VENDOR "Zeng Xiangfei")
set(CPACK_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_PACKAGE_CONTACT "zxf1023818103@gmail.com")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6bluetooth6, libssl3, libcap2-bin")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/zxf1023818103/blufi")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_SOURCE_DIR}/postinst)
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "powerpc64le")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "ppc64el")
else()
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_RPM_PACKAGE_LICENSE ${CPACK_PACKAGE_LICENSE})
set(CPACK_RPM_PACKAGE_GROUP "Unspecified")
#set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtconnectivity, libcap")
#set(CPACK_RPM_PACKAGE_REQUIRES_POST "")
set(CPACK_RPM_SPEC_MORE_DEFINE "%define debug_package %{nil}
    %define __os_install_post %{nil}")
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_RPM_PACKAGE_URL ${CPACK_DEBIAN_PACKAGE_HOMEPAGE})
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA})
set(CPACK_RPM_PACKAGE_RELOCATABLE ON)

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/PackageConfig.cmake)

include(CPack)
