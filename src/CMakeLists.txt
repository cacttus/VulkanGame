message("/src/ current CMAKE src dir is " ${CMAKE_CURRENT_SOURCE_DIR})
set(VG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)

file(GLOB_RECURSE VG_SOURCE_FILES ${VG_SRC_DIR}*.cpp *.c)
file(GLOB_RECURSE VG_HEADER_FILES ${VG_SRC_DIR}*.h *.hpp)

#message(" + Headers " ${VG_HEADER_FILES})
#message(" + Sources " ${VG_SOURCE_FILES})

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
