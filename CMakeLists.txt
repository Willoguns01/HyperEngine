cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

# weird spdlog workaround to stop a build error
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

project(Engine)
project(Editor)
project(Runtime)

find_package(daxa CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Jolt CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(enkiTS CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# sources and includes
# engine
file(GLOB_RECURSE ENGINE_PUBLIC Engine/public/**.cpp Engine/public/**.hpp)
file(GLOB_RECURSE ENGINE_PRIVATE Engine/private/**.cpp Engine/private/**.hpp)
# editor
file(GLOB_RECURSE EDITOR_PUBLIC Editor/public/**.cpp Editor/public/**.hpp)
file(GLOB_RECURSE EDITOR_PRIVATE Editor/private/**.cpp Editor/private/**.hpp)
# runtime
file(GLOB_RECURSE RUNTIME_PUBLIC Runtime/public/**.cpp Runtime/public/**.hpp)
file(GLOB_RECURSE RUNTIME_PRIVATE Runtime/private/**.cpp Runtime/private/**.hpp)

# engine
add_library(Engine ${ENGINE_PUBLIC} ${ENGINE_PRIVATE})
target_include_directories(Engine PUBLIC Engine/public/)
target_include_directories(Engine PRIVATE Engine/private/ Engine/public/)
target_link_libraries(Engine PUBLIC 
    # rendering / windowing
    glfw
    daxa::daxa
    # engine
    spdlog::spdlog 
    Jolt::Jolt 
    EnTT::EnTT 
    glm::glm 
    enkiTS::enkiTS 
    # editor
    imgui::imgui 
)
target_precompile_headers(Engine PUBLIC Engine/public/Engine/pch.hpp)

# editor
add_executable(Editor ${EDITOR_PUBLIC} ${EDITOR_PRIVATE})
target_include_directories(Editor PUBLIC Editor/public/)
target_include_directories(Editor PRIVATE Editor/private/ Editor/public/)
target_link_libraries(Editor PRIVATE Engine)
target_precompile_headers(Editor REUSE_FROM Engine)

# runtime
add_executable(Runtime ${RUNTIME_PUBLIC} ${RUNTIME_PRIVATE})
target_include_directories(Runtime PUBLIC Runtime/public/)
target_include_directories(Runtime PRIVATE Runtime/private/ Runtime/public/)
target_link_libraries(Runtime PRIVATE Engine)
target_precompile_headers(Runtime REUSE_FROM Engine)
