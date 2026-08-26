#include <iostream>
#include "inventory.h"

int main() {
    int prices[]      = {1200, 800, 1500, 300, 950};
    int quantities[]  = {3, 10, 2, 25, 5};
    const int count = 5;

    std::cout << "== Laboratorio DevSecOps C++ ==" << std::endl;
    std::cout << "Ultimo indice con precio < 1000: "
              << find_last_below(prices, count, 1000) << std::endl;
    std::cout << "Precio promedio: " << average_price(prices, count) << std::endl;
    std::cout << "Valor total de inventario: "
              << total_stock_value(prices, quantities, count) << std::endl;
    std::cout << "Crecimiento compuesto (base=1000, 5%, 10 periodos): "
              << apply_growth(1000, 5, 10) << std::endl;
    std::cout << "Checksum32 de 9876543210: "
              << checksum32(9876543210LL) << std::endl;
    std::cout << "Costo de envio (2kg, 100km, express, fragil): "
              << evaluate_shipping_cost(2, 100, true, true) << std::endl;

    return 0;
}
