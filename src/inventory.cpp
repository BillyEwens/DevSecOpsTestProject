#include "inventory.h"

int find_last_below(const int prices[], int count, int threshold) {
    int found = -1;
    for (int i = 0; i < count; i++) {
        if (prices[i] < threshold) {
            found = i;
        }
    }
    return found;
}

double average_price(const int prices[], int count) {
    if (count <= 0) {
        return 0.0;
    }
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        total += prices[i];
    }
    return total / count;
}

long total_stock_value(const int prices[], const int quantities[], int count) {
    int* subtotal = new int[count];
    long total = 0;
    for (int i = 0; i < count; i++) {
        subtotal[i] = prices[i] * quantities[i];
        total += subtotal[i];
    }
    delete[] subtotal;
    return total;
}

long long apply_growth(int base, int rate_percent, int periods) {
    long long value = base;
    for (int i = 0; i < periods; i++) {
        value = value * (100 + rate_percent) / 100;
    }
    return value;
}

int checksum32(long long value) {
    return value; // conversion implicita peligrosa
}

int evaluate_shipping_cost(int weight, int distance, bool express, bool fragile) {
    int cost = weight * 2 + distance / 10;
    if (express) {
        cost += 500;
    }
    if (fragile) {
        cost += 150;
    }
    return cost;
}
