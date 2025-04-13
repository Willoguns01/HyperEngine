
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################
include(${CMAKE_CURRENT_LIST_DIR}/daxa-targets.cmake)
check_required_components(daxa)

get_target_property(DAXA_PREV_DEFINITIONS daxa::daxa INTERFACE_COMPILE_DEFINITIONS)
set_target_properties(daxa::daxa PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "${DAXA_PREV_DEFINITIONS};DAXA_SHADER_INCLUDE_DIR=\"${PACKAGE_PREFIX_DIR}/include\""
)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
