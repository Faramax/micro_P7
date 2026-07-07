cmake_minimum_required(VERSION 3.10)

project(up7Tests)

include_directories(${PATH_UP7_API})
include_directories(${PATH_UP7_PLATFORM})
include_directories(${PATH_UP7_LIB})
include_directories(${PATH_SHARED})
include_directories(${PATH_SHARED_PLATFORM})
include_directories(${PATH_P7_API})
include_directories(${PATH_UP7_PROXY_API})
include_directories("${PATH_P7}/Sources")
message("${PATH_P7}/Sources")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor")

set(P7_PREPROCESSOR_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor
    )

set(P7_GENERATED_HEADERS
        ${P7_PREPROCESSOR_DIR}/uP7IDs.h
        ${P7_PREPROCESSOR_DIR}/uP7preprocessed.h
        )

set(SOURCES
        common.h
        main.cpp
        t1CreateDestroy.cpp t1CreateDestroy.h
        t2Format.cpp        t2Format.h
        t3Performance.cpp   t3Performance.h
        t4MultiCore.cpp     t4MultiCore.h
        t5Time.cpp          t5Time.h
        t6Resync.cpp        t6Resync.h
        t7Pattern.cpp       t7Pattern.h
   )

add_executable(${PROJECT_NAME}
               ${SOURCES}
               ${P7_GENERATED_HEADERS}
              )

# For Cortex-A7 we use cross-compilation. So the preprocessor must be built for the build platfrorm. We're not sutisfied
# with the default library solution - build the preprocessor as a target and call it to process the sources.
# Instead of that we create the CMake variable PRE_PROCESSOR where store the path to
# the preprocessor binary.It's attached also to the uP7
# target as a property to be used outside of the directory.
add_custom_command(
        OUTPUT ${P7_GENERATED_HEADERS}
        COMMAND ${PRE_PROCESSOR} "${P7_PREPROCESSOR_DIR}/uP7Testx86.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${P7_PREPROCESSOR_DIR}"
        DEPENDS ${SOURCES}
        )

#if(MSVC)
#    add_custom_command(TARGET ${PROJECT_NAME}
#                    PRE_BUILD
#                    COMMAND uP7preProcessor "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Testx86.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
#                    )
#else()
#    add_custom_target(TestPreProcessor 
#                    COMMAND uP7preProcessor "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Testx86.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
#                    )
#
#    add_dependencies(${PROJECT_NAME} TestPreProcessor)
#endif()

target_link_libraries (${PROJECT_NAME} uP7proxy p7 uP7)

set_ide_folder(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)