cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

cmake_policy(VERSION 3.16)

project(FileCopyServer)

include(GNUInstallDirs)

add_definitions(-Wall)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(/usr/include)
link_directories(/usr/lib)

#Build FileCopyServer
set(EXE_BIN_NAME "FileCopyServer")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ EXE_SOURCE_FILES)

add_executable(${EXE_BIN_NAME} ${EXE_SOURCE_FILES})

target_link_libraries(${EXE_BIN_NAME} FileCopyUtils pthread)

#Build libFileCopyUtils
set(FileCopy_LIB_VERSION "0.0.1")
set(FileCopy_LIB_NAME "FileCopyUtils")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils LIB_SOURCE_FILES)

add_library(${FileCopy_LIB_NAME} SHARED ${LIB_SOURCE_FILES})

set_target_properties(${FileCopy_LIB_NAME} PROPERTIES VERSION "${FileCopy_LIB_VERSION}" CLEAN_DIRECT_OUTPUT 1)