cmake_minimum_required(VERSION 3.0)

project(uP7)

set(PATH_UP7_LIB ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_library(uP7 STATIC
            "${PATH_UP7_API}/uP7.h"
            "${PATH_UP7_API}/uP7helpers.h"
            "${PATH_UP7_API}/uP7protocol.h"
            "${PATH_UP7_PLATFORM}/uP7platform.h"
            uP7context.h
            uP7hash.h
            uP7.c)

set_property(TARGET uP7 PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(uP7
   PUBLIC ${PATH_UP7_PLATFORM}
   PUBLIC ${PATH_UP7_API}
   PUBLIC ${PATH_UP7_PLATFORM_INCLUDES})

# Header-only facade: exposes uP7 include directories so that a couple of uP7
# types can be used in cross-core interfaces (MPU) WITHOUT building/linking uP7.
# On MCU the real uP7/up7_mcu targets are still built and linked.
add_library(up7_types INTERFACE)
target_include_directories(up7_types INTERFACE
   ${PATH_UP7_PLATFORM}
   ${PATH_UP7_API}
   ${PATH_UP7_PLATFORM_INCLUDES})

set_ide_folder(${PROJECT_NAME})




