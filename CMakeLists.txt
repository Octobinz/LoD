cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

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

# Game Name Customization
set(PLAYDATE_GAME_NAME LOD_playdate)
set(PLAYDATE_GAME_DEVICE LOD_playdate)
project(${PLAYDATE_GAME_DEVICE} C CXX ASM)

file(GLOB game_SRC
     "src/*.cpp",
	"src/*.h",
)

file(GLOB IMAGES
	"Source/textures/*"
)


# add_definitions(/D_CRT_SECURE_NO_WARNINGS /DBITS64)
# add_compile_options(-fno-use-cxa-atexit)


if (TOOLCHAIN STREQUAL "armgcc")
	message("Building for device")
	
	file(GLOB game_ASM
     "src/*.s",
	)

	set(CMAKE_CONFIGURATION_TYPES "Release")
	add_compile_options(-Wdouble-promotion)
	add_compile_options(-Ofast)

	add_subdirectory(pdcpp)
	add_playdate_application(${PLAYDATE_GAME_DEVICE})
	target_sources(${PLAYDATE_GAME_DEVICE} PUBLIC ${game_SRC} ${game_ASM})
	target_link_libraries(${PLAYDATE_GAME_DEVICE} pdcpp_core)
else()
	message("Building for Windows")
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

	add_definitions(/D_CRT_SECURE_NO_WARNINGS /DBITS64)
	add_compile_options(-fno-use-cxa-atexit)
	set(CMAKE_GENERATOR_PLATFORM x64)
	add_library(${PLAYDATE_GAME_NAME} SHARED ${game_SRC} ${game_ASM} ${IMAGES})
	include(${SDK}/C_API/buildsupport/playdate_game.cmake)
endif()


