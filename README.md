# Laboratorio DevSecOps en C++: bloqueo de código defectuoso

## Pregunta orientadora

**¿Cómo puede un pipeline impedir que código C++ defectuoso o potencialmente
vulnerable sea integrado a la rama principal?**

Este proyecto responde esa pregunta con un caso práctico: una pequeña
aplicación C++ con 6 categorías de defectos controlados, y un pipeline
CI/CD que los detecta y bloquea automáticamente antes de que lleguen a
`main`.

## Estructura

```
devsecops-cpp-lab/
├── src/
│   ├── main.cpp
│   ├── inventory.h        # 6 funciones, una por categoría de defecto
│   └── inventory.cpp       # version correcta (baseline)
├── tests/
│   └── test_inventory.cpp  # pruebas unitarias que ejercitan las 6 funciones
├── CMakeLists.txt          # opciones: WARNINGS_AS_ERRORS, ENABLE_SANITIZERS, ENABLE_COVERAGE
├── docs/
│   ├── DEFECTOS.md          # catalogo de los 6 defectos, con el diff exacto
│   └── CRITERIOS_BLOQUEO.md # que criterios bloquean un PR y por que
└── .github/workflows/
    └── devsecops-pipeline.yml
```

## Compilar y correr localmente

```bash
# Build normal
cmake -S . -B build
cmake --build build
./build/app
ctest --test-dir build --output-on-failure

# Trabajo de calidad (advertencias como error)
cmake -S . -B build-quality -DWARNINGS_AS_ERRORS=ON
cmake --build build-quality

# Con sanitizers
cmake -S . -B build-san -DENABLE_SANITIZERS=ON
cmake --build build-san
ctest --test-dir build-san --output-on-failure

# Con cobertura
cmake -S . -B build-cov -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov --output-on-failure
gcovr --root . --object-directory build-cov --filter 'src/' -s
```

## Las 8 etapas del pipeline

1. **Secret scanning** (Gitleaks)
2. **Build** con GCC y Clang (matriz)
3. **Calidad**: advertencias tratadas como error (`-Werror`)
4. **Análisis estático**: `cppcheck` + `jscpd` (duplicación)
5. **Pruebas unitarias** (CTest)
6. **Sanitizers**: AddressSanitizer + UndefinedBehaviorSanitizer + LeakSanitizer
7. **Cobertura**: reporte HTML/XML con `gcovr`, umbral mínimo 70%
8. **Puerta de integración**: job que agrega todos los checks obligatorios

Ver el detalle de qué bloquea y por qué en
[`docs/CRITERIOS_BLOQUEO.md`](docs/CRITERIOS_BLOQUEO.md).

## Los 6 defectos controlados

| # | Categoría | Función afectada | Detectado por |
|---|-----------|-------------------|-----------------|
| 1 | Acceso fuera de límites | `find_last_below` | AddressSanitizer |
| 2 | Variable no inicializada | `average_price` | `-Wmaybe-uninitialized` (Werror) |
| 3 | Pérdida de memoria | `total_stock_value` | LeakSanitizer |
| 4 | Comportamiento indefinido | `apply_growth` | UndefinedBehaviorSanitizer |
| 5 | Conversión numérica peligrosa | `checksum32` | `-Wconversion` (Werror) + prueba unitaria |
| 6 | Código duplicado / complejo | `evaluate_shipping_cost` | `jscpd` |

El detalle completo (el diff exacto de cada defecto y el mensaje de
diagnóstico esperado) está en [`docs/DEFECTOS.md`](docs/DEFECTOS.md).

## Cómo hacer la demostración (pasos 7-9 de la guía)

Cada defecto vive en su propia rama, lista para usarse:

```
defecto/1-fuera-de-limites
defecto/2-variable-no-inicializada
defecto/3-perdida-de-memoria
defecto/4-comportamiento-indefinido
defecto/5-conversion-peligrosa
defecto/6-codigo-duplicado
```

Flujo sugerido para la demostración en vivo:

1. Subir el repositorio a GitHub (`git remote add origin ... && git push -u origin main`
   y luego `git push origin defecto/1-fuera-de-limites`, etc.)
2. Abrir un pull request desde una rama `defecto/...` hacia `main`.
3. Mostrar en la pestaña **Actions** cómo el job correspondiente falla, y
   leer el mensaje de diagnóstico exacto (sanitizer, warning o herramienta
   de análisis estático, según el defecto).
4. Corregir el defecto en el propio PR.
5. Agregar una **prueba de regresión** que habría detectado el defecto,
   para que no pueda reintroducirse sin que el pipeline lo note.
6. Empujar el fix, ver el pipeline pasar en verde (las 8 etapas, incluida
   la puerta de integración) y fusionar.

## Configuración adicional recomendada en GitHub

En **Settings → Branches → Branch protection rules** para `main`:
- Require a pull request before merging.
- Require status checks to pass before merging → seleccionar el check
  `8. Puerta de integracion (obligatoria)`.
- (Opcional) Require branches to be up to date before merging.

Esto convierte el "bloqueo lógico" del pipeline en un bloqueo real a nivel
de repositorio: nadie puede hacer merge a `main` si el pipeline no está en
verde.
