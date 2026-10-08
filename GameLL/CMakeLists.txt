cmake_minimum_required(VERSION 3.20)
project(GameLL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_ROOT "C:/SFML-2.6.1" CACHE PATH "Path to extracted SFML")

include_directories("${SFML_ROOT}/include")
link_directories("${SFML_ROOT}/lib")

file(GLOB_RECURSE GAMELL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(GameLL ${GAMELL_SOURCES}
        src/CraftManager.cpp
        src/CraftManager.h
        src/C_Inventory.cpp
        src/C_Inventory.h
        src/C_Inventory.h)

target_include_directories(GameLL PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dev"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Materials"
)

# Matches GameLL.vcxproj exactly: static SFML, same define, same extra
# dependency libs the static SFML libs need pulled in directly.
target_compile_definitions(GameLL PRIVATE SFML_STATIC)
target_compile_options(GameLL PRIVATE /showIncludes)

set(SFML_MODULES graphics window system audio network)
foreach(mod ${SFML_MODULES})
    target_link_libraries(GameLL PRIVATE
        debug sfml-${mod}-s-d
        optimized sfml-${mod}-s
    )
endforeach()

if(WIN32)
    target_link_libraries(GameLL PRIVATE
        opengl32 freetype winmm gdi32 openal32
        flac vorbisenc vorbisfile vorbis ogg
        ws2_32 shlwapi
    )
    # Matches the .vcxproj's Windows-subsystem-with-console-main trick.
    target_link_options(GameLL PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()

# Matches Visual Studio's old default $(ProjectDir) working directory,
# so Utils::GetWorkingDirectory() + relative paths (Assets/..., nav/..., src/Materials/...)
# keep resolving the same way they did under VS.
set_target_properties(GameLL PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
