cmake_minimum_required(VERSION 3.10)
project(boop)
add_subdirectory(win)
set(CMAKE_CXX_STANDARD 17)

set(executable boop)

set(sources
	src/main.cpp

	src/Texture.hpp
	src/Input.hpp
	src/Colors.hpp

	src/sim/menu/MenuService.hpp
	src/sim/menu/MenuService.cpp
	src/sim/menu/MainMenu.hpp
	src/sim/menu/MainMenu.cpp
	src/sim/menu/DifficultyMenu.hpp
	src/sim/menu/DifficultyMenu.cpp
	src/sim/menu/GameModeMenu.hpp
	src/sim/menu/GameModeMenu.cpp
	src/sim/menu/HostMenu.hpp
	src/sim/menu/HostMenu.cpp
	src/sim/menu/JoinMenu.hpp
	src/sim/menu/JoinMenu.cpp
	src/sim/menu/JoiningMenu.hpp
	src/sim/menu/JoiningMenu.cpp

	src/sim/Simulation.hpp
	src/sim/Simulation.cpp

	src/sim/Game.hpp
	src/sim/Game.cpp
	src/sim/SoundManager.hpp
	src/sim/SoundManager.cpp
	src/sim/Bot.hpp
	src/sim/Bot.cpp
	src/sim/WinState.hpp
	src/sim/GameMode.hpp
	src/sim/Difficulty.hpp
	src/sim/Difficulty.cpp
	src/sim/NetworkMatch.hpp
	src/sim/NetworkMatch.cpp

	src/render/Renderable.hpp
	src/render/Renderer.hpp
	src/render/Renderer.cpp
	src/render/RendererBackend.hpp

	src/render/gl/GLConstants.hpp
	src/render/gl/GLRendererBackend.hpp
	src/render/gl/GLRendererBackend.cpp
	src/render/gl/GLBackgroundRenderer.hpp
	src/render/gl/GLBackgroundRenderer.cpp
	src/render/gl/GLCommonRenderer.hpp
	src/render/gl/GLCommonRenderer.cpp
	src/render/gl/GLPostProcessingRenderer.hpp
	src/render/gl/GLPostProcessingRenderer.cpp
	src/render/gl/GLMenuRenderer.hpp
	src/render/gl/GLMenuRenderer.cpp
)

if (ROLLDATA)
	file(REAL_PATH ${ROLLDATA} path BASE_DIRECTORY ${CMAKE_BINARY_DIR})
	add_compile_definitions(ROLLDATA=\"${path}\")
endif ()

if (WIN32)
	list(APPEND sources icon.rc)
endif ()

if (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
	add_executable(${executable} WIN32 ${sources})
else ()
	add_executable(${executable} ${sources})
endif ()

target_link_libraries(${executable} win)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${executable} PRIVATE -fno-exceptions)
elseif (WIN32)
	# /w3 is too aggressive
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W1" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif ()
endif ()

if (USE_ASAN)
	message("Enabling ASAN")

	target_compile_options(${executable} PRIVATE -fsanitize=address)
	target_link_options(${executable} PRIVATE -fsanitize=address)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${executable} PRIVATE -fsanitize=undefined)
		target_link_options(${executable} PRIVATE -fsanitize=undefined)
	endif ()
endif ()
