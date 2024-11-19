#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void nano_wait(unsigned int n)
{
    asm("        mov r0,%0\n"
        "repeat: sub r0,#83\n"
        "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}



char* float_to_string(float input) {
    char *output = (char*)malloc(20 * sizeof(char));  // Allocate memory for the string
    if (output != NULL) {
        sprintf(output, "%.2f", input);  // Converts float to string with 2 decimal places
    }
    return output;
}


