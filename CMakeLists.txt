cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})
set(CMAKE_GENERATOR_PLATFORM x64)

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME mansion_playdate)
set(PLAYDATE_GAME_DEVICE mansion_playdate_DEVICE)

project(${PLAYDATE_GAME_NAME} CXX C ASM)

file(GLOB game_SRC
     "src/*.cpp",
	"src/*.h",
)

file(GLOB IMAGES
	"Source/textures/*"
)

add_definitions(/D_CRT_SECURE_NO_WARNINGS /DBITS64)

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE} ${game_SRC})
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED ${game_SRC} ${IMAGES})
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

