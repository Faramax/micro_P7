cmake_minimum_required(VERSION 3.10)

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

#if(MSVC)
#    add_custom_command(TARGET ${PROJECT_NAME}
#                    PRE_BUILD
#                    COMMAND uP7preProcessor "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Preprocessor.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
#                    )
#else()
#    add_custom_target(ExamplePreProcessor
#                    COMMAND uP7preProcessor "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Preprocessor.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
#                    )
#
#    add_dependencies(${PROJECT_NAME} ExamplePreProcessor)
#endif()

# For Cortex-A7 we use cross-compilation. So the preprocessor must be built for the build platfrorm. We're not sutisfied
# with the default library solution - build the preprocessor as a target and call it to process the sources.
# Instead of that we create the CMake variable PRE_PROCESSOR where store the path to
# the preprocessor binary.It's attached also to the uP7
# target as a property to be used outside of the directory.
add_custom_command(
        OUTPUT  ${P7_GENERATED_HEADERS}
        COMMAND ${PRE_PROCESSOR} "${P7_PREPROCESSOR_DIR}/uP7Preprocessor.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${P7_PREPROCESSOR_DIR}"
        DEPENDS ${SOURCES}
        )

target_link_libraries (${PROJECT_NAME} uP7proxy p7 uP7)

set_ide_folder(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)