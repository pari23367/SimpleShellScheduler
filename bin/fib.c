#include <stdio.h>
#include "../src/dummy_main.h"
// Recursive function to calculate Fibonacci numbers without dynamic programming
int fibonacci(int n) {
    if (n <= 1)
        return n;
    return fibonacci(n - 1) + fibonacci(n - 2);  // Calculate each term from scratch
}

int main() {
    int n = 30;  // Number of terms to calculate
    printf("Fibonacci Sequence (first 30 terms):\n");

    // Loop through and print each Fibonacci number up to the 30th term
    for (int i = 0; i < n; i++) {
        printf("%d ", fibonacci(i));
    }
    printf("\n");

    return 0;
}

