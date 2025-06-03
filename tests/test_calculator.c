#include <stdio.h>
#include <assert.h>
#include "../src/calculator.h"

void test_add() {
    assert(add(2, 3) == 5);
    assert(add(-1, 1) == 0);
    assert(add(0, 0) == 0);
    printf("✓ Tests addition réussis\n");
}

void test_subtract() {
    assert(subtract(5, 3) == 2);
    assert(subtract(0, 5) == -5);
    assert(subtract(10, 10) == 0);
    printf("✓ Tests soustraction réussis\n");
}

void test_multiply() {
    assert(multiply(3, 4) == 12);
    assert(multiply(-2, 3) == -6);
    assert(multiply(0, 5) == 0);
    printf("✓ Tests multiplication réussis\n");
}

void test_divide() {
    assert(divide(10, 2) == 5);
    assert(divide(7, 3) == 2); // Division entière
    assert(divide(5, 0) == 0); // Gestion division par zéro
    printf("✓ Tests division réussis\n");
}

int main() {
    printf("=== Exécution des tests unitaires ===\n");
    
    test_add();
    test_subtract();
    test_multiply();
    test_divide();
    
    printf("\n🎉 Tous les tests sont passés avec succès!\n");
    return 0;
}