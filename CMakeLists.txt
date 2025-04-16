# Definindo o mínimo requerido do CMake
cmake_minimum_required(VERSION 3.10)

# Definindo o nome do projeto
project(VoxelGame)

# Definindo a versão do C++
set(CMAKE_CXX_STANDARD 17)

# Definindo o diretório de inclusão\
include_directories(${PROJECT_SOURCE_DIR}/include/imgui)
include_directories(${PROJECT_SOURCE_DIR}/include/stb)
include_directories(${PROJECT_SOURCE_DIR}/include/dev)
include_directories(${PROJECT_SOURCE_DIR}/include/dev/world)
include_directories(${PROJECT_SOURCE_DIR}/include/dev/render)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Definindo os diretórios de bibliotecas
link_directories(${PROJECT_SOURCE_DIR}/lib)

# Definindo os arquivos de código-fonte
file(GLOB_RECURSE SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.c
)

# Criando o executável do projeto
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Adicionando as dependências (GLFW, GLAD)
target_link_libraries(${PROJECT_NAME} glfw3)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:VoxelGame>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:VoxelGame>/assets
)