// Pruebas unitarias minimalistas del módulo inventory.
// Los casos de prueba están elegidos deliberadamente para que, cuando se
// introduce cada defecto controlado (ver docs/DEFECTOS.md), el fallo
// se manifieste al ejecutar ESTA MISMA suite bajo sanitizers.

#include <cassert>
#include <iostream>
#include "../src/inventory.h"

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(cond)                                                        \
    do {                                                                   \
        tests_run++;                                                       \
        if (!(cond)) {                                                     \
            tests_failed++;                                                \
            std::cerr << "FALLO: " << #cond << " (linea " << __LINE__      \
                      << ")" << std::endl;                                 \
        }                                                                  \
    } while (0)

int main() {
    // (1) find_last_below - también ejercita el límite superior del arreglo,
    // por lo que un off-by-one en la versión defectuosa dispara ASan.
    int prices[] = {10, 20, 5, 30, 15};
    CHECK(find_last_below(prices, 5, 12) == 2);
    CHECK(find_last_below(prices, 5, 1) == -1);

    // (2) average_price
    int stock[] = {100, 200, 300};
    CHECK(average_price(stock, 3) == 200.0);
    CHECK(average_price(stock, 0) == 0.0);

    // (3) total_stock_value - sin fuga en la version correcta;
    // LeakSanitizer debe reportar 0 fugas al terminar el binario.
    int p[] = {10, 20, 30};
    int q[] = {1, 2, 3};
    CHECK(total_stock_value(p, q, 3) == 10 + 40 + 90);

    // (4) apply_growth - periodos suficientes para desbordar un int de 32
    // bits en la version defectuosa (1000 * 1.5^40 excede INT_MAX).
    long long crecimiento = apply_growth(1000, 50, 40);
    CHECK(crecimiento > 0);

    // (5) checksum32 - valor mayor que INT_MAX para exponer una conversion
    // peligrosa si se elimina el modulo explicito.
    int cs = checksum32(9876543210LL);
    CHECK(cs == static_cast<int>(9876543210LL % 2147483647LL));

    // (6) evaluate_shipping_cost - casos funcionales básicos; la duplicación
    // o complejidad excesiva la detecta el análisis estático, no este test.
    CHECK(evaluate_shipping_cost(2, 100, false, false) == 14);
    CHECK(evaluate_shipping_cost(2, 100, true, true) == 14 + 500 + 150);

    std::cout << tests_run - tests_failed << "/" << tests_run
              << " pruebas exitosas" << std::endl;

    return tests_failed == 0 ? 0 : 1;
}
