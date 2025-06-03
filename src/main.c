#include <stdio.h>
#include "calculator.h"

int main() {
    printf("=== Calculatrice Simple ===\n");
    
    int a = 10, b = 5;
    
    printf("Addition: %d + %d = %d\n", a, b, add(a, b));
    printf("Soustraction: %d - %d = %d\n", a, b, subtract(a, b));
    printf("Multiplication: %d * %d = %d\n", a, b, multiply(a, b));
    
    if (b != 0) {
        printf("Division: %d / %d = %d\n", a, b, divide(a, b));
    }
    
    return 0;
}