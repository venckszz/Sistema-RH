#include <stdlib.h>
#include <stdbool.h>

#define ORDEM 5

typedef struct pagina {
    bool pagina_ativa;

    // Não impacta página ativa, apenas as inativas, logo, quando ativa, valor = -1
    int pos_prox_livre;

    int posPagina;
    int qtd_chaves_atuais;
    bool ehFolha;
   
    // Vetor de posicao para os filhos, só se usa em interno
    int posFilhos[ORDEM + 1];
   
    // Caso seja pagina interna, posProximo = -1
    int posProximo;

    // Vetor para os indices de cada chave, só se usa em folha
    int posRegistro[ORDEM];

     // Vetor para os dados genericos
    void* chaves[ORDEM]; 
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


void abreBplus(FILE* arquivo, size_t tamanho_dado);

void fechaBplus(FILE* arquivo, Bplus* cabecalho);

Bplus* leituraCabecalhoBplus(FILE* arquivo);

void escreveCabecalhoBplus(FILE* arquivo, Bplus* cabecalho);

int ProxPagina(FILE* arquivo, Bplus* cabecalho, void (*leituraDado)(void*, FILE*));

Pagina* criaPagina(size_t tamanho_dado, bool ehFolha);

void liberaPaginaRAM(Pagina *p);

void liberaLogicamentePagina(FILE* arquivo, Bplus* cabecalho, Pagina* pagina);

void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*));

Pagina* leituraPagina(FILE* arquivo, Bplus* cabecalho, int posPagina, void (*leituraDado)(void*, FILE*));

Pagina* buscaDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoBuscado, int* indiceBusca, bool* encontrou, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*));

int insereBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, int posRegistro, bool (*ehMenor)(void*, void*), void* (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*));

void insereNaFolha(Pagina* pagina, void* novo_dado, int indiceDado, bool (*ehMenor)(void*, void*));

Pagina* splitFolha(Pagina* pagina, Bplus* cabecalho);

void insereNoPai(Pagina* pai, void* novo_dado, int indiceFilhoAntigo, int posNovoFilho);

Pagina* splitInterno(Pagina* pagina, Bplus* cabecalho);

bool removeDadoBplus(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, void* dadoRemover, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*));

void concatenaBplus(FILE* arquivo, Bplus* cabecalho, Pagina* pai, int indiceFilhoEsq, Pagina* irmaoEsq, Pagina* irmaoDir, void (*escreveDado)(void*, FILE*));

bool redistribui(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, int posPai, int indiceFilho, void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*));

void imprimeArvoreBplus(FILE* arquivo, Bplus* cabecalho, int posAtual, int profundidade, void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*));

void buscaIntervaloBplus(FILE* arquivo, Bplus* cabecalho, void* limite_inferior, void* limite_superior, bool (*ehMenor)(void*, void*), void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*));