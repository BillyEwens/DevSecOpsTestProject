#ifndef INVENTORY_H
#define INVENTORY_H

// Cada función de este módulo está diseñada para mapear a UNA categoría
// de defecto del catálogo del laboratorio (ver docs/DEFECTOS.md). Aquí
// viven en su versión CORRECTA; los defectos se introducen en ramas git
// aparte para la demostración del pipeline.

// (1) Fuera de límites: busca el último precio por debajo de 'threshold'.
//     Devuelve el índice o -1 si no existe.
int find_last_below(const int prices[], int count, int threshold);

// (2) Variable no inicializada: promedio de precios.
double average_price(const int prices[], int count);

// (3) Pérdida de memoria: calcula el valor total del inventario usando
//     un buffer temporal en el heap.
long total_stock_value(const int prices[], const int quantities[], int count);

// (4) Comportamiento indefinido (overflow de entero con signo):
//     aplica una tasa de crecimiento compuesta durante 'periods' ciclos.
long long apply_growth(int base, int rate_percent, int periods);

// (5) Conversión numérica peligrosa: reduce un valor de 64 bits a un
//     checksum de 32 bits de forma segura (con módulo explícito).
int checksum32(long long value);

// (6) Código duplicado / excesivamente complejo:
//     calcula el costo de envío según peso, distancia y condiciones.
int evaluate_shipping_cost(int weight, int distance, bool express, bool fragile);

#endif // INVENTORY_H
