# VG Sources

#message("/src/ current CMAKE src dir is " ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE VG_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp *.c)
file(GLOB_RECURSE VG_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h *.hpp)

# Remove LuaIntf source files (Using header only mode)
set (ITEMS_TO_REMOVE 
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/CppBindClass.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/CppBindModule.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/CppFunction.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/CppObject.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/LuaCompat.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/LuaRef.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/LuaState.cpp
${CMAKE_CURRENT_LIST_DIR}/ext/LuaIntf/src/QtLuaIntf.cpp
)
list(REMOVE_ITEM VG_SOURCE_FILES ${ITEMS_TO_REMOVE})

list(LENGTH VG_SOURCE_FILES VG_SOURCE_FILES_LEN)
#message(" + " ${VG_SOURCE_FILES_LEN} )# " Sources: " ${VG_SOURCE_FILES})
list(LENGTH VG_HEADER_FILES VG_HEADER_FILES_LEN)
#message(" + " ${VG_HEADER_FILES_LEN} ) #" Headers: " ${VG_HEADER_FILES})

set(VG_SOURCE_FILES ${VG_SOURCE_FILES} PARENT_SCOPE)
set(VG_HEADER_FILES ${VG_HEADEPR_FILES} PARENT_SCOPE)

