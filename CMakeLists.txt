cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(VulkanGame VERSION 0.1.0 LANGUAGES C CXX)

message("CMAKE_MODULE_PATH=" ${CMAKE_MODULE_PATH})
message("CMAKE_PREFIX_PATH=" ${CMAKE_PREFIX_PATH})
message("CMAKE_TOOLCHAIN_PATH=" ${CMAKE_TOOLCHAIN_PATH})

if(WIN32)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/ext/")
message("Set Module Path to " ${CMAKE_MODULE_PATH} "Win32")
endif()

set(VG_TARGET_NAME ${PROJECT_NAME})

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_executable(VulkanGame ${SOURCE} ${HEADERS})

#Architecture Detection. 
#https://stackoverflow.com/questions/11944060/how-to-detect-target-architecture-using-cmake
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
set(VG_BUILD_ARCH ${ARCHITECTURE})

# Directories
set(VG_BIN_DIR ${PROJECT_SOURCE_DIR}/bin/)
set(VG_LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
set(VG_BIN_DEBUG_DIR ${VG_BIN_DIR})
set(VG_BIN_RELEASE_DIR ${VG_BIN_DIR})

# Target
if(GENERATOR_IS_MULTI_CONFIG)
  message("Error: Generator is MULTI CONFIG")
   return()
	# "Multi-config generators use CMAKE_CONFIGURATION_TYPES as the set of configurations and ignore CMAKE_BUILD_TYPE."
else()
	message(" + BUILD TYPE " ${CMAKE_BUILD_TYPE})
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(VG_CONFIG_SHORT "d")
	else ()
		set(VG_CONFIG_SHORT "r")
	endif()
endif()

set(VG_OUTPUT_NAME ${VG_TARGET_NAME}-${CMAKE_PROJECT_VERSION}-${VG_BUILD_ARCH}-${VG_CONFIG_SHORT})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${VG_OUTPUT_NAME}
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${VG_BIN_DEBUG_DIR}
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${VG_BIN_RELEASE_DIR}
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${VG_BIN_DEBUG_DIR}
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${VG_BIN_DEBUG_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${VG_LIB_DIR}
)

# Packages
set(OpenGL_GL_PREFERENCE LEGACY) #GLVND or LEGACY
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)  
find_package(Threads REQUIRED) #This is better than -pthread
find_package(Lua REQUIRED)

set(LUAINTF_INCLUDE_DIR ${PROJECT_SOURCE_DIR} "/ext/")

# Includes
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS} ${SDLNET_INCLUDE_DIR} ${VULKAN_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${LUAINTF_INCLUDE_DIR})

# Libraries
set(ADDL_LIBS "")
if(WIN32)
  find_package(SDL2_net REQUIRED)

  #Unicode required for building with Unicode Character Set (vs MBCS)
  ADD_DEFINITIONS(-DUNICODE)
  ADD_DEFINITIONS(-D_UNICODE)

  #Suppress error
  ADD_DEFINITIONS(-D_CRT_NO_VA_START_VALIDATION)

  #Enable multicore build.
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " /MP ") 

  set(ADDL_LIBS "ws2_32.lib" "winmm.lib" "imm32.lib" "version.lib" "dbghelp.lib" "setupapi.lib" "iphlpapi.lib")
elseif(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(ADDL_LIBS ncurses)
  set(CPP_FLAGS_GLOBAL " -pthread ")
  #DEBUG flag -D_GLIBCXX_DEBUG enables string visualization in vscode.
  string(APPEND CMAKE_CXX_FLAGS_DEBUG ${CPP_FLAGS_GLOBAL} " -D_DEBUG -D_GLIBCXX_DEBUG ")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE ${CPP_FLAGS_GLOBAL} )

  #Not sure
  set(VULKAN_LIBRARIES vulkan)
  set(SDLNET_LIBRARIES SDL2_net)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE ${VULKAN_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} ${SDLNET_LIBRARIES} ${ADDL_LIBS} ${LUA_LIBRARIES})

# Debug
if(${LUA_FOUND})
  message("Lua " ${LUA_VERSION_MAJOR} "." ${LUA_VERSION_MINOR} " found.")
endif()
message("LUAINTF_INCLUDE_DIR " ${LUAINTF_INCLUDE_DIR})
message("Vulkan includes:" ${VULKAN_INCLUDE_DIRS})
message("Vulkan libs:" ${VULKAN_LIBRARIES})
