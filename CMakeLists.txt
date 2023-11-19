cmake_minimum_required(VERSION 3.16)

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
  else()
    set(GIT_DIFF "-")
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

if ("${GIT_TAG}" STREQUAL "")
  set(GIT_TAG "0")
endif()

set(CPACK_PACKAGE_VERSION "${GIT_TAG}${GIT_DIFF}git.${GIT_REV}")

add_compile_definitions(GIT_REV="${GIT_REV}")
add_compile_definitions(GIT_BRANCH="${GIT_BRANCH}")
add_compile_definitions(GIT_TAG="${GIT_TAG}")
add_compile_definitions(VERSION="${CPACK_PACKAGE_VERSION}")

find_package(Qt6 REQUIRED COMPONENTS Core LinguistTools Bluetooth)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

qt_standard_project_setup(REQUIRES 6.5)

set(TS_FILES blufi_zh_CN.ts)

qt_add_library(blufiframecoder STATIC
    blufiframecoder.cpp
    blufiframecoder.h
)
target_link_libraries(blufiframecoder PUBLIC Qt::Core Qt::Bluetooth)

qt_add_executable(bluficlient
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
target_link_libraries(bluficlient PUBLIC blufiframecoder Qt::Core Qt::Bluetooth OpenSSL::Crypto)
set_target_properties(bluficlient PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)

set(BLUFI_SERVER_SRCS
  blufiservermain.cpp
  blufiserver.cpp
  blufiserver.h
  blufiserverframecoderadapter.cpp
  blufiserverframecoderadapter.h
  blufiserverapplication.cpp
  blufiserverapplication.h
  blufiwlancontrolagent.h
  cpp.hint
)

if (APPLE)
  qt_add_executable(blufiserver
    ${BLUFI_SERVER_SRCS}
    blufiwlancontrolagent_darwin.mm
    ${TS_FILES}
  )
  target_link_libraries(blufiserver PUBLIC blufiframecoder Qt::Core Qt::Bluetooth OpenSSL::Crypto)
elseif (WIN32)
  qt_add_executable(blufiserver
    ${BLUFI_SERVER_SRCS}
    blufiwlancontrolagent_win32.cpp
    ${TS_FILES}
  )
  target_link_libraries(blufiserver PUBLIC blufiframecoder Qt::Core Qt::Bluetooth OpenSSL::Crypto)
elseif (LINUX)
  qt_add_executable(blufiserver
    ${BLUFI_SERVER_SRCS}
    blufiwlancontrolagent_networkmanager.cpp
    ${TS_FILES}
  )
  find_package(Qt6 REQUIRED COMPONENTS Dbus)
  target_link_libraries(blufiserver PUBLIC blufiframecoder Qt::Core Qt::Bluetooth Qt::Dbus OpenSSL::Crypto)
endif()

set_target_properties(blufiserver PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)

if (APPLE)
  if (IOS)
    set_target_properties(bluficlient PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.cmake.ios.plist")
    set_target_properties(blufiserver PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.cmake.ios.plist")
  else()
    set_target_properties(bluficlient PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.cmake.macos.plist")
    set_target_properties(blufiserver PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.cmake.macos.plist")
  endif()
endif()

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

install(TARGETS bluficlient blufiserver BUNDLE DESTINATION ${CMAKE_BINARY_DIR})

if(WIN32)
  set(CPACK_GENERATOR External)
else()
  set(CPACK_GENERATOR 7Z ZIP)
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
