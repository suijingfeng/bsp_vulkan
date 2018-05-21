cmake_minimum_required(VERSION 2.8.4)
project(Q3MapView)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

message(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})



find_package(Vulkan REQUIRED)
message(STATUS "VULKAN_INCLUDE_DIR ${VULKAN_INCLUDE_DIR}")
message(STATUS "VULKAN_LIBRARY ${VULKAN_LIBRARY}")

find_package(SDL2 REQUIRED)
message(STATUS "SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR}")
message(STATUS "SDL2_LIBRARY ${SDL2_LIBRARY}")



set(SOURCE_FILES src/main.cpp
src/Application.hpp
src/Application.cpp
src/Frustum.hpp
src/Frustum.cpp
src/Math.hpp
src/Math.cpp
src/InputHandlers.hpp
src/InputHandlers.cpp
src/StringHelpers.hpp
src/StringHelpers.cpp
src/Utils.hpp
src/Utils.cpp

src/BspMap.hpp
src/StatsUI.hpp



src/q3bsp/Q3BspRenderHelpers.hpp
src/q3bsp/Q3Bsp.hpp
src/q3bsp/Q3BspLoader.hpp
src/q3bsp/Q3BspLoader.cpp
src/q3bsp/Q3BspMap.hpp
src/q3bsp/Q3BspMap.cpp
src/q3bsp/Q3BspPatch.hpp
src/q3bsp/Q3BspPatch.cpp
src/q3bsp/Q3BspStatsUI.cpp
src/q3bsp/Q3BspStatsUI.hpp
    

src/renderer/Camera.hpp
src/renderer/CameraDirector.hpp
src/renderer/Font.hpp
src/renderer/GameTexture.hpp
src/renderer/RenderContext.hpp
src/renderer/TextureManager.hpp
src/renderer/Camera.cpp
src/renderer/CameraDirector.cpp
src/renderer/Font.cpp
src/renderer/GameTexture.cpp
src/renderer/RenderContext.cpp
src/renderer/TextureManager.cpp
src/renderer/stb_image.h
src/renderer/stb_image.c


src/renderer/vulkan/Base.hpp
src/renderer/vulkan/Buffers.hpp
src/renderer/vulkan/CmdBuffer.hpp
src/renderer/vulkan/Device.hpp
src/renderer/vulkan/Image.hpp
src/renderer/vulkan/Pipeline.hpp
src/renderer/vulkan/Validation.hpp
src/renderer/vulkan/vk_mem_alloc.h
src/renderer/vulkan/Base.cpp
src/renderer/vulkan/Buffers.cpp
src/renderer/vulkan/CmdBuffer.cpp
src/renderer/vulkan/Device.cpp
src/renderer/vulkan/Image.cpp
src/renderer/vulkan/Pipeline.cpp
src/renderer/vulkan/Validation.cpp
)


include_directories(${VULKAN_INCLUDE_DIR} ${SDL2_INCLUDE_DIR})
add_executable(Q3MapView ${SOURCE_FILES})
target_link_libraries(Q3MapView ${VULKAN_LIBRARY} ${SDL2_LIBRARY})
