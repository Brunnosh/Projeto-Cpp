# Definindo o m�nimo requerido do CMake
cmake_minimum_required(VERSION 3.10)

# Definindo o nome do projeto
project(VoxelGame)

# Definindo a vers�o do C++
set(CMAKE_CXX_STANDARD 17)

# Definindo o diret�rio de inclus�o
include_directories(${PROJECT_SOURCE_DIR}/include/stb)
include_directories(${PROJECT_SOURCE_DIR}/include/dev)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Definindo os diret�rios de bibliotecas
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Definindo os arquivos de c�digo-fonte
set(SOURCE_FILES 
    src/main.cpp
    src/game.cpp
    src/block.cpp
    src/chunk.cpp
    src/shader.cpp
    src/window.cpp
    src/glad.c
    src/camera.cpp
    src/world.cpp
)

# Criando o execut�vel do projeto
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Adicionando as depend�ncias (GLFW, GLAD)
target_link_libraries(${PROJECT_NAME} glfw3)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:VoxelGame>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:VoxelGame>/assets
)