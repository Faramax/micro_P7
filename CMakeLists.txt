cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(microP7)

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# OPTIONS:
set(BUILD_UP7_LIB          ON CACHE INTERNAL "build uP7 library")
set(BUILD_UP7_P7_LIB       ON CACHE INTERNAL "build P7 library")
set(BUILD_UP7_PROXY        ON CACHE INTERNAL "build uP7 proxy library")
set(BUILD_UP7_PREPROCESSOR ON CACHE INTERNAL "build uP7 preprocessor")
set(BUILD_UP7_TEST         ON CACHE INTERNAL "build uP7 tests")
set(BUILD_UP7_EXAMPLE      ON CACHE INTERNAL "build uP7 example")
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

function(enable_c_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_C_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_c_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

function(enable_cpp_c_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
    endif()
    
    string(FIND "${CMAKE_C_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_c_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

set(ROOT_UP7_PATH ${PROJECT_SOURCE_DIR} CACHE INTERNAL "")

if(MSVC)
    add_definitions(-D_UNICODE)
    add_definitions(-DUNICODE)

    if(NOT COMMAND set_ide_folder)
        macro(set_ide_folder _project_name)
            string(LENGTH ${ROOT_UP7_PATH} ROOT_UP7_PATH_LEN)
            string(LENGTH ${CMAKE_CURRENT_SOURCE_DIR} CURRENT_SOURCE_DIR_LEN)
            MATH(EXPR ROOT_UP7_PATH_LEN "${ROOT_UP7_PATH_LEN}+1")
            MATH(EXPR CURRENT_SOURCE_DIR_LEN "${CURRENT_SOURCE_DIR_LEN} - ${ROOT_UP7_PATH_LEN}")

            string(SUBSTRING ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_UP7_PATH_LEN} ${CURRENT_SOURCE_DIR_LEN} IDE_SUBFOLDER)
            set_property(TARGET ${_project_name} PROPERTY FOLDER ${IDE_SUBFOLDER})

            message(${_project_name})
        endmacro()
    endif()

else()
    enable_c_compiler_flag_if_supported("-std=gnu99")

    enable_cpp_c_compiler_flag_if_supported("-Wall")
    enable_cpp_c_compiler_flag_if_supported("-Wextra")
    enable_cpp_c_compiler_flag_if_supported("-pedantic")
    enable_cpp_c_compiler_flag_if_supported("-D_FILE_OFFSET_BITS=64")
    enable_cpp_c_compiler_flag_if_supported("-D_LARGEFILE64_SOURCE")
    enable_cpp_c_compiler_flag_if_supported("-fvisibility=hidden")
    enable_cpp_c_compiler_flag_if_supported("-Wno-variadic-macros")
    enable_cpp_c_compiler_flag_if_supported("-Wno-long-long")
    enable_cpp_c_compiler_flag_if_supported("-Wno-missing-field-initializers")
    enable_cpp_c_compiler_flag_if_supported("-Wno-unknown-pragmas")
    enable_cpp_c_compiler_flag_if_supported("-Wno-address-of-packed-member")
    
    macro(set_ide_folder _project_name)
    endmacro()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


if(BUILD_UP7_TEST OR BUILD_UP7_EXAMPLE)
    if (NOT BUILD_UP7_LIB)
        message("WARNING: require to activate up7")
        SET(BUILD_UP7_LIB ON CACHE INTERNAL "up7")
    endif()
    if (NOT BUILD_UP7_P7_LIB)
        message("WARNING: require to activate p7")
        SET(BUILD_UP7_P7_LIB ON CACHE INTERNAL "p7")
    endif()
    if (NOT BUILD_UP7_PROXY)
        message("WARNING: require to activate up7 proxy")
        SET(BUILD_UP7_PROXY ON CACHE INTERNAL "proxy")
    endif()

    if (NOT BUILD_UP7_PREPROCESSOR)
        message("WARNING: require to activate up7 preprocessor")
        SET(BUILD_UP7_PREPROCESSOR ON CACHE INTERNAL "proxy")
    endif()
endif()

if(BUILD_UP7_PROXY)
    if (NOT BUILD_UP7_P7_LIB)
        message("WARNING: require to activate p7")
        SET(BUILD_UP7_P7_LIB ON CACHE INTERNAL "p7")
    endif()
endif()


if(uP7_EXTERNAL_INTEGRATION)
else()
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/_Build_)
endif()

if (BUILD_UP7_P7_LIB)
    if (TARGET p7)
        message("INFO: P7 target is already existing, scipping local one")
    else()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/P7lib")
           add_subdirectory(P7lib)
        else()
            message(FATAL_ERROR "Can't find P7 library!")
        endif()
    endif()
else()
    set(PATH_P7_API ${CMAKE_CURRENT_SOURCE_DIR}/P7lib/Headers CACHE INTERNAL "")
    set(PATH_SHARED ${CMAKE_CURRENT_SOURCE_DIR}/P7lib/Shared CACHE INTERNAL "")

    if(WIN32)
        set(PATH_SHARED_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/P7lib/Shared/Platforms/Windows_x86 CACHE INTERNAL "")
    else()
        set(PATH_SHARED_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/P7lib/Shared/Platforms/Linux_x86 CACHE INTERNAL "")
    endif()
endif()


#message("INFO: BUILD_UP7_P7_LIB:${BUILD_UP7_P7_LIB}")
#message("INFO: BUILD_UP7_LIB:${BUILD_UP7_LIB}")
#message("INFO: BUILD_UP7_PROXY:${BUILD_UP7_PROXY}")
#message("INFO: BUILD_UP7_PREPROCESSOR:${BUILD_UP7_PREPROCESSOR}")
#message("INFO: BUILD_UP7_TEST:${BUILD_UP7_TEST}")
#message("INFO: BUILD_UP7_EXAMPLE:${BUILD_UP7_EXAMPLE}")


if (BUILD_UP7_LIB)
    add_subdirectory(uP7lib)
else()
    set(PATH_UP7_API ${CMAKE_CURRENT_SOURCE_DIR}/uP7lib/Headers CACHE INTERNAL "")
    set(PATH_UP7_LIB ${CMAKE_CURRENT_SOURCE_DIR}/uP7lib/Source CACHE INTERNAL "")
    set(PATH_UP7_LIB ${CMAKE_CURRENT_SOURCE_DIR}/uP7lib/Source CACHE INTERNAL "")
    set(PATH_UP7_PLATFORM ${CMAKE_CURRENT_SOURCE_DIR}/uP7lib/Platforms/x86 CACHE INTERNAL "")
endif()

if (BUILD_UP7_PROXY)
    add_subdirectory(uP7proxyLib)
else()
    set(PATH_UP7_PROXY_API ${CMAKE_CURRENT_SOURCE_DIR}/uP7proxyLib/Headers CACHE INTERNAL "")
    set(PATH_UP7_PROXY_LIB ${CMAKE_CURRENT_SOURCE_DIR}/uP7proxyLib/Source CACHE INTERNAL "")
endif()

if (BUILD_UP7_PREPROCESSOR)
    add_subdirectory(uP7preProcessor)
endif()

if (BUILD_UP7_TEST)
    add_subdirectory(uP7test)
endif()

if (BUILD_UP7_EXAMPLE)
    add_subdirectory(uP7example)
endif()
