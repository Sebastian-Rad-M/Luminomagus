cmake_minimum_required(VERSION 3.26)


set(MAIN_PROJECT_NAME "Luminomagus")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")

project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)
include(FetchContent)

#####-RAYLIB-####################################################################
FetchContent_Declare(
				raylib
				GIT_REPOSITORY https://github.com/raysan5/raylib.git
				GIT_TAG        master
				GIT_SHALLOW    1
				GIT_PROGRESS   ON
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
				freetype
				GIT_REPOSITORY https://github.com/freetype/freetype.git
				GIT_TAG        VER-2-13-2
				GIT_SHALLOW    1
				GIT_PROGRESS   ON
)
set(FT_DISABLE_ZLIB ON CACHE BOOL "Disable Zlib" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "Disable BZip2" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "Disable PNG" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable Harfbuzz" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "Disable Brotli" FORCE)

FetchContent_MakeAvailable(freetype)
if(NOT TARGET Freetype::Freetype)
	add_library(Freetype::Freetype ALIAS freetype)
endif()


#####-RMLUI THE GOAT-############################################################
FetchContent_Declare(
				rmlui
				GIT_REPOSITORY https://github.com/mikke89/RmlUi.git
				GIT_TAG        master
				GIT_SHALLOW    1
				GIT_PROGRESS   ON
)
FetchContent_MakeAvailable(rmlui)


###############################################################################

add_executable(${MAIN_EXECUTABLE_NAME}
				main.cpp
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

target_sources(${MAIN_EXECUTABLE_NAME}
				PRIVATE
								${SRC_FILES}
)

set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME})

###############################################################################

# use SYSTEM so cppcheck and clang-tidy do not report warnings from these directories
target_include_directories(${MAIN_EXECUTABLE_NAME} PUBLIC
				$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
				$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/backend>
				$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/ui>
				$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/RaylibHomeMadeLibraries>
)

target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE
				ext
)

target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE
				raylib
				RmlUi::Core
				RmlUi::Debugger
)

###############################################################################

# copy binaries to "bin" folder; these are uploaded as artifacts on each release
# DESTINATION_DIR is set as "bin" in cmake/Options.cmake:6
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
	install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
