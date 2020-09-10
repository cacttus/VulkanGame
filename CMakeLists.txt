cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(VulkanGame VERSION 0.1.0)

#Unlikely we'll need this.
#include(CTest)
#enable_testing()

set(VG_TARGET_NAME ${PROJECT_NAME})

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_executable(VulkanGame ${SOURCE} ${HEADERS})

#Architecture Detection.
#https://stackoverflow.com/questions/11944060/how-to-detect-target-architecture-using-cmake
EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
set(VG_BUILD_ARCH ${ARCHITECTURE})

#Project_source_dir is the project root.
set(VG_BIN_DIR ${PROJECT_SOURCE_DIR}/bin/)
set(VG_LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
set(VG_BIN_DEBUG_DIR ${VG_BIN_DIR})
set(VG_BIN_RELEASE_DIR ${VG_BIN_DIR})

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

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION}) 
include(CPack)

set(OpenGL_GL_PREFERENCE GLVND) #GLVND or LEGACY
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
 
#ncurses library is for getting term info for console colors.
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} vulkan OpenGL::OpenGL SDL2_net ncurses)

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    #set(CPP_FLAGS_GLOBAL " -W -Wall -pedantic -LSDL2 -pthread -static-libgcc -static-libstdc++")
    #-pthread required for std::thread
    set(CPP_FLAGS_GLOBAL " -pthread ")
    # thE DEBUG flag -D_GLIBCXX_DEBUG is from this link - it enables string visualization in vscode. https://github.com/Microsoft/vscode-cpptools/issues/201
    string(APPEND CMAKE_CXX_FLAGS_DEBUG ${CPP_FLAGS_GLOBAL} "-D_DEBUG -D_GLIBCXX_DEBUG ")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE ${CPP_FLAGS_GLOBAL})

    #-rdynamic is required to place all symbols in the table for backtracing unmangled names.
    #-rdynamic is already added in cmake here..
    #string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG "")
    #string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE " -rdynamic ")

endif()

