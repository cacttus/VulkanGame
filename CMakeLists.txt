cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(VulkanGame VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")
add_executable(VulkanGame ${SOURCE} ${HEADERS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

#find_package(OpenGL REQUIRED)
#find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)
#find_package(SDL2_net REQUIRED)
include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} vulkan ${OPENGL_gl_LIBRARY} SDL2_net)


if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	message("Setting GCC/CLANG options.")
    #set(CPP_FLAGS_GLOBAL " -W -Wall -pedantic -LSDL2 -pthread -static-libgcc -static-libstdc++")
    set(CPP_FLAGS_GLOBAL " -pthread ")

    string(APPEND CMAKE_CXX_FLAGS_DEBUG ${CPP_FLAGS_GLOBAL} " -D_DEBUG")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE ${CPP_FLAGS_GLOBAL})
endif()
