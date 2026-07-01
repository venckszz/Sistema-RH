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

size_t calculaDeslocamentoPagina(int posPagina, size_t tamanho_registro) {
                                                                        //    posFilhos            posRegistro
    size_t tamanho_fixo_dados = (sizeof(bool) * 2) + (sizeof(int) * 4) + (sizeof(int) * ORDEM) + (sizeof(int) * ORDEM);
    
    //Tamanho de acordo com os dados genéricos
    size_t tamanho_chaves = (ORDEM - 1) * tamanho_registro;
    
    size_t tamanho_pagina_disco = tamanho_fixo_dados + tamanho_chaves;
    
    // Considera o tamanho do cabeçalho no inicio do arquivo
    return sizeof(Bplus) + (posPagina * tamanho_pagina_disco);
}

void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}