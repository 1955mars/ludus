cmake_minimum_required(VERSION 3.4.1)

project(questart)

set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../third-party")
set(MAIN_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/common")
set(MAIN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/include")
set(PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/platform/windows")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out)

set(LIB_SDL2 ${THIRD_PARTY_DIR}/sdl-windows/lib/x64/SDL2.lib)
set(LIB_SDL2_MAIN ${THIRD_PARTY_DIR}/sdl-windows/lib/x64/SDL2main.lib)
set(LIB_GLEW ${THIRD_PARTY_DIR}/glew/lib/Release/x64/glew32s.lib)
set(LIB_SDL2_IMAGE ${THIRD_PARTY_DIR}/sdl-image-windows/lib/x64/SDL2_image.lib)
set(LIB_VULKAN ${THIRD_PARTY_DIR}/vulkan-windows/Lib/vulkan-1.lib)

include_directories(${THIRD_PARTY_DIR}/sdl-windows/include)
include_directories(${THIRD_PARTY_DIR}/glew/include)
include_directories(${THIRD_PARTY_DIR}/glm)
include_directories(${THIRD_PARTY_DIR}/ktx/include)
include_directories(${THIRD_PARTY_DIR}/ktx/other_include/)
include_directories(${THIRD_PARTY_DIR}/tiny-obj-loader)
include_directories(${THIRD_PARTY_DIR}/sdl-image-windows/include)
include_directories(${THIRD_PARTY_DIR}/vulkan-windows/Include)
include_directories(${PLATFORM_SOURCE_DIR}/scene)
include_directories(${MAIN_SOURCE_DIR}/core)
include_directories(${MAIN_SOURCE_DIR}/sdl)
include_directories(${MAIN_SOURCE_DIR}/graphics/vulkan/common/)
include_directories(${MAIN_SOURCE_DIR}/graphics/opengl/)
include_directories(${MAIN_INCLUDE_DIR})

file(GLOB_RECURSE MAIN_INCLUDE_HEADERS ${MAIN_INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE CORE_CPP_HEADERS ${MAIN_SOURCE_DIR}/core/*.hpp)
file(GLOB_RECURSE CORE_CPP_SOURCES ${MAIN_SOURCE_DIR}/core/*.cpp)
file(GLOB_RECURSE OPENGL_CPP_HEADERS ${MAIN_SOURCE_DIR}/graphics/opengl/*.hpp)
file(GLOB_RECURSE OPENGL_CPP_SOURCES ${MAIN_SOURCE_DIR}/graphics/opengl/*.cpp)
file(GLOB_RECURSE VULKAN_CPP_HEADERS ${MAIN_SOURCE_DIR}/graphics/vulkan/common/*.hpp)
file(GLOB_RECURSE VULKAN_CPP_SOURCES ${MAIN_SOURCE_DIR}/graphics/vulkan/common/*.cpp)
file(GLOB_RECURSE VULKAN_WINDOWS_CPP_HEADERS ${MAIN_SOURCE_DIR}/graphics/vulkan/windows/*.hpp)
file(GLOB_RECURSE VULKAN_WINDOWS_CPP_SOURCES ${MAIN_SOURCE_DIR}/graphics/vulkan/windows/*.cpp)
file(GLOB_RECURSE SDL_CPP_HEADERS ${MAIN_SOURCE_DIR}/sdl/*.hpp)
file(GLOB_RECURSE SDL_CPP_SOURCES ${MAIN_SOURCE_DIR}/sdl/*.cpp)
file(GLOB_RECURSE PLATFORM_CPP_HEADERS ${PLATFORM_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE PLATFORM_CPP_SOURCES ${PLATFORM_SOURCE_DIR}/*.cpp)

# If we are generating a release build, then request that we produce a Windows desktop
# application executable by specifying WIN32 as an executable option. This avoids the
# console window from appearing when we launch the .exe file. However for debug builds,
# the console window makes it really easy to view logging output so we'll allow it by
# NOT specifying the WIN32 executable option.

if(CMAKE_BUILD_TYPE MATCHES "Release")
	set(EXECUTABLE_TYPE WIN32)
endif()

add_executable(
	questart ${EXECUTABLE_TYPE}
	${MAIN_INCLUDE_HEADERS}
	${CORE_CPP_HEADERS}
	${CORE_CPP_SOURCES}
	${OPENGL_CPP_HEADERS}
	${OPENGL_CPP_SOURCES}
	${VULKAN_CPP_HEADERS}
	${VULKAN_CPP_SOURCES}
	${VULKAN_WINDOWS_CPP_HEADERS}
	${VULKAN_WINDOWS_CPP_SOURCES}
	${SCENE_CPP_HEADERS}
	${SCENE_CPP_SOURCES}
	${SDL_CPP_HEADERS}
	${SDL_CPP_SOURCES}
	${PLATFORM_CPP_HEADERS}
	${PLATFORM_CPP_SOURCES}

	${THIRD_PARTY_DIR}/ktx/lib/texture.c
	${THIRD_PARTY_DIR}/ktx/lib/hashlist.c
	${THIRD_PARTY_DIR}/ktx/lib/checkheader.c
	${THIRD_PARTY_DIR}/ktx/lib/swap.c
	${THIRD_PARTY_DIR}/ktx/lib/memstream.c
	${THIRD_PARTY_DIR}/ktx/lib/filestream.c
)

set_property(TARGET questart PROPERTY CXX_STANDARD 17)
set_property(TARGET questart PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(
	questart
	opengl32
	${LIB_GLEW}
	${LIB_SDL2}
	${LIB_SDL2_MAIN}
	${LIB_SDL2_IMAGE}
	${LIB_VULKAN}
)

add_custom_command(
    TARGET questart
    POST_BUILD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND PowerShell -File cmake-post-build.ps1
)