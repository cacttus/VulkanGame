
# set(SOURCE
#    ${SOURCE}
#    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/SDLVulkan.cpp
#    PARENT_SCOPE
# )
# set(HEADERS
#    ${HEADERS}
#    ${CMAKE_CURRENT_SOURCE_DIR}/SDLVulkan.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/BaseHeader.h
#    PARENT_SCOPE
# )

#Gather all include & src
set(VG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)

file(GLOB_RECURSE VG_SOURCE_FILES ${VG_SRC_DIR}*.cpp)
file(GLOB_RECURSE VG_HEADER_FILES ${VG_SRC_DIR}*.h)
set (VG_INCLUDE_DIRS "")
foreach (_headerFile ${BR2_HEADERS_BR2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND VG_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES VG_INCLUDE_DIRS)

set(SOURCE
   ${SOURCE}
   ${VG_SOURCE_FILES}
   PARENT_SCOPE
)
set(HEADERS
   ${HEADERS}
   ${VG_HEADER_FILES}
   PARENT_SCOPE
)
