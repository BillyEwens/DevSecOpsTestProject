cmake_minimum_required(VERSION 3.16)
project(devsecops_cpp_lab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---------------------------------------------------------------------
# Opciones activables desde el pipeline (cada etapa usa una combinación
# distinta, ver .github/workflows/devsecops-pipeline.yml)
# ---------------------------------------------------------------------
option(WARNINGS_AS_ERRORS "Tratar advertencias del compilador como errores" OFF)
option(ENABLE_SANITIZERS  "Compilar con AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(ENABLE_COVERAGE    "Instrumentar el binario para medir cobertura de pruebas" OFF)

add_library(core src/inventory.cpp)
target_include_directories(core PUBLIC src)

# --- Advertencias estrictas del compilador ---
if(MSVC)
    target_compile_options(core PUBLIC /W4)
else()
    target_compile_options(core PUBLIC
        -Wall -Wextra -Wpedantic -Wshadow
        -Wconversion -Wsign-conversion -Wuninitialized
    )
endif()

if(WARNINGS_AS_ERRORS)
    if(MSVC)
        target_compile_options(core PUBLIC /WX)
    else()
        # -O1 es necesario para que el análisis de flujo de datos del
        # compilador detecte -Wmaybe-uninitialized de forma confiable.
        target_compile_options(core PUBLIC -Werror -O1)
    endif()
endif()

# --- AddressSanitizer + UndefinedBehaviorSanitizer (LeakSanitizer viene
#     incluido automáticamente con ASan en Linux) ---
if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer -g -O0)
    target_link_options(core PUBLIC -fsanitize=address,undefined)
endif()

# --- Instrumentación de cobertura (gcov/lcov) ---
if(ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(core PUBLIC --coverage -O0 -g)
    target_link_options(core PUBLIC --coverage)
endif()

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE core)

enable_testing()
add_executable(test_inventory tests/test_inventory.cpp)
target_link_libraries(test_inventory PRIVATE core)
add_test(NAME test_inventory COMMAND test_inventory)
