# CMakeLists.txt
# To run CMAKE run 
#   ./cmake.sh 
# 
# To run without .sh file: 
#  CMAKE puts everything in the directory that it was run from.  To run CMAKE correctly use the following syntax:
#       CD to /build folder
#       run cmake ..
# This puts everything in the correct folder.
#   Note: The CMAKE_CURRENT_SOURCE_DIR is where CMakeLists.TXT is, not where CMAKE is run from (so it's in the project root directory)
#     https://stackoverflow.com/questions/27247123/how-do-i-clean-up-the-project-files-generated-by-cmake/27254365
#     https://stackoverflow.com/questions/9680420/looking-for-a-cmake-clean-command-to-clear-up-cmake-output
#     https://stackoverflow.com/questions/6921695/how-can-i-build-a-c-project-with-multiple-interdependent-subdirectories
#
# Note: Currently we have some issues
# - [ ] This is currently a test CMakeLists to build the test project.
# - [ ] The output folder isn't /bin/ for some reason.  Fix this.
#

#Force Unix makefile
#We want to build with GCC on windows, using CYGWIN (Install CYGWIN if on Windows)
#set (CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)
message("Generator is set to ${CMAKE_GENERATOR}")

#Needed to print compile command to stdout
#set( CMAKE_VERBOSE_MAKEFILE on )

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Put predefined targets in their own folder.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(VMA_PROJECT_NAME VulkanMarioDemo)
set(VMA_BUILD_ARCH x64) 
set(VMA_PROJECT_VERSION 0.01)
set(VMA_GLOBAL_BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
set(VMA_GLOBAL_BIN_DEBUG_DIR ${VMA_GLOBAL_BIN_DIR}/debug/)
set(VMA_GLOBAL_BIN_RELEASE_DIR ${VMA_GLOBAL_BIN_DIR}/release)
set(VMA_GLOBAL_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
set(VMA_GLOBAL_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/)

if(${GENERATOR_IS_MULTI_CONFIG})
	#Per CMAKE doc.
	# "Multi-config generators (visual studio) use CMAKE_CONFIGURATION_TYPES as the set of 
    # configurations and ignore CMAKE_BUILD_TYPE."
else()
    #CMAKE_BUILD_TYPE is only applicable to single configuration generators.
	message("BUILD TYPE " ${CMAKE_BUILD_TYPE})
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(VMA_CONFIG_SHORT "d")
	else ()
		set(VMA_CONFIG_SHORT "r")
	endif()
	message("BUILD TYPE " ${VMA_CONFIG_SHORT})
endif()
project(${VMA_PROJECT_NAME} VERSION ${VMA_PROJECT_VERSION})

set(CMAKE_CACHEFILE_DIR ${VMA_BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VMA_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${VMA_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${VMA_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VMA_BIN_DIR}) #DLL, not .lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VMA_LIB_DIR}) 

#VulkanMarioDemo Target
set(VMA_SRC_DIR_VMA ${CMAKE_CURRENT_SOURCE_DIR}/src/)

file(GLOB_RECURSE VMA_SOURCES_VMA ${VMA_SRC_DIR_VMA}*.cpp)
file(GLOB_RECURSE VMA_HEADERS_VMA ${VMA_SRC_DIR_VMA}*.h)
set (VMA_INCLUDE_DIRS "")
foreach (_headerFile ${VMA_HEADERS_VMA})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND VMA_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES VMA_INCLUDE_DIRS)

#TEST -- Header directory for temp source file
#list (APPEND VMA_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake_test_src/)
#TEST

message("Info ")
message("  + Found Sources: " ${VMA_SOURCES_VMA})
message("  + Found Headers: " ${VMA_HEADERS_VMA})
message("  + Include Directories: " ${VMA_INCLUDE_DIRS})

set(VMA_TARGET_NAME_VMA ${VMA_PROJECT_NAME})
set(VMA_EXE_NAME_VMA ${VMA_TARGET_NAME_VMA}_${VMA_CONFIG_SHORT}${VMA_BUILD_ARCH})

add_executable(${VMA_TARGET_NAME_VMA} ${VMA_SOURCES_VMA})
#https://stackoverflow.com/questions/24395517/cmake-executable-name
set_target_properties(${VMA_TARGET_NAME_VMA} PROPERTIES OUTPUT_NAME ${VMA_EXE_NAME_VMA})
target_include_directories(${VMA_TARGET_NAME_VMA} PUBLIC ${VMA_HEADERS_VMA})
set_target_properties(${VMA_TARGET_NAME_VMA} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${VMA_GLOBAL_BIN_DEBUG_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${VMA_GLOBAL_BIN_RELEASE_DIR}
    #RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${VMA_GLOBAL_BIN_DEBUG_DIR}
    #RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${VMA_GLOBAL_BIN_DEBUG_DIR}
)

#Linux only?
#https://stackoverflow.com/questions/28395833/using-sdl2-with-cmake
#find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
#find_library(SDL2_LIBRARY NAME SDL2)
#target_include_directories(${VMA_TARGET_NAME_VMA} ${SDL2_INCLUDE_DIR})
#target_link_libraries(${VMA_TARGET_NAME_VMA} ${SDL2_LIBRARY})  

find_package(SDL2 REQUIRED)
include_directories(${VMA_TARGET_NAME_VMA} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${VMA_TARGET_NAME_VMA} ${SDL2_LIBRARIES})

#target_link_libraries(${VMA_TARGET_NAME_VMA} PUBLIC TestLibrary)
#
#Compile Options

function(add_compile_options_config CONFIG)
	#This function adds compile options for a TARGET to debug/release.
	#https://stackoverflow.com/questions/48895687/cmake-add-compile-options-error-with-release-debug-config
    foreach(opt ${ARGN})
        add_compile_options("$<$<CONFIG:${CONFIG}>:${opt}>")
    endforeach()
endfunction()

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	message("Setting GCC/CLANG options.")

#`sdl-config --libs` 
    
    set(CPP_FLAGS_GLOBAL " -W -Wall -pedantic -LSDL2 -pthread -static-libgcc -static-libstdc++")
	
    string(APPEND CMAKE_CXX_FLAGS_DEBUG ${CPP_FLAGS_GLOBAL})
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -O0")
	
    string(APPEND CMAKE_CXX_FLAGS_RELEASE ${CPP_FLAGS_GLOBAL})
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -Ofast") #-Ofast is also -O3
    
    #Add options after file specification
    #add_compile_options_config(RELEASE "sdl2-config" "--cflags" "--libs")
    #add_compile_options_config(DEBUG "sdl2-config" "--cflags" "--libs")
    #add_compile_options(sdl2-config --cflags --libs)
    #add_compile_options("$<$<CONFIG:DEBUG>:sdl2-config --cflags --libs>")
    #add_compile_options("$<$<CONFIG:RELEASE>:sdl2-config --cflags --libs>")
    
endif()

message("  + CXX FLags Debug: " ${CMAKE_CXX_FLAGS_DEBUG})
message("  + CXX FLags Release: " ${CMAKE_CXX_FLAGS_RELEASE})

#
#Set Default Target for Visual Studio
#https://stackoverflow.com/questions/7304625/how-do-i-change-the-startup-project-of-a-visual-studio-solution-via-cmake
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${VMA_TARGET_NAME_VMA})




















