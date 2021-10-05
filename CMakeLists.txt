cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

cmake_policy(VERSION 3.16)

project(FileCopyServer)

include(GNUInstallDirs)

add_definitions(-Wall)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(/usr/include)
link_directories(/usr/lib)

#Build FileCopyServer
set(SERVER_BIN_NAME "FileCopyServer")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Server SERVER_SOURCE_FILES)

add_executable(${SERVER_BIN_NAME} ${SERVER_SOURCE_FILES})

target_link_libraries(${SERVER_BIN_NAME} FileCopyUtils)


#Build FileCopyClient
set(CLIENT_BIN_NAME "FileCopyClient")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Client CLIENT_SOURCE_FILES)

add_executable(${CLIENT_BIN_NAME} ${CLIENT_SOURCE_FILES})

target_link_libraries(${CLIENT_BIN_NAME} FileCopyUtils)

#Build libFileCopyUtils
set(FILECOPY_LIB_VERSION "0.0.1")
set(FILECOPY_LIB_NAME "FileCopyUtils")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils LIB_SOURCE_FILES)

add_library(${FILECOPY_LIB_NAME} SHARED ${LIB_SOURCE_FILES})

target_link_libraries(${FILECOPY_LIB_NAME} pthread)

set_target_properties(${FILECOPY_LIB_NAME} PROPERTIES VERSION "${FILECOPY_LIB_VERSION}" CLEAN_DIRECT_OUTPUT 1)


#Install include files
install(DIRECTORY include/ DESTINATION $ENV{HOME}/file_copy_app)

#Install libFileCopyUtils
install(TARGETS ${FILECOPY_LIB_NAME}
        LIBRARY DESTINATION $ENV{HOME}/file_copy_app)

#Install FileCopyServer
install(TARGETS ${SERVER_BIN_NAME}
        RUNTIME DESTINATION  $ENV{HOME}/file_copy_app)

#Install FileCopyClient
install(TARGETS ${CLIENT_BIN_NAME}
        RUNTIME DESTINATION  $ENV{HOME}/file_copy_app)