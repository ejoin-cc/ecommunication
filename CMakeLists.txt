cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/clang/bin/clang")
set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/clang/bin/clang++")

project(EComm
        LANGUAGES C CXX
)

# #####################################################################################################################
# INIT
# #####################################################################################################################
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to `Debug` as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)

if(EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake ..\" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION MATCHES "^14.")
        message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    else()
        message(WARNING "!! We recommend that you use clang-14 for developing. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, a different version.")
    endif()
else()
    message(WARNING "!! We recommend that you use clang-14 for developing. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, which is not clang.")
endif()

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

set(ECOMM_SCRIPT_DIR "${CMAKE_SOURCE_DIR}/script")
set(ECOMM_CLANG_BIN_DIR "${CMAKE_SOURCE_DIR}/clang/bin")

enable_testing()

find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-14
        HINTS ${ECOMM_CLANG_BIN_DIR})

if("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Couldn't find clang-format.")
else()
    message(STATUS "Found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################

# Compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes.
set(ECOMM_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(ECOMM_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(ECOMM_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        ${PROJECT_SOURCE_DIR}/third_party/backward-cpp
)

include_directories(${ECOMM_SRC_INCLUDE_DIR} ${ECOMM_TEST_INCLUDE_DIR} ${ECOMM_THIRD_PARTY_INCLUDE_DIR})
include_directories(BEFORE src)

# #####################################################################################################################
# Other CMake modules
# MUST BE ADDED AFTER CONFIGURING COMPILER PARAMETERS
# #####################################################################################################################

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(test)

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

string(CONCAT ECOMM_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
)

# Runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${ECOMM_SCRIPT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${ECOMM_SCRIPT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${ECOMM_FORMAT_DIRS}
        --quiet
        --style
        "file:${ECOMM_SCRIPT_DIR}/.clang-format"
)