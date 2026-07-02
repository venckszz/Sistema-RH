#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Bplus.h"
#include "Util.h"

#define ORDEM 5
#define MAX_CHAVES 20
#define MIN_CHAVES 2

void abreBplus(FILE* arquivo, size_t tamanho_dado) {
    verificaArquivo(arquivo);
    
    Bplus cabecalho;
    cabecalho.ordem = ORDEM;
    cabecalho.prox_pos_livre = -1;
    cabecalho.qtd_paginas = 0;
    cabecalho.raiz = -1;
    cabecalho.tamanho_registro = tamanho_dado;

    rewind(arquivo);
    fwrite(&cabecalho, sizeof(Bplus), 1, arquivo);
}

void fechaBplus(FILE* arquivo, Bplus* cabecalho) {
    verificaArquivo(arquivo);
    escreveCabecalhoBplus(arquivo, cabecalho);
    fclose(arquivo);
}

Bplus* leituraCabecalhoBplus(FILE* arquivo) {
    verificaArquivo(arquivo);
    
    rewind(arquivo);
    
    // Aloca o cabeçalho do arquivo na memória RAM
    Bplus* cabecalho = mallocSafe(sizeof(Bplus));
    if (fread(cabecalho, sizeof(Bplus), 1, arquivo) != 1) {
        free(cabecalho);
        return NULL;
    }
    
    return cabecalho;
}

void escreveCabecalhoBplus(FILE* arquivo, Bplus* cabecalho) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL) return;

    rewind(arquivo);
    fwrite(cabecalho, sizeof(Bplus), 1, arquivo);
}

// Procura qual sera a próxima página carregada na RAM
int ProxPagina(FILE* arquivo, Bplus* cabecalho, void (*leituraDado)(void*, FILE*)) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL) return -1;

    // Retorna a posição livre no meio do arquivo
    if (cabecalho->prox_pos_livre != -1) {
        int posLivre = cabecalho->prox_pos_livre;
        Pagina* paginaLivre = leituraPagina(arquivo, cabecalho, posLivre, leituraDado);
        cabecalho->prox_pos_livre = paginaLivre->pos_prox_livre;
        liberaPaginaRAM(paginaLivre);
        return posLivre;
    }

    // "Cria" mais um espaço no arquivo
    int novaPos = cabecalho->qtd_paginas;
    cabecalho->qtd_paginas++;
    return novaPos;
}

// Cria uma página na memória RAM para manipulação
Pagina* criaPagina(size_t tamanho_dado, bool ehFolha) {
    Pagina* novaPagina = mallocSafe(sizeof(Pagina));
    
    novaPagina->pagina_ativa = true;
    novaPagina->pos_prox_livre = -1;
    novaPagina->posPagina = -1;
    novaPagina->qtd_chaves_atuais = 0;
    novaPagina->ehFolha = ehFolha;
    novaPagina->posProximo = -1;
    
    // Insere como -1 a posição dos filhos
    for (int i = 0; i < ORDEM; i++) {
        novaPagina->posFilhos[i] = -1;
    }
    
    for (int i = 0; i < ORDEM - 1; i++) {
        novaPagina->posRegistro[i] = -1;
    }

    // Aloca espaço para o dado genérico
    for (int i = 0; i < ORDEM - 1; i++) {
        novaPagina->chaves[i] = mallocSafe(tamanho_dado);
    }

    return novaPagina;
}

void liberaPaginaRAM(Pagina *p) {
    if (p == NULL) return;

    for (int i = 0; i < ORDEM - 1; i++) {
        free(p->chaves[i]);
    }

    free(p);
}

void liberaLogicamentePagina(FILE* arquivo, Bplus* cabecalho, Pagina* pagina) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL || pagina == NULL) return;

    int posPagina = pagina->posPagina;
    pagina->pagina_ativa = false;
    pagina->pos_prox_livre = cabecalho->prox_pos_livre;
    cabecalho->prox_pos_livre = posPagina;
    pagina->qtd_chaves_atuais = 0;
}

// Insere uma pagina da RAM no arquivo
void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*)){
    verificaArquivo(arquivo);
    if (cabecalho == NULL || p == NULL) return;

    size_t posicaoPaginaArquivo = calculaDeslocamentoPagina(p->posPagina, cabecalho->tamanho_registro);
    fseek(arquivo, posicaoPaginaArquivo, SEEK_SET);
    
    // Escreve primeiro as informações iniciais da página em questão, sem considerar o dado genérico
    fwrite(&p->pagina_ativa, sizeof(bool), 1, arquivo);
    fwrite(&p->pos_prox_livre, sizeof(int), 1, arquivo);
    fwrite(&p->posPagina, sizeof(int), 1, arquivo);
    fwrite(&p->qtd_chaves_atuais, sizeof(int),  1, arquivo);
    fwrite(&p->ehFolha, sizeof(bool), 1, arquivo);
    fwrite(p->posFilhos, sizeof(int) * ORDEM, 1, arquivo);
    fwrite(&p->posProximo, sizeof(int), 1, arquivo);
    fwrite(&p->posRegistro, sizeof(int) * ORDEM, 1, arquivo);

    // Escreve as chaves do dado genérico (o restante)
    for (int i = 0; i < p->qtd_chaves_atuais; i++) {
        escreveDado(p->chaves[i], arquivo);
    }
}

// Reconstitui uma página do arquivo para a RAM
Pagina* leituraPagina(FILE* arquivo, Bplus* cabecalho, int posPagina, void (*leituraDado)(void*, FILE*)) {
    verificaArquivo(arquivo);
    if (cabecalho == NULL || posPagina < 0) return NULL;

    Pagina* p = criaPagina(cabecalho->tamanho_registro, false);

    size_t posicaoPaginaArquivo = calculaDeslocamentoPagina(posPagina, cabecalho->tamanho_registro);
    fseek(arquivo, posicaoPaginaArquivo, SEEK_SET);

    // Lê os dados iniciais (não genéricos) de acordo com a ordem na struct
    fread(&p->pagina_ativa, sizeof(bool), 1, arquivo);
    fread(&p->pos_prox_livre, sizeof(int),  1, arquivo);
    fread(&p->posPagina, sizeof(int),  1, arquivo);
    fread(&p->qtd_chaves_atuais, sizeof(int),  1, arquivo);
    fread(&p->ehFolha, sizeof(bool), 1, arquivo);
    fread(p->posFilhos, sizeof(int) * ORDEM, 1, arquivo);
    fread(&p->posProximo, sizeof(int),  1, arquivo);
    fread(&p->posRegistro, sizeof(int) * ORDEM, 1, arquivo);

    // Lê o restante da página (os dados genéricos)
    for (int i = 0; i < p->qtd_chaves_atuais; i++) {
        leituraDado(p->chaves[i], arquivo);
    }

    return p;
}

//Retorna una página, além de que a variável 'posBusca' terminará com a posição final da busca e 'encontrou' indicará se foi encontrado ou não:
Pagina* buscaDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoBuscado, int* indiceBusca, bool* encontrou, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*)) {
    *indiceBusca = -1;
    *encontrou = false;

    verificaArquivo(arquivo);
    
    if (cabecalho == NULL || cabecalho->raiz == -1) return NULL;

    int posAtual = cabecalho->raiz;
    
    // Traz a página raiz do disco para a RAM
    Pagina* paginaAtual = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);

    // Navegação pelas páginas internas
    while (!paginaAtual->ehFolha) {
        int i = 0;
        
        // Se a chave K_i é menor que o X (dado buscado), devemos olhar mais para a direita.
        // O laço para assim que encontrar um K_i >= X, o que significa que o filho correto é o P_i.
        while (i < paginaAtual->qtd_chaves_atuais && !ehMenor(dadoBuscado, paginaAtual->chaves[i])) {
            i++;
        }
        
        // Atualiza a posição atual para o ponteiro do filho encontrado
        posAtual = paginaAtual->posFilhos[i];
        
        // Libera a página interna da RAM antes de descer de nível
        liberaPaginaRAM(paginaAtual);
        
        // Lê a página filha do disco para a RAM
        paginaAtual = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);
        
        // Tratamento de segurança caso a leitura falhe
        if (paginaAtual == NULL) return NULL; 
    }
    
    // Busca na página folha
    int i = 0;
    
    // O laço para de avançar assim que achar um elemento que não é menor que o dado buscado.
    while (i < paginaAtual->qtd_chaves_atuais && !ehMenor(dadoBuscado, paginaAtual->chaves[i])) {
        i++;
    }
    
    *indiceBusca = i; 

    // Verifica se o elemento nessa posição é uma correspondência exata
    if (i < paginaAtual->qtd_chaves_atuais) {
        
        // Se X não é menor que K, e K não é menor que X, então X == K.
        // Utiliza ehMenor para aproveitar código
        if (!ehMenor(dadoBuscado, paginaAtual->chaves[i]) && !ehMenor(paginaAtual->chaves[i], dadoBuscado)) {
            *encontrou = true;
        }
    }

    return paginaAtual;
}

int insereBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, int posRegistro, bool (*ehMenor)(void*, void*), void* (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*)) {
    verificaArquivo(arquivo);
    
    if (cabecalho == NULL) return -1;

    int raiz = cabecalho->raiz;
    int posAtual = raiz;
    
    // Caso a árvore não tenha nehuma página
    if (raiz == -1) {
        Pagina* pagina_raiz = criaPagina(cabecalho->tamanho_registro, true);
        pagina_raiz->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);
        pagina_raiz->chaves[0] = novo_dado;
        pagina_raiz->posRegistro[0] = posRegistro;
        pagina_raiz->qtd_chaves_atuais = 1;
        cabecalho->raiz = pagina_raiz->posPagina;

        escrevePagina(arquivo, cabecalho, pagina_raiz, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);
        liberaPaginaRAM(pagina_raiz);
        return 0;
    }
    
    Pagina* paginaAtual = leituraPagina(arquivo, cabecalho, raiz, leituraDado);

    // Guarda, antes de cada descida, a posição do nó de origem
    // Definimos o valor 20 pois o vetor está associado à altura da árvore, sendo muito difícil atingir o valor max.
    int posicoes_pais[MAX_CHAVES];

    // Guarda, antes de cada descida, o caminho escolhido para descer (indice do vetor de filhos)
    int indices_filhos[MAX_CHAVES];
    
    // Guarda quantas descidas foram realizas até uma folha
    int aux = 0;

    int i = 0;

    // Descida até a página correta para inserção
    while (!paginaAtual->ehFolha) {
        
        i = 0;
        while (i < paginaAtual->qtd_chaves_atuais && !ehMenor(novo_dado, paginaAtual->chaves[i])) {
            i++;
        }

        posicoes_pais[aux] = posAtual;
        indices_filhos[aux] = i;

        posAtual = paginaAtual->posFilhos[i];
        liberaPaginaRAM(paginaAtual);
        paginaAtual = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);
                
        if (paginaAtual == NULL) return -1;
        
        aux++; 
    }

    i = 0;

    while (i < paginaAtual->qtd_chaves_atuais && !ehMenor(novo_dado, paginaAtual->chaves[i])) {
        i++;
    }
        
    // Retorno valor especifico para tratar o caso dos dados serem iguais
    if (i < paginaAtual->qtd_chaves_atuais && !ehMenor(novo_dado, paginaAtual->chaves[i]) && !ehMenor(paginaAtual->chaves[i], novo_dado)) {
        liberaPaginaRAM(paginaAtual);
        return 42;
    }

    insereNaFolha(paginaAtual, novo_dado, posRegistro, ehMenor);
    
    if (paginaAtual->qtd_chaves_atuais < ORDEM) {
        escrevePagina(arquivo, cabecalho, paginaAtual, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);
        liberaPaginaRAM(paginaAtual);
        return 0;
    }
    
    Pagina* nova = splitFolha(paginaAtual, cabecalho);

    nova->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);

    nova->posProximo = paginaAtual->posProximo;
    paginaAtual->posProximo = nova->posPagina;

    // Cópia da chave que será enviada à página pai
    void* copiaChave = mallocSafe(cabecalho->tamanho_registro);
    memcpy(copiaChave, nova->chaves[0], cabecalho->tamanho_registro); 

    int posNova = nova->posPagina;
    
    // Caso tenha somente a raíz e ela deu overflow
    if (aux == 0) {
        Pagina* novaRaiz = criaPagina(cabecalho->tamanho_registro, false);
        novaRaiz->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);
        novaRaiz->chaves[0] = copiaChave;
        novaRaiz->posFilhos[0] = paginaAtual->posPagina;
        novaRaiz->posFilhos[1] = posNova;
        novaRaiz->qtd_chaves_atuais = 1;

        cabecalho->raiz = novaRaiz->posPagina;
        escrevePagina(arquivo, cabecalho, paginaAtual, escreveDado);
        escrevePagina(arquivo, cabecalho, nova, escreveDado);
        escrevePagina(arquivo, cabecalho, novaRaiz, escreveDado);
        
        escreveCabecalhoBplus(arquivo, cabecalho);
        
        liberaPaginaRAM(paginaAtual);
        liberaPaginaRAM(nova);
        liberaPaginaRAM(novaRaiz);
        return 0;
    }

    escrevePagina(arquivo, cabecalho, paginaAtual, escreveDado);
    liberaPaginaRAM(paginaAtual);
    escrevePagina(arquivo, cabecalho, nova, escreveDado);
    liberaPaginaRAM(nova);

    aux--;

    int posPai = posicoes_pais[aux];
    int indiceFilho = indices_filhos[aux];

    Pagina* pai = leituraPagina(arquivo, cabecalho, posPai, leituraDado);

    insereNoPai(pai, copiaChave, indiceFilho, posNova);
    
    if (pai->qtd_chaves_atuais < ORDEM) {
        escrevePagina(arquivo, cabecalho, pai, escreveDado);
        liberaPaginaRAM(pai);
        escreveCabecalhoBplus(arquivo, cabecalho);
        return 0;
    }

    aux--;

    while (pai->qtd_chaves_atuais >= ORDEM) {
        void* dadoSubida = pai->chaves[ORDEM / 2];
        pai->chaves[ORDEM / 2] = NULL;

        Pagina* nova = splitInterno(pai, cabecalho);
        nova->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);

        escrevePagina(arquivo, cabecalho, pai, escreveDado);
        escrevePagina(arquivo, cabecalho, nova, escreveDado);
        
        if (aux >= 0) {
            Pagina* avo = leituraPagina(arquivo, cabecalho, posicoes_pais[aux], leituraDado);
            insereNoPai(avo, dadoSubida, indices_filhos[aux], nova->posPagina);

            if (avo->qtd_chaves_atuais < ORDEM) {
                escrevePagina(arquivo, cabecalho, avo, escreveDado);

                liberaPaginaRAM(avo);
                liberaPaginaRAM(pai);
                liberaPaginaRAM(nova);

                break;
            }

            liberaPaginaRAM(pai);
            liberaPaginaRAM(nova);

            pai = avo;
            aux--;

            continue;
        }

        Pagina* novaRaiz = criaPagina(cabecalho->tamanho_registro, false);
        novaRaiz->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);
        novaRaiz->chaves[0] = dadoSubida;
        novaRaiz->qtd_chaves_atuais = 1;
        novaRaiz->posFilhos[0] = pai->posPagina;
        novaRaiz->posFilhos[1] = nova->posPagina;

        cabecalho->raiz = novaRaiz->posPagina;

        escrevePagina(arquivo, cabecalho, novaRaiz, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);

        liberaPaginaRAM(pai);
        liberaPaginaRAM(nova);
        liberaPaginaRAM(novaRaiz);

        break;
    }

    return 0;
}

void insereNaFolha(Pagina* pagina, void* novo_dado, int indiceDado, bool (*ehMenor)(void*, void*)) {
    if (pagina == NULL) return;

    int i = 0;

    // Procura a posição correta de inserção
    while (i < pagina->qtd_chaves_atuais && !ehMenor(novo_dado, pagina->chaves[i])) {
        i++;
    }
    
    // Percorre a página empurrando os elementos para a direita
    for (int j = pagina->qtd_chaves_atuais; j > i; j--) {
        pagina->chaves[j] = pagina->chaves[j - 1];
        pagina->posRegistro[j] = pagina->posRegistro[j - 1];
    }

    pagina->chaves[i] = novo_dado;
    pagina->posRegistro[i] = indiceDado;
    pagina->qtd_chaves_atuais++;
}

Pagina* splitFolha(Pagina* pagina, Bplus* cabecalho){
    if (cabecalho == NULL) return NULL;

    // divide a pagina ao meio
    int indiceDivisao = ORDEM / 2;
    
    Pagina* nova = criaPagina(cabecalho->tamanho_registro, true);

    // Envia para a nova folha os 3 registros sobressalentes
    for (int i = indiceDivisao; i < pagina->qtd_chaves_atuais; i++) {

        nova->chaves[nova->qtd_chaves_atuais] = pagina->chaves[i];
        nova->posRegistro[nova->qtd_chaves_atuais] = pagina->posRegistro[i];
        nova->qtd_chaves_atuais++;

        pagina->chaves[i] = NULL;
        pagina->posRegistro[i] = -1;
    }

    pagina->qtd_chaves_atuais = indiceDivisao;
    return nova;
}

void insereNoPai(Pagina* pai, void* novo_dado, int indiceFilhoAntigo, int posNovoFilho) {
    if (pai == NULL || posNovoFilho == -1) return;

    // Desloca as chaves à direita de posFilhoAntigo
    for (int i = pai->qtd_chaves_atuais; i > indiceFilhoAntigo; i--) {
        pai->chaves[i] = pai->chaves[i - 1];
    }

    // Desloca os filhos à direita de posFilhoAntigo + 1
    for (int i = pai->qtd_chaves_atuais + 1; i > indiceFilhoAntigo + 1; i--) {
        pai->posFilhos[i] = pai->posFilhos[i - 1];
    }

    pai->chaves[indiceFilhoAntigo] = novo_dado;
    pai->posFilhos[indiceFilhoAntigo + 1] = posNovoFilho;
    pai->qtd_chaves_atuais++;
}

Pagina* splitInterno(Pagina* pagina, Bplus* cabecalho) {
    if (pagina == NULL || cabecalho == NULL) return NULL;

    int indiceDivisao = ORDEM / 2;
    int j = 0;

    Pagina* nova = criaPagina(cabecalho->tamanho_registro, false);

    // Copia as chaves da direita
    for (int i = indiceDivisao + 1; i < pagina->qtd_chaves_atuais; i++) {
        nova->chaves[j] = pagina->chaves[i];
        pagina->chaves[i] = NULL;
        j++;
    }

    // Copia os filhos correspondentes
    j = 0;

    for (int i = indiceDivisao + 1; i <= pagina->qtd_chaves_atuais; i++) {
        nova->posFilhos[j] = pagina->posFilhos[i];
        pagina->posFilhos[i] = -1;
        j++;
    }

    // Remove também a chave promovida
    pagina->chaves[indiceDivisao] = NULL;
    nova->qtd_chaves_atuais = pagina->qtd_chaves_atuais - indiceDivisao - 1;
    pagina->qtd_chaves_atuais = indiceDivisao;
    return nova;
}

bool removeDadoBplus(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, void* dadoRemover, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*)) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL || pagina == NULL) return;
    
    int i = 0;
    int aux = 0;
    int posAtual = cabecalho->raiz;
    int indices_filhos[MAX_CHAVES];
    int posicoes_pais[MAX_CHAVES];
    
    while (!pagina->ehFolha) {
        
        while (i < pagina->qtd_chaves_atuais && !ehMenor(dadoRemover, pagina->chaves[i])) {
            i++;
        }

        posicoes_pais[aux] = posAtual;
        indices_filhos[aux] = i;

        posAtual = pagina->posFilhos[i];
        liberaPaginaRAM(pagina);
        pagina = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);
                
        if (pagina == NULL) return -1;
        
        aux++; 
    }

    i = 0;

    while (i < pagina->qtd_chaves_atuais && !ehMenor(dadoRemover, pagina->chaves[i])) {
        i++;
    }
    
    bool encontrou = (i < pagina->qtd_chaves_atuais) && !ehMenor(dadoRemover, pagina->chaves[i]) && !ehMenor(pagina->chaves[i], dadoRemover);
    
    if (!encontrou) {
        liberaPaginaRAM(pagina);
        return -1;
    }

    // Move os elementos 1 posição à esquerda
    for (int j = i; j < pagina->qtd_chaves_atuais - 1; j++) {
        pagina->chaves[j] = pagina->chaves[j + 1];
        pagina->posRegistro[j] = pagina->posRegistro[j + 1];
    }

    pagina->qtd_chaves_atuais--;

    // Remove o elemento para a raíz sendo folha
    if (pagina->posPagina == cabecalho->raiz) {

        // Caso em que a raíz fica com 0 chaves
        if (pagina->qtd_chaves_atuais == 0) {
            liberaLogicamentePagina(arquivo, cabecalho, pagina);
            cabecalho->raiz = -1;
            escreveCabecalhoBplus(arquivo, cabecalho);
            liberaPaginaRAM(pagina);
            return;
        }
        escrevePagina(arquivo, cabecalho, pagina, escreveDado);
        liberaPaginaRAM(pagina);
        return;
    }

    if (pagina->qtd_chaves_atuais >= MIN_CHAVES) {
        escrevePagina(arquivo, cabecalho, pagina, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);
        liberaPaginaRAM(pagina);
        return;
    }

    if (pagina->qtd_chaves_atuais < MIN_CHAVES) {
        bool conseguiu = redistribui(arquivo, cabecalho, pagina, posicoes_pais[aux], escreveDado, leituraDado);
        
    }

}

bool redistribui(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, int posPai, int indiceFilho, void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*)) {
    Pagina* pai = leituraPagina(arquivo, cabecalho, posPai, leituraDado);

    int chaveEsq = (indiceFilho < 0);
    int chaveDir = (indiceFilho < pai->qtd_chaves_atuais);

    Pagina* irmaoEsq = chaveEsq ? leituraPagina(arquivo, cabecalho, pai->posFilhos[indiceFilho - 1], leituraDado) : NULL;
    Pagina* irmaoDir = chaveDir ? leituraPagina(arquivo, cabecalho, pai->posFilhos[indiceFilho + 1], leituraDado) : NULL;
    
    if (irmaoDir != NULL && irmaoEsq->chaves > MIN_CHAVES){
        int tmp = irmaoEsq->qtd_chaves_atuais - 1;

        for (int i = pagina->qtd_chaves_atuais; i > 0; i--) {
            pagina->chaves[i] = pagina->chaves[i - 1];
            pagina->posRegistro[i] = pagina->posRegistro[i + 1];
        }

        
    }
    
}



void imprimeArvoreBplus(FILE* arquivo, Bplus* cabecalho, int posAtual, int profundidade, void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*)) {
    if (posAtual == -1) return;

    Pagina *p = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);

    for (int i = 0; i < profundidade; i++) {
        printf("        ");
    }

    if (p->ehFolha) 
        printf("[FOLHA | posicao: %d]", p->posPagina);

    else
        printf("[INTERNO | posicao: %d]", p->posPagina);

    for (int i = 0; i < p->qtd_chaves_atuais; i++) {
        imprimeChave(p->chaves[i]);
        if (i < p->qtd_chaves_atuais - 1)
            printf(" | ");
    }
    printf("\n");

    if (!p->ehFolha) {
        for (int i = 0; i <= p->qtd_chaves_atuais; i++) {
            imprimeArvoreBplus(arquivo, cabecalho, p->posFilhos[i], profundidade + 1, imprimeChave, leituraDado);
        }
    }
    liberaPaginaRAM(p);
}

void buscaIntervaloBplus(FILE* arquivo, Bplus* cabecalho, void* limite_inferior, void* limite_superior, int *indiceBusca, bool (*ehMenor)(void*, void*), void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*)) {
    Pagina* paginaAtual = buscaDadoBplus(arquivo, cabecalho, limite_inferior, indiceBusca, false, ehMenor, leituraDado);
    while (!ehMenor(lmi))
        for (int i = indiceBusca; i < paginaAtual->qtd_chaves_atuais; i++) {
            if (ehMenor(limite_inferior, paginaAtual->chaves[i]) && ehMenor(paginaAtual->chaves[i], limite_inferior))
                imprimeChave(paginaAtual->chaves[i]);
            if (!ehMenor(limite_superior, paginaAtual->chaves[i]))
                break;
        }
        
        if (paginaAtual->posProximo == -1) return;

        int posProxPagina = paginaAtual->posProximo;
        liberaPaginaRAM(paginaAtual);

        paginaAtual = (arquivo, cabecalho, posProxPagina, leituraDado);
        
        i = 0;
    
    
    

    
}












