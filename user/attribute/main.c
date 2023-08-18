#include <stdio.h>

__attribute__((section("my_custom_section"))) void custom_function() {
    printf("Hello from custom section!\n");
}

int main() {
    custom_function();
    return 0;
}

