cmake_minimum_required(VERSION 3.10)

project(uP7example)

include_directories(${PATH_UP7_API})
include_directories(${PATH_UP7_PLATFORM})
include_directories(${PATH_SHARED})
include_directories(${PATH_SHARED_PLATFORM})
include_directories(${PATH_UP7_PROXY_API})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor")

add_executable(${PROJECT_NAME} 
               cpu.cpp  cpu.h
               host.cpp
              )

if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME}
                    PRE_BUILD
                    COMMAND uP7preProcessor "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Preprocessor.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
                    )
else()
    add_custom_target(ExamplePreProcessor 
                    COMMAND uP7preProcessor "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Preprocessor.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
                    )

    add_dependencies(${PROJECT_NAME} ExamplePreProcessor)
endif()

target_link_libraries (${PROJECT_NAME} uP7proxy p7 uP7)

set_ide_folder(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)