cmake_minimum_required(VERSION 2.8.5)
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
	target_compile_definitions(ace PUBLIC ACE_DEBUG ACE_DEBUG_UAE)
endif()

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/res)
set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

# Directory preparation
file(
	MAKE_DIRECTORY
	${DATA_DIR}
	${GEN_DIR}/tileset
)

# Palette
set(PLT_PATH ${DATA_DIR}/mirmirowo.plt)
set(TRANSPARENCY_HEX "#FF00FF")
convertPalette(${VILLAGES_EXECUTABLE} ${RES_DIR}/mirmirowo.gpl ${PLT_PATH})

extractBitmaps(TARGET ${VILLAGES_EXECUTABLE} SOURCE ${RES_DIR}/mirmirowo_tileset.png
	DESTINATIONS
		${GEN_DIR}/tileset/0.png    0   0 32 32
		${GEN_DIR}/tileset/1.png   32   0 32 32
		${GEN_DIR}/tileset/2.png   64   0 32 32
		${GEN_DIR}/tileset/3.png   96   0 32 32

		${GEN_DIR}/tileset/4.png    0  32 32 32
		${GEN_DIR}/tileset/5.png   32  32 32 32
		${GEN_DIR}/tileset/6.png   64  32 32 32
		${GEN_DIR}/tileset/7.png   96  32 32 32

		${GEN_DIR}/tileset/8.png    0  64 32 32
		${GEN_DIR}/tileset/9.png   32  64 32 32
		${GEN_DIR}/tileset/10.png  64  64 32 32
		${GEN_DIR}/tileset/11.png  96  64 32 32

		${GEN_DIR}/tileset/12.png   0  96 32 32
		${GEN_DIR}/tileset/13.png  32  96 32 32
		${GEN_DIR}/tileset/14.png  64  96 32 32
		${GEN_DIR}/tileset/15.png  96  96 32 32

		${GEN_DIR}/tileset/16.png   0 128 32 32
		${GEN_DIR}/tileset/17.png  32 128 32 32
		${GEN_DIR}/tileset/18.png  64 128 32 32
		${GEN_DIR}/tileset/19.png  96 128 32 32

		${GEN_DIR}/tileset/20.png   0 160 32 32
		${GEN_DIR}/tileset/21.png  32 160 32 32
		${GEN_DIR}/tileset/22.png  64 160 32 32
		${GEN_DIR}/tileset/23.png  96 160 32 32

		${GEN_DIR}/tileset/24.png   0 192 32 32
		${GEN_DIR}/tileset/25.png  32 192 32 32
		${GEN_DIR}/tileset/26.png  64 192 32 32
		${GEN_DIR}/tileset/27.png  96 192 32 32

		${GEN_DIR}/tileset/28.png   0 224 32 32
		${GEN_DIR}/tileset/29.png  32 224 32 32
		${GEN_DIR}/tileset/30.png  64 224 32 32
		${GEN_DIR}/tileset/31.png  96 224 32 32

		${GEN_DIR}/tileset/32.png   0 256 32 32
		${GEN_DIR}/tileset/33.png  32 256 32 32
		${GEN_DIR}/tileset/34.png  64 256 32 32
		${GEN_DIR}/tileset/35.png  96 256 32 32

		${GEN_DIR}/tileset/36.png   0 288 32 32
		${GEN_DIR}/tileset/37.png  32 288 32 32
		${GEN_DIR}/tileset/38.png  64 288 32 32
		${GEN_DIR}/tileset/39.png  96 288 32 32

		${GEN_DIR}/tileset/40.png   0 320 32 32
		${GEN_DIR}/tileset/41.png  32 320 32 32
		${GEN_DIR}/tileset/42.png  64 320 32 32
		${GEN_DIR}/tileset/43.png  96 320 32 32

		${GEN_DIR}/tileset/44.png   0 352 32 32
		${GEN_DIR}/tileset/45.png  32 352 32 32
		${GEN_DIR}/tileset/46.png  64 352 32 32
		${GEN_DIR}/tileset/47.png  96 352 32 32

		${GEN_DIR}/tileset/48.png   0 384 32 32
		${GEN_DIR}/tileset/49.png  32 384 32 32
		${GEN_DIR}/tileset/50.png  64 384 32 32
		${GEN_DIR}/tileset/51.png  96 384 32 32

		${GEN_DIR}/tileset/52.png   0 416 32 32
		${GEN_DIR}/tileset/53.png  32 416 32 32
		${GEN_DIR}/tileset/54.png  64 416 32 32
		${GEN_DIR}/tileset/55.png  96 416 32 32

		${GEN_DIR}/tileset/56.png   0 448 32 32
		${GEN_DIR}/tileset/57.png  32 448 32 32
		${GEN_DIR}/tileset/58.png  64 448 32 32
		${GEN_DIR}/tileset/59.png  96 448 32 32

		${GEN_DIR}/tileset/60.png   0 480 32 32
		${GEN_DIR}/tileset/61.png  32 480 32 32
		${GEN_DIR}/tileset/62.png  64 480 32 32
		${GEN_DIR}/tileset/63.png  96 480 32 32

		${GEN_DIR}/tileset/64.png   0 512 32 32
		${GEN_DIR}/tileset/65.png  32 512 32 32
		${GEN_DIR}/tileset/66.png  64 512 32 32
		${GEN_DIR}/tileset/67.png  96 512 32 32

		${GEN_DIR}/tileset/68.png   0 544 32 32
		${GEN_DIR}/tileset/69.png  32 544 32 32
		${GEN_DIR}/tileset/70.png  64 544 32 32
		${GEN_DIR}/tileset/71.png  96 544 32 32

		${GEN_DIR}/tileset/72.png   0 576 32 32
		${GEN_DIR}/tileset/73.png  32 576 32 32
		${GEN_DIR}/tileset/74.png  64 576 32 32
		${GEN_DIR}/tileset/75.png  96 576 32 32

		${GEN_DIR}/tileset/76.png   0 608 32 32
		${GEN_DIR}/tileset/77.png  32 608 32 32
		${GEN_DIR}/tileset/78.png  64 608 32 32
		${GEN_DIR}/tileset/79.png  96 608 32 32

		${GEN_DIR}/tileset/80.png   0 640 32 32
		${GEN_DIR}/tileset/81.png  32 640 32 32
		${GEN_DIR}/tileset/82.png  64 640 32 32
		${GEN_DIR}/tileset/83.png  96 640 32 32

		${GEN_DIR}/tileset/84.png   0 672 32 32
		${GEN_DIR}/tileset/85.png  32 672 32 32
		${GEN_DIR}/tileset/86.png  64 672 32 32
		${GEN_DIR}/tileset/87.png  96 672 32 32

		${GEN_DIR}/tileset/88.png   0 704 32 32
		${GEN_DIR}/tileset/89.png  32 704 32 32
		${GEN_DIR}/tileset/90.png  64 704 32 32
		${GEN_DIR}/tileset/91.png  96 704 32 32
)

convertTileset(
	TARGET ${VILLAGES_EXECUTABLE} SIZE 32
	SOURCE ${GEN_DIR}/tileset DESTINATION ${GEN_DIR}/mirmirowo_tileset.png
	TILES 0.png 1.png 2.png 3.png 4.png 5.png 6.png 7.png 8.png 9.png 10.png 11.png 12.png 13.png 14.png 15.png 16.png 17.png 18.png 19.png 20.png 21.png 22.png 23.png 24.png 25.png 26.png 27.png 28.png 29.png 30.png 31.png 32.png 33.png 34.png 35.png 36.png 37.png 38.png 39.png 40.png 41.png 42.png 43.png 44.png 45.png 46.png 47.png 48.png 49.png 50.png 51.png 52.png 53.png 54.png 55.png 56.png 57.png 58.png 59.png 60.png 61.png 62.png 63.png 64.png 65.png 66.png 67.png 68.png 69.png 70.png 71.png 72.png 73.png 74.png 75.png 76.png 77.png 78.png 79.png 80.png 81.png 82.png 83.png 84.png 85.png 86.png 87.png 88.png 89.png 90.png 91.png
)

convertBitmaps(
	TARGET ${VILLAGES_EXECUTABLE} PALETTE ${PLT_PATH} MASK_COLOR ${TRANSPARENCY_HEX}
	INTERLEAVED SOURCES
		${GEN_DIR}/mirmirowo_tileset.png
	DESTINATIONS
		${DATA_DIR}/mirmirowo_tileset.bm
	MASKS
		${DATA_DIR}/mirmirowo_tileset_mask.bm
)
