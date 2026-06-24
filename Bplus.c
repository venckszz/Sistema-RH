#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Bplus.h"

void* mallocSafe(size_t nbytes) {
    void* ptr = malloc(nbytes);
    
    if (ptr == NULL) {
        printf("Erro na alocacao de memoria! Encerrando o programa...\n");
        exit(1);
    }
    return ptr;
}


No* alocaNo(int ordem, int tamanhoRegistro) {
    
}