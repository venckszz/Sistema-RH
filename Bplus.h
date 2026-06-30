#include <stdlib.h>
#include <stdbool.h>

#define ORDEM 5

typedef struct pagina {
    int pai;
    bool pagina_ativa;

    // Se pagina ativa, pos_prox_livre = -1;:
    int pos_prox_livre;

    int posPagina;
    int qtd_chaves_atuais;
    bool ehFolha;

    // Vetor para os dados genericos
    void* chaves[ORDEM - 1];
   
    // Vetor de posicao para os filhos
    int posFilhos[ORDEM];
   
    // Caso seja pagina interna, posProximo = -1
    int posProximo;
} Pagina;

typedef struct Bplus {
    int raiz;
    int ordem;

    // Tamanho em bytes do dado generico sendo armazenado
    size_t tamanho_registro;
    int qtd_paginas;

    // Posicao para reaproveitamento/crescimento do arquivo
    int prox_pos_livre;
} Bplus;


Bplus* criaCabecalhoBplus(FILE* arquivo, size_t tamanho_dado);