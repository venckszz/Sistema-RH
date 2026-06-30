#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bplus.h"

void* mallocSafe(size_t nbytes) {
    void* ptr = malloc(nbytes);
    if (ptr == NULL) {
        printf("Erro na alocacao de memoria! Encerrando o programa...\n");
        exit(1);
    }
    return ptr;
}

void verificaArquivo(FILE* arquivo) {
    if (arquivo == NULL) {
        printf("ERRO! Arquivo vazio\n");
        exit(1);
    }

    return;
}

size_t calculaPosicaoPagina(int posPagina, size_t tamanho_registro) {
    return sizeof(Bplus) + sizeof(Pagina) + ((ORDEM - 1) * tamanho_registro);
}