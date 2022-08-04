cmake_minimum_required(VERSION 3.0)

project(uP7example)

include_directories(${PATH_UP7_API})
include_directories(${PATH_UP7_PLATFORM})
include_directories(${PATH_SHARED})
include_directories(${PATH_SHARED_PLATFORM})
include_directories(${PATH_UP7_PROXY_API})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor")

set(P7_PREPROCESSOR_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor
    )

set(P7_GENERATED_HEADERS
        ${P7_PREPROCESSOR_DIR}/uP7IDs.h
        ${P7_PREPROCESSOR_DIR}/uP7preprocessed.h
        )

set(SOURCES 
    cpu.cpp  cpu.h
    host.cpp
    )

add_executable(${PROJECT_NAME}
               ${SOURCES}
               ${P7_GENERATED_HEADERS}
              )

add_custom_command(
        OUTPUT ${P7_GENERATED_HEADERS}
        COMMAND ${PRE_PROCESSOR} "${P7_PREPROCESSOR_DIR}/uP7Preprocessor.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${P7_PREPROCESSOR_DIR}"
        DEPENDS ${SOURCES}
        )

target_link_libraries (${PROJECT_NAME} uP7proxy p7 uP7)

set_ide_folder(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
