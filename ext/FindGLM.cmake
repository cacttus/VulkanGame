# # Find GLM
# # Defines:
# # ${VULKAN_INCLUDE_DIRS} 
# # ${VULKAN_LIBRARIES}
# 
# if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#   set(VC_LIB_PATH_SUFFIX Lib)
# else()
#   set(VC_LIB_PATH_SUFFIX Lib32)
# endif()
# 
# FIND_PATH(VULKAN_INCLUDE_DIR
#   NAMES
#   vulkan.h
#   HINTS
#     ${CMAKE_SOURCE_DIR}/ext/LunarG/VulkanSDK/1.1.114.0/
#   PATH_SUFFIXES
#     Include Include/vulkan
# )
# 
# FIND_LIBRARY(VULKAN_LIBRARY
#   NAMES
#   vulkan-1
#   HINTS
#   ${CMAKE_SOURCE_DIR}/ext/LunarG/VulkanSDK/1.1.114.0/${VC_LIB_PATH_SUFFIX}
#   PATH_SUFFIXES
#     lib64 lib
# )
# 
# include(FindPackageHandleStandardArgs)
# find_package_handle_standard_args(VULKAN DEFAULT_MSG VULKAN_LIBRARY VULKAN_INCLUDE_DIR)
# 
# IF(VULKAN_FOUND)
# message("Found VULKAN: include:" ${VULKAN_INCLUDE_DIR} " lib:" ${VULKAN_LIBRARY})
# set(VULKAN_INCLUDE_DIRS ${VULKAN_INCLUDE_DIR})
# set(VULKAN_LIBRARIES ${VULKAN_LIBRARY})
# ENDIF(VULKAN_FOUND)
# 
# mark_as_advanced(
#   VULKAN_INCLUDE_DIR
#   VULKAN_LIBRARY
# )
