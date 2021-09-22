cmake_minimum_required(VERSION 3.0)

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

add_executable(${PROJECT_NAME} 
               common.h
               main.cpp
               t1CreateDestroy.cpp t1CreateDestroy.h
               t2Format.cpp        t2Format.h
               t3Performance.cpp   t3Performance.h
               t4MultiCore.cpp     t4MultiCore.h
               t5Time.cpp          t5Time.h
               t6Resync.cpp        t6Resync.h			   
              )

add_custom_command(TARGET ${PROJECT_NAME}
                   PRE_BUILD
                   COMMAND ${PRE_PROCESSOR} "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor/uP7Testx86.xml" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/uP7preProcessor"
                  )

target_link_libraries (${PROJECT_NAME} uP7proxy p7 uP7)

set_ide_folder(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)