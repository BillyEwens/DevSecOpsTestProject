# Catálogo de defectos controlados

Este documento describe los 6 defectos del laboratorio: qué función afectan,
qué cambio exacto los introduce, y qué etapa del pipeline debe detectarlos.
Cada defecto vive en su propia rama git (`defecto/<n>-<nombre>`), lista para
abrir un pull request de demostración.

> Los defectos se aplican **solo** en `src/inventory.cpp`. La firma de las
> funciones (`src/inventory.h`) nunca cambia.

---

## 1. Acceso fuera de los límites de un arreglo
**Rama:** `defecto/1-fuera-de-limites`
**Función:** `find_last_below`

```diff
- for (int i = 0; i < count; i++) {
+ for (int i = 0; i <= count; i++) {   // <-- off-by-one
```

- **Detectado en:** etapa 5, *Sanitizers*.
- **Herramienta:** AddressSanitizer.
- **Señal esperada:** `AddressSanitizer: stack-buffer-overflow ... in find_last_below`.

---

## 2. Uso de una variable no inicializada
**Rama:** `defecto/2-variable-no-inicializada`
**Función:** `average_price`

```diff
- if (count <= 0) {
-     return 0.0;
- }
- double total = 0.0;
+ double total;   // <-- nunca se inicializa
  for (int i = 0; i < count; i++) {
      total += prices[i];
  }
```

- **Detectado en:** etapa 2, *Calidad: Warnings-as-Errors*.
- **Herramienta:** `-Wmaybe-uninitialized` (GCC/Clang).
- **Señal esperada:** `error: 'total' may be used uninitialized [-Werror=maybe-uninitialized]`.
- Nota: requiere compilar con optimización (`-O1` o superior) para que el
  compilador pueda hacer el análisis de flujo de datos; el `CMakeLists.txt`
  ya lo activa cuando `WARNINGS_AS_ERRORS=ON`.

---

## 3. Pérdida de memoria
**Rama:** `defecto/3-perdida-de-memoria`
**Función:** `total_stock_value`

```diff
  }
- delete[] subtotal;
  return total;
```

- **Detectado en:** etapa 5, *Sanitizers*.
- **Herramienta:** LeakSanitizer (incluido con AddressSanitizer en Linux).
- **Señal esperada:** `LeakSanitizer: detected memory leaks ... leaked in 1 allocation(s)`.

---

## 4. Operación con comportamiento indefinido
**Rama:** `defecto/4-comportamiento-indefinido`
**Función:** `apply_growth`

```diff
- long long value = base;
+ int value = base;   // <-- se pierde el ancho necesario, provoca overflow
```

- **Detectado en:** etapa 5, *Sanitizers*.
- **Herramienta:** UndefinedBehaviorSanitizer.
- **Señal esperada:** `runtime error: signed integer overflow: ... cannot be represented in type 'int'`.

---

## 5. Conversión numérica peligrosa
**Rama:** `defecto/5-conversion-peligrosa`
**Función:** `checksum32`

```diff
  int checksum32(long long value) {
-     const long long modulo = 2147483647LL;
-     return static_cast<int>(value % modulo);
+     return value;   // <-- conversion implicita, trunca los bits altos
  }
```

- **Detectado en:** dos etapas independientes (defensa en profundidad):
  1. Etapa 2 (*Calidad*): `-Wconversion` → `error: conversion from 'long long int' to 'int' may change value`.
  2. Etapa 4 (*Pruebas unitarias*): el test de `checksum32` con un valor
     mayor a `INT_MAX` falla porque el resultado ya no coincide con el
     valor esperado.

---

## 6. Código duplicado o excesivamente complejo
**Rama:** `defecto/6-codigo-duplicado`
**Función:** `evaluate_shipping_cost` (se agrega una función nueva)

```diff
+ int evaluate_shipping_cost_premium(int weight, int distance, bool express, bool fragile) {
+     int cost = weight * 2 + distance / 10;
+     if (express) { cost += 500; }
+     if (fragile) { cost += 150; }
+     cost = cost - (cost / 10);
+     return cost;
+ }
```

- **Detectado en:** etapa 3, *Análisis estático*.
- **Herramienta:** `jscpd` (detector de clones de código), umbral configurado en 0%.
- **Señal esperada:** `ERROR: jscpd found too many duplicates (X%) over threshold (0.0%)`.

---

## Cómo usar este catálogo para la demostración (paso 7-9 de la guía)

1. `git checkout defecto/<n>-<nombre>`
2. `git push origin defecto/<n>-<nombre>` y abrir un pull request contra `main`.
3. Observar en la pestaña **Actions** cuál job del pipeline falla y copiar
   el mensaje de diagnóstico exacto (son los que se listan arriba).
4. Corregir el defecto en `src/inventory.cpp` (revertir el cambio, o
   aplicar una solución más robusta si aplica).
5. **Agregar una prueba de regresión** en `tests/test_inventory.cpp` que
   habría fallado con el defecto presente, para que no pueda reintroducirse
   sin que el pipeline lo note. Por ejemplo, para el defecto 1, un caso que
   llame `find_last_below` con `threshold` mayor que todos los precios,
   ejercitando explícitamente el último índice válido.
6. Hacer commit del fix + la prueba, empujar la rama y confirmar que
   **todas** las etapas del pipeline (incluida la 8, *puerta de
   integración*) quedan en verde antes de fusionar.
