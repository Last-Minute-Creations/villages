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

file(
	GLOB
	RES_FILES
	${RES_DIR}/*
)

file(
	COPY
	${RES_FILES}
	DESTINATION
	${DATA_DIR}
)
