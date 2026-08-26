# Criterios que bloquean un pull request

Estos son los criterios obligatorios ("quality gates") que debe cumplir
cualquier cambio antes de integrarse a `main`. Cada uno corresponde a un
job del pipeline (`.github/workflows/devsecops-pipeline.yml`).

| # | Criterio | Job responsable | Tipo de fallo |
|---|----------|------------------|----------------|
| 1 | No debe haber secretos/credenciales en el código | `secret-scanning` | Bloqueante |
| 2 | El proyecto debe compilar con GCC y con Clang | `build` | Bloqueante |
| 3 | Cero advertencias del compilador (`-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wuninitialized`, tratadas como error) | `quality-warnings-as-errors` | Bloqueante |
| 4 | Cero hallazgos de `cppcheck` (warning/style/performance/portability) | `static-analysis` | Bloqueante |
| 5 | 0% de código duplicado (`jscpd`, umbral configurado en 0%) | `static-analysis` | Bloqueante |
| 6 | Todas las pruebas unitarias deben pasar | `unit-tests` | Bloqueante |
| 7 | Cero hallazgos de AddressSanitizer, UndefinedBehaviorSanitizer o LeakSanitizer al ejecutar la suite de pruebas | `sanitizers` | Bloqueante |
| 8 | Cobertura de línea ≥ 70% (`COVERAGE_THRESHOLD` en el workflow) | `coverage` | Bloqueante |

El job `integration-gate` (etapa 8 del pipeline) depende de **todos** los
anteriores. Es el único check que se debe marcar como "obligatorio" en la
configuración de **Branch protection rules** de GitHub
(`Settings → Branches → Require status checks to pass before merging`),
porque agrega el resultado de todo lo demás en un solo semáforo.

## Por qué el umbral de cobertura es 70% y no 100%

Un umbral de cobertura demasiado alto (100%) castiga código defensivo o
casos límite poco realistas y puede incentivar pruebas de relleno sin valor
real. 70% es un punto de partida razonable para un proyecto pequeño; en un
proyecto real este número debería acordarse con el equipo y revisarse
periódicamente, no fijarse arbitrariamente para siempre.

## Qué NO bloquea (a propósito)

- Los reportes de cobertura y de `cppcheck` se publican como **artefactos**
  (etapas 6 y 7) para que el equipo los revise, pero no todo hallazgo no
  crítico bloquea el merge; solo lo hace cruzar el umbral definido arriba.
- El job `build` en la matriz GCC/Clang confirma portabilidad de la
  compilación pero no aplica `-Werror`; ese criterio más estricto vive
  únicamente en `quality-warnings-as-errors`, para separar "¿compila?" de
  "¿compila sin advertencias?".
