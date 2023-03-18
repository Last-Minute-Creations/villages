cmake_minimum_required(VERSION 2.8.12)
project(villages LANGUAGES C)

if(NOT AMIGA)
	message(SEND_ERROR "This project only compiles for Amiga")
endif()

# ACE
add_subdirectory(deps/ace ace)

set(CMAKE_C_STANDARD 11)
file(GLOB_RECURSE SOURCES src/*.c src/*.h)

if(ELF2HUNK)
	set(VILLAGES_EXECUTABLE villages.elf)
	add_executable(${VILLAGES_EXECUTABLE} ${SOURCES})
	target_link_libraries(${VILLAGES_EXECUTABLE} -Wl,-Map=villages.map)
	add_custom_command(
		TARGET ${VILLAGES_EXECUTABLE} POST_BUILD
		COMMAND ${ELF2HUNK} ${VILLAGES_EXECUTABLE} villages.exe
	)
	add_custom_command(
		TARGET ${VILLAGES_EXECUTABLE} POST_BUILD
		COMMAND ${OBJDUMP} --disassemble -S ${VILLAGES_EXECUTABLE} > villages.s
	)
else()
	SET(VILLAGES_EXECUTABLE villages)
	add_executable(${VILLAGES_EXECUTABLE} ${SOURCES})
endif()

target_include_directories(${VILLAGES_EXECUTABLE} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_options(${VILLAGES_EXECUTABLE} PUBLIC -Wall)
target_link_libraries(${VILLAGES_EXECUTABLE} ace)
if(GAME_DEBUG)
	target_compile_definitions(${VILLAGES_EXECUTABLE} PRIVATE GAME_DEBUG)
endif()
if(ACE_DEBUG)
	target_compile_definitions(ace PUBLIC ACE_DEBUG ACE_DEBUG_UAE)
endif()

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/res)
if(GAME_DEBUG)
	set(BITMAP_DIR ${CMAKE_CURRENT_LIST_DIR}/res/bitmaps/debug)
else()
	set(BITMAP_DIR ${CMAKE_CURRENT_LIST_DIR}/res/bitmaps/release)
endif()
set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data)

# Directory preparation
file(
	MAKE_DIRECTORY
	${DATA_DIR}
	${DATA_DIR}/palettes
	${DATA_DIR}/cursors
	${DATA_DIR}/fonts
	${DATA_DIR}/bitmaps
)

file(GLOB RES_FILES ${RES_DIR}/*)

# FIXME: This is only to early development
file(COPY ${RES_DIR}/fonts/arpegius-15.fnt DESTINATION ${DATA_DIR}/fonts)

# Palette
set(PLT_PATH ${DATA_DIR}/palettes/global.plt)
set(TRANSPARENCY_HEX "#880088")
convertPalette(${VILLAGES_EXECUTABLE} ${RES_DIR}/palettes/global.gpl ${PLT_PATH})

# Convert to .bm
convertBitmaps(
	TARGET ${VILLAGES_EXECUTABLE}
	PALETTE ${RES_DIR}/palettes/cursor.gpl
	INTERLEAVED SOURCES ${RES_DIR}/cursors/hand.png
	DESTINATIONS ${DATA_DIR}/cursors/hand.bm
)

convertBitmaps(
	TARGET ${VILLAGES_EXECUTABLE} PALETTE ${PLT_PATH} MASK_COLOR ${TRANSPARENCY_HEX}
	SOURCES
		${BITMAP_DIR}/splash.png
		${BITMAP_DIR}/scrolls.png
	DESTINATIONS
		${DATA_DIR}/bitmaps/splash.bm
		${DATA_DIR}/bitmaps/scrolls.bm
	MASKS
		${DATA_DIR}/bitmaps/splash_mask.bm
		${DATA_DIR}/bitmaps/scrolls_mask.bm
)
