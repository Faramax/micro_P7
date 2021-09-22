cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

#todo: selection of proper platform depending on external variable!
if(MCU)
   add_subdirectory(stm32mp1)
else()
   add_subdirectory(x86)
endif()


