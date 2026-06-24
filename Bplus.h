#include <stdlib.h>
#include <stdbool.h>

typedef struct no {
   int posicaoNo;
   int qtd_nos_atuais;
   bool ehFolha; 
   int posProximo;

   // Vetor para os dados genéricos
   void* chaves;
   
   // Vetor de posição para os filhos
   int* posFilhos;
} No;

typedef struct Bplus {
    
    // Posicao da raiz
    int raiz;
    int ordem;

    // Tamanho em bytes do dado genérico sendo armazenado
    int tamanhoRegistro;

    // Posiçãoo para reaproveitamento/crescimento do arquivo
    int proxima_posicao_livre;
    
} Bplus;

void* mallocSafe(size_t nbytes)

No* alocaNo(int ordem, int tamanhoRegistro);

void liberaNo(No* no);

void leNoArquivo(FILE* arquivo, No* no, int posicaoNo, int ordem, int tamanhoRegistro);

void escreveNoArquivo(FILE* arquivo, No* no, int ordem, int tamanhoRegistro);

int obtemPosicaoLivreArquivo(FILE* arquivo, Bplus* cabecalho);


Bplus abreArvore(FILE* arquivo, int ordem, int tamanhoRegistro);

void fechaArvore(FILE* arquivo, Bplus* cabecalho);

void insereRegistro(FILE* arquivo, Bplus* cabecalho, const void* dado, int (*compara)(const void*, const void*));

bool buscaRegistro(FILE* arquivo, Bplus* cabecalho, const void* chaveBusca, void* dadoEncontrado, int (*compara)(const void*, const void*));


void divideNocheio(FILE* arquivo, Bplus* cabecalho, No* noPai, int indiceFilho, No* noFilho);

void insereNo(FILE* arquivo, Bplus* cabecalho, No* no_atual, const void* dado, int (*compara)(const void*, const void*));


bool removeRegistro(FILE* arquivo, Bplus* cabecalho, const void* chaveBusca, int (*compara)(const void*, const void*));

void balanciarNo(FILE* arquivo, Bplus* cabecalho, No* noPai, int indiceFilho);

void redistribuiNo(FILE* arquivo, Bplus* cabecalho, No* noPai, No* noFilho, No* noIrmao, int indiceFilho, bool irmaoEsq);

void mesclaNo(FILE* arquivo, Bplus* cabecalho, No* noPai, No* noFilho, No* noIrmao, int indiceFilho, bool irmaoEsq);


void listaRegistroIntervalo(FILE* arquivo, Bplus* cabecalho, const void* chaveInicio, const void* chaveFim, int (*compara)(const void*, const void*), void (*imprime)(const void*));

void imprimeArvore(FILE* arquivo, Bplus* cabecalho, void (*imprimeChave)(const void*));