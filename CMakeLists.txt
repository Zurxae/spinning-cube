cmake_minimum_required(VERSION 3.20)
project(mygame LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add SDL3 (vendored)
add_subdirectory(vendored/SDL)

add_executable(mygame
  main.c
)

target_link_libraries(mygame PRIVATE SDL3::SDL3)

# macOS bundle (important)
set_target_properties(mygame PROPERTIES
  MACOSX_BUNDLE TRUE
)

