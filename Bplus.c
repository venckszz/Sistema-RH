/**
 * @file Bplus.c
 * @brief Arquivo do código fonte responsável por toda implementação de estrutura e funcionamento da Bplus(árvore B+).
 * @author Grupo 1: Jonathan Alves Bispo da Paz [2024200497], Leandro Brognoli Grazziotin [2024200523] e Victor da Rocha Toniato [2024200493].
 * @date 05/07/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "Bplus.h"
#include "Util.h"

#define MAX_ALTURA 20
#define MIN_CHAVES (ORDEM / 2)

void inicializaBplus(FILE* arquivo, size_t tamanho_dado) {
    verificaArquivo(arquivo);
    
    Bplus cabecalho;
    memset(&cabecalho, 0, sizeof(Bplus));

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
    fflush(arquivo);
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
    for (int i = 0; i <= ORDEM; i++) {
        novaPagina->posFilhos[i] = -1;
    }
    
    for (int i = 0; i < ORDEM; i++) {
        novaPagina->posRegistro[i] = -1;
    }

    // Aloca espaço para o dado genérico
    for (int i = 0; i < ORDEM; i++) {
        novaPagina->chaves[i] = mallocSafe(tamanho_dado);
        memset(novaPagina->chaves[i], 0, tamanho_dado);
    }

    return novaPagina;
}

void liberaPaginaRAM(Pagina *p) {
    if (p == NULL) return;

    for (int i = 0; i < ORDEM; i++) {
        if(p->chaves[i] != NULL) free(p->chaves[i]);
    }

    free(p);
}

void liberaLogicamentePagina(FILE* arquivo, Bplus* cabecalho, 
    Pagina* pagina, void (*escreveDado)(void*, FILE*)) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL || pagina == NULL) return;

    int posPagina = pagina->posPagina;
    pagina->pagina_ativa = false;
    pagina->pos_prox_livre = cabecalho->prox_pos_livre;
    cabecalho->prox_pos_livre = posPagina;
    pagina->qtd_chaves_atuais = 0;

    escrevePagina(arquivo, cabecalho, pagina, escreveDado);
}

// Insere uma pagina da RAM no arquivo
void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*)) {
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

    fflush(arquivo);
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
Pagina* buscaDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoBuscado, 
    int* indiceBusca, bool* encontrou, bool (*ehMenor)(void*, void*), 
    void (*leituraDado)(void*, FILE*)) {
    
    *indiceBusca = -1;
    *encontrou = false;

    verificaArquivo(arquivo);
    
    if (cabecalho == NULL || cabecalho->raiz == -1) return NULL;

    int posAtual = cabecalho->raiz;
    
    // Traz a página raiz do disco para a RAM
    Pagina* paginaAtual = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);

    int i = 0;

    // Navegação pelas páginas internas
    while (!paginaAtual->ehFolha) {
        i = 0;
        
        // Percorre para a direita para tentar encontrar a chave 
        while (i < paginaAtual->qtd_chaves_atuais && 
        !ehMenor(dadoBuscado, paginaAtual->chaves[i])) {
            
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
    i = 0;
    
    // O laço para de avançar assim que achar um elemento que não é menor que o dado buscado.
    while (i < paginaAtual->qtd_chaves_atuais && 
    ehMenor(paginaAtual->chaves[i], dadoBuscado)) {
        
        i++;
    }
    
    *indiceBusca = i; 

    // Verifica se o elemento nessa posição é uma correspondência exata
    if (i < paginaAtual->qtd_chaves_atuais) {
        
        // Verifica se o dado é igual a chave 
        if (!ehMenor(dadoBuscado, paginaAtual->chaves[i]) && 
            !ehMenor(paginaAtual->chaves[i], dadoBuscado)) {
            
                *encontrou = true;
        }
    }

    return paginaAtual;
}

int insereBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, int posRegistro, 
    bool (*ehMenor)(void*, void*), void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*)) {
    
        verificaArquivo(arquivo);

    if (cabecalho == NULL) return -1;

    int raiz = cabecalho->raiz;
    int posAtual = raiz;
    
    // Caso a árvore não tenha nehuma página
    if (raiz == -1) {
        Pagina* pagina_raiz = criaPagina(cabecalho->tamanho_registro, true);
        pagina_raiz->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);
        memcpy(pagina_raiz->chaves[0], novo_dado, cabecalho->tamanho_registro);
        pagina_raiz->posRegistro[0] = posRegistro;
        pagina_raiz->qtd_chaves_atuais = 1;
        cabecalho->raiz = pagina_raiz->posPagina;

        escrevePagina(arquivo, cabecalho, pagina_raiz, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);
        liberaPaginaRAM(pagina_raiz);
        return 0;
    }
    
    Pagina* paginaAtual = leituraPagina(arquivo, cabecalho, raiz, leituraDado);

    int posicoes_pais[MAX_ALTURA];

    // Guarda, antes de cada descida, o caminho escolhido para descer (indice do vetor de filhos)
    int indices_filhos[MAX_ALTURA];
    
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

    while (i < paginaAtual->qtd_chaves_atuais && ehMenor(paginaAtual->chaves[i], novo_dado)) {
        i++;
    }

    if (i < paginaAtual->qtd_chaves_atuais && !ehMenor(novo_dado, paginaAtual->chaves[i]) 
        && !ehMenor(paginaAtual->chaves[i], novo_dado)) {

        liberaPaginaRAM(paginaAtual);
        return 42; //Retorno específico para o caso dos dois iguais
    }

    insereNaFolha(paginaAtual, cabecalho, novo_dado, posRegistro, ehMenor);
    
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
    memset(copiaChave, 0, cabecalho->tamanho_registro);
    memcpy(copiaChave, nova->chaves[0], cabecalho->tamanho_registro);

    int posNova = nova->posPagina;
    
    // Caso tenha somente a raíz e ela deu overflow
    if (aux == 0) {
        Pagina* novaRaiz = criaPagina(cabecalho->tamanho_registro, false);
        novaRaiz->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);
        memcpy(novaRaiz->chaves[0], copiaChave, cabecalho->tamanho_registro);
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

        free(copiaChave);
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

    insereNoPai(pai, cabecalho, copiaChave, indiceFilho, posNova);
    free(copiaChave);
    
    if (pai->qtd_chaves_atuais < ORDEM) {
        escrevePagina(arquivo, cabecalho, pai, escreveDado);
        liberaPaginaRAM(pai);
        escreveCabecalhoBplus(arquivo, cabecalho);
        return 0;
    }

    aux--;

    while (pai->qtd_chaves_atuais >= ORDEM) {
        void* dadoSubida = mallocSafe(cabecalho->tamanho_registro);
        memset(dadoSubida, 0, cabecalho->tamanho_registro);
        memcpy(dadoSubida, pai->chaves[ORDEM / 2], cabecalho->tamanho_registro);

        nova = splitInterno(pai, cabecalho);
        nova->posPagina = ProxPagina(arquivo, cabecalho, leituraDado);

        escrevePagina(arquivo, cabecalho, pai, escreveDado);
        escrevePagina(arquivo, cabecalho, nova, escreveDado);
        
        if (aux >= 0) {
            Pagina* avo = leituraPagina(arquivo, cabecalho, posicoes_pais[aux], leituraDado);
            insereNoPai(avo, cabecalho, dadoSubida, indices_filhos[aux], nova->posPagina);

            free(dadoSubida);

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
        memcpy(novaRaiz->chaves[0], dadoSubida, cabecalho->tamanho_registro);
        free(dadoSubida);
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

void insereNaFolha(Pagina* pagina, Bplus* cabecalho, void* novo_dado, 
    int indiceDado, bool (*ehMenor)(void*, void*)) {
    
    if (pagina == NULL) return;

    int i = 0;

    // Procura a posição correta de inserção
    while (i < pagina->qtd_chaves_atuais && ehMenor(pagina->chaves[i], novo_dado)) {
        i++;
    }
    
    // Percorre a página empurrando os elementos para a direita
    for (int j = pagina->qtd_chaves_atuais; j > i; j--) {
        memcpy( pagina->chaves[j], pagina->chaves[j - 1], cabecalho->tamanho_registro);
        pagina->posRegistro[j] = pagina->posRegistro[j - 1];
    }

    memcpy(pagina->chaves[i], novo_dado, cabecalho->tamanho_registro);
    pagina->posRegistro[i] = indiceDado;
    pagina->qtd_chaves_atuais++;
}

Pagina* splitFolha(Pagina* pagina, Bplus* cabecalho) {
    if (cabecalho == NULL) return NULL;

    // divide a pagina ao meio
    int indiceDivisao = ORDEM / 2;
    
    Pagina* nova = criaPagina(cabecalho->tamanho_registro, true);

    // Envia para a nova folha os 3 registros sobressalentes
    for (int i = indiceDivisao; i < pagina->qtd_chaves_atuais; i++) {
        memcpy(nova->chaves[nova->qtd_chaves_atuais], pagina->chaves[i], 
               cabecalho->tamanho_registro);
        
            nova->posRegistro[nova->qtd_chaves_atuais] = pagina->posRegistro[i];
        nova->qtd_chaves_atuais++;

        pagina->posRegistro[i] = -1;
    }

    pagina->qtd_chaves_atuais = indiceDivisao;
    return nova;
}

void insereNoPai(Pagina* pai, Bplus* cabecalho, void* novo_dado, int indiceFilhoAntigo, int posNovoFilho) {
    if (pai == NULL || posNovoFilho == -1) return;

    // Desloca as chaves à direita de posFilhoAntigo
    for (int i = pai->qtd_chaves_atuais; i > indiceFilhoAntigo; i--) {
        memcpy(pai->chaves[i], pai->chaves[i - 1], cabecalho->tamanho_registro);
    }

    // Desloca os filhos à direita de posFilhoAntigo + 1
    for (int i = pai->qtd_chaves_atuais + 1; i > indiceFilhoAntigo + 1; i--) {
        pai->posFilhos[i] = pai->posFilhos[i - 1];
    }

    memcpy(pai->chaves[indiceFilhoAntigo], novo_dado, cabecalho->tamanho_registro);
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
        memcpy(nova->chaves[j], pagina->chaves[i], cabecalho->tamanho_registro);
        j++;
    }

    // Copia os filhos correspondentes
    j = 0;

    for (int i = indiceDivisao + 1; i <= pagina->qtd_chaves_atuais; i++) {
        nova->posFilhos[j] = pagina->posFilhos[i];
        pagina->posFilhos[i] = -1;
        j++;
    }

    nova->qtd_chaves_atuais = pagina->qtd_chaves_atuais - indiceDivisao - 1;
    pagina->qtd_chaves_atuais = indiceDivisao;
    return nova;
}

int removeDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoRemover, 
    bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*)) {
    
        verificaArquivo(arquivo);

    if (cabecalho == NULL || cabecalho->raiz == -1) return - 1;
    
    int i = 0;
    int aux = 0;
    int posAtual = cabecalho->raiz;
    int indices_filhos[MAX_ALTURA];
    int posicoes_pais[MAX_ALTURA];
    
    Pagina* pagina = leituraPagina(arquivo, cabecalho, cabecalho->raiz, leituraDado);

    if (pagina == NULL) return -1;

    while (!pagina->ehFolha) {
        i = 0;

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

    while (i < pagina->qtd_chaves_atuais && ehMenor(pagina->chaves[i], dadoRemover)) {
        i++;
    }
    
    bool encontrou = (i < pagina->qtd_chaves_atuais) && !ehMenor(dadoRemover, pagina->chaves[i]) 
                      && !ehMenor(pagina->chaves[i], dadoRemover);
    
    if (!encontrou) {
        liberaPaginaRAM(pagina);
        return -1;
    }

    int posRegistroNoArquivo = pagina->posRegistro[i];
    bool removeuPrimeiraChave = (i == 0);
    
    // Move os elementos 1 posição à esquerda
    for (int j = i; j < pagina->qtd_chaves_atuais - 1; j++) {
        memcpy(pagina->chaves[j], pagina->chaves[j + 1], cabecalho->tamanho_registro);
        pagina->posRegistro[j] = pagina->posRegistro[j + 1];
    }

    pagina->qtd_chaves_atuais--;

    // Remove o elemento para a raíz sendo folha
    if (pagina->posPagina == cabecalho->raiz) {

        // Caso em que a raíz fica com 0 chaves
        if (pagina->qtd_chaves_atuais == 0) {
            liberaLogicamentePagina(arquivo, cabecalho, pagina, escreveDado);
            cabecalho->raiz = -1;
            escreveCabecalhoBplus(arquivo, cabecalho);
            liberaPaginaRAM(pagina);
            return posRegistroNoArquivo;
        }
        escrevePagina(arquivo, cabecalho, pagina, escreveDado);
        liberaPaginaRAM(pagina);
        return posRegistroNoArquivo;
    }

    if (pagina->qtd_chaves_atuais >= MIN_CHAVES) {
        if (removeuPrimeiraChave && aux > 0) {
            int posPaiAtualizar = posicoes_pais[aux - 1];
            int indiceFilhoAtualizar = indices_filhos[aux - 1];

            if (indiceFilhoAtualizar > 0) {
                Pagina* paiAtualizar = leituraPagina(arquivo, cabecalho, posPaiAtualizar, leituraDado);

                memcpy(paiAtualizar->chaves[indiceFilhoAtualizar - 1],
                    pagina->chaves[0],
                    cabecalho->tamanho_registro);

                escrevePagina(arquivo, cabecalho, paiAtualizar, escreveDado);
                liberaPaginaRAM(paiAtualizar);
            }
        }

        escrevePagina(arquivo, cabecalho, pagina, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);
        liberaPaginaRAM(pagina);
        return posRegistroNoArquivo;
    }
    
    int posPai = posicoes_pais[aux - 1];
    int indiceFilho = indices_filhos[aux - 1];

    Pagina* pai = leituraPagina(arquivo, cabecalho, posPai, leituraDado);

    // Tenta em primeiro caso redistribuir as chaves com as páginas vizinhas, caso dê certo, atualiza as páginas no disco e finaliza
    if (redistribui(arquivo, cabecalho, pagina, pai->posPagina, indiceFilho, escreveDado, leituraDado)) {
        escreveCabecalhoBplus(arquivo, cabecalho);
        liberaPaginaRAM(pagina);
        liberaPaginaRAM(pai);
        return posRegistroNoArquivo;
    }
    
    // A distribuição não dando certo, tenta concatenar com páginas vizinhas
    Pagina* irmaoEsqFolha = (indiceFilho > 0) ? leituraPagina(arquivo, cabecalho, pai->posFilhos[indiceFilho - 1],leituraDado): NULL;
    Pagina* irmaoDirFolha = (indiceFilho < pai->qtd_chaves_atuais) ? leituraPagina(arquivo, cabecalho, pai->posFilhos[indiceFilho + 1], leituraDado) : NULL;

    if (irmaoEsqFolha != NULL) {
        concatenaBplus(arquivo, cabecalho, pai, indiceFilho - 1, irmaoEsqFolha, pagina, escreveDado);
        liberaPaginaRAM(pagina);
        pagina = NULL;

        if (irmaoDirFolha != NULL) {
            liberaPaginaRAM(irmaoDirFolha);
            irmaoDirFolha = NULL;
        }
    }  

    else {
        if (irmaoDirFolha != NULL) {
            concatenaBplus(arquivo, cabecalho, pai, indiceFilho, pagina, irmaoDirFolha, escreveDado);
            liberaPaginaRAM(irmaoDirFolha);
            irmaoDirFolha = NULL;
        }
    }

    // Verifica se a concatenação da folha deixou o 'pai' em underflow
    // Se o pai é a raiz e ficou sem chaves, a raiz deve descer para o único filho restante
    if (pai->posPagina == cabecalho->raiz && pai->qtd_chaves_atuais == 0) {
        cabecalho->raiz = pai->posFilhos[0];

        liberaLogicamentePagina(arquivo, cabecalho, pai, escreveDado);
        escreveCabecalhoBplus(arquivo, cabecalho);

        liberaPaginaRAM(pagina);

        if (irmaoEsqFolha != NULL) liberaPaginaRAM(irmaoEsqFolha);

        if (irmaoDirFolha != NULL) liberaPaginaRAM(irmaoDirFolha);

        liberaPaginaRAM(pai);
        return posRegistroNoArquivo;
    }

    if (pai->qtd_chaves_atuais >= MIN_CHAVES) {
        escreveCabecalhoBplus(arquivo, cabecalho);

        liberaPaginaRAM(pagina);

        if (irmaoEsqFolha != NULL) liberaPaginaRAM(irmaoEsqFolha);

        if (irmaoDirFolha != NULL) liberaPaginaRAM(irmaoDirFolha);

        liberaPaginaRAM(pai);
        return posRegistroNoArquivo;
    }

    aux--;

    if (pagina != NULL) {
    liberaPaginaRAM(pagina);
    pagina = NULL;
    }

    if (irmaoEsqFolha != NULL) {
        liberaPaginaRAM(irmaoEsqFolha);
        irmaoEsqFolha = NULL;
    }

    if (irmaoDirFolha != NULL) {
        liberaPaginaRAM(irmaoDirFolha);
        irmaoDirFolha = NULL;
    }

    while(pai->qtd_chaves_atuais < MIN_CHAVES) {
        if (pai->posPagina == cabecalho->raiz) {
            if (pai->qtd_chaves_atuais == 0) {
                cabecalho->raiz = pai->posFilhos[0];
                liberaLogicamentePagina(arquivo, cabecalho, pai, escreveDado);
                escreveCabecalhoBplus(arquivo, cabecalho);
            }

            else escrevePagina(arquivo, cabecalho, pai, escreveDado);
            
            liberaPaginaRAM(pai);
            return posRegistroNoArquivo;
        }

        int posAvo = posicoes_pais[aux - 1];
        int indicePaiNoAvo = indices_filhos[aux - 1];
        Pagina* avo = leituraPagina(arquivo, cabecalho, posAvo, leituraDado);

        // Tenta redistribuir os nós internos primeiro
        if (redistribui(arquivo, cabecalho, pai, posAvo, indicePaiNoAvo, escreveDado, leituraDado)) {
            escreveCabecalhoBplus(arquivo, cabecalho);
            liberaPaginaRAM(pai);
            liberaPaginaRAM(avo);
            return posRegistroNoArquivo;
        }

        // Se a redistribuição falhou, faz a concatenação dos nós internos
        Pagina* tioEsq = (indicePaiNoAvo > 0) ? leituraPagina(arquivo, cabecalho, avo->posFilhos[indicePaiNoAvo - 1], leituraDado) : NULL;
        Pagina* tioDir = (indicePaiNoAvo < avo->qtd_chaves_atuais) ? leituraPagina(arquivo, cabecalho, avo->posFilhos[indicePaiNoAvo + 1], leituraDado) : NULL;

        if (tioEsq != NULL) {
            concatenaBplus(arquivo, cabecalho, avo, indicePaiNoAvo - 1, tioEsq, pai, escreveDado);

            liberaPaginaRAM(pai);
            pai = NULL;

            liberaPaginaRAM(tioEsq);
            tioEsq = NULL;

            if (tioDir != NULL) {
                liberaPaginaRAM(tioDir);
                tioDir = NULL;
            }
        }

        else if (tioDir != NULL) {
            concatenaBplus(arquivo, cabecalho, avo, indicePaiNoAvo, pai, tioDir, escreveDado);
            liberaPaginaRAM(tioDir);
            tioDir = NULL;
        }

        if (pai != NULL) {
            liberaPaginaRAM(pai);
        }

        pai = avo;
        aux--;
    }

    escrevePagina(arquivo, cabecalho, pai, escreveDado);
    escreveCabecalhoBplus(arquivo, cabecalho);
    liberaPaginaRAM(pai);

    return posRegistroNoArquivo;
}

void concatenaBplus(FILE* arquivo, Bplus* cabecalho, Pagina* pai, int indiceFilhoEsq, Pagina* irmaoEsq,
    Pagina* irmaoDir, void (*escreveDado)(void*, FILE*)) {
    
    verificaArquivo(arquivo);

    if (cabecalho == NULL || pai == NULL || irmaoEsq == NULL || irmaoDir == NULL) return;

    if (!irmaoEsq->ehFolha) {
        memcpy(irmaoEsq->chaves[irmaoEsq->qtd_chaves_atuais], pai->chaves[indiceFilhoEsq], cabecalho->tamanho_registro);
        irmaoEsq->qtd_chaves_atuais++;
    }

    for (int i = 0; i < irmaoDir->qtd_chaves_atuais; i++) {
        memcpy(irmaoEsq->chaves[irmaoEsq->qtd_chaves_atuais + i], irmaoDir->chaves[i], cabecalho->tamanho_registro);
        
        if (irmaoDir->ehFolha && irmaoEsq->ehFolha)
            irmaoEsq->posRegistro[irmaoEsq->qtd_chaves_atuais + i] = irmaoDir->posRegistro[i];

        else
            irmaoEsq->posFilhos[irmaoEsq->qtd_chaves_atuais + i] = irmaoDir->posFilhos[i];
    }

    if (!irmaoDir->ehFolha && !irmaoEsq->ehFolha)
        irmaoEsq->posFilhos[irmaoEsq->qtd_chaves_atuais + irmaoDir->qtd_chaves_atuais] = irmaoDir->posFilhos[irmaoDir->qtd_chaves_atuais];

    irmaoEsq->qtd_chaves_atuais += irmaoDir->qtd_chaves_atuais;

    if (irmaoDir->ehFolha && irmaoEsq->ehFolha)
        irmaoEsq->posProximo = irmaoDir->posProximo;
    
    for (int i = indiceFilhoEsq; i < pai->qtd_chaves_atuais - 1; i++) {
        memcpy(pai->chaves[i], pai->chaves[i + 1], cabecalho->tamanho_registro);
        pai->posFilhos[i + 1] = pai->posFilhos[i + 2];
    }

    pai->qtd_chaves_atuais--;

    liberaLogicamentePagina(arquivo, cabecalho, irmaoDir, escreveDado);
    escrevePagina(arquivo, cabecalho, irmaoEsq, escreveDado);
    escrevePagina(arquivo, cabecalho, pai, escreveDado);
    escreveCabecalhoBplus(arquivo, cabecalho);
}

bool redistribui(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, int posPai, int indiceFilho, void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*)) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL || pagina == NULL) return false;
    
    Pagina* pai = leituraPagina(arquivo, cabecalho, posPai, leituraDado);
    if (pai == NULL) return false;

    int temEsq = (indiceFilho > 0);
    int temDir = (indiceFilho < pai->qtd_chaves_atuais);

    Pagina* irmaoEsq = temEsq ? leituraPagina(arquivo, cabecalho, pai->posFilhos[indiceFilho - 1], leituraDado) : NULL;
    Pagina* irmaoDir = temDir ? leituraPagina(arquivo, cabecalho, pai->posFilhos[indiceFilho + 1], leituraDado) : NULL;

    if (irmaoEsq != NULL && irmaoEsq->qtd_chaves_atuais > MIN_CHAVES) {
        int qtdPagina = pagina->qtd_chaves_atuais;
        int qtdEsq = irmaoEsq->qtd_chaves_atuais;

        if (pagina->ehFolha) {
            for (int i = qtdPagina; i > 0; i--) {
                memcpy(pagina->chaves[i], pagina->chaves[i - 1], cabecalho->tamanho_registro);
                pagina->posRegistro[i] = pagina->posRegistro[i - 1];
            }

            memcpy(pagina->chaves[0], irmaoEsq->chaves[qtdEsq - 1], cabecalho->tamanho_registro);
            pagina->posRegistro[0] = irmaoEsq->posRegistro[qtdEsq - 1];

            pagina->qtd_chaves_atuais++;
            irmaoEsq->qtd_chaves_atuais--;

            memcpy(pai->chaves[indiceFilho - 1], pagina->chaves[0], cabecalho->tamanho_registro);
        }
        else {
            for (int i = qtdPagina; i > 0; i--) {
                memcpy(pagina->chaves[i], pagina->chaves[i - 1], cabecalho->tamanho_registro);
            }

            for (int i = qtdPagina + 1; i > 0; i--) {
                pagina->posFilhos[i] = pagina->posFilhos[i - 1];
            }

            memcpy(pagina->chaves[0], pai->chaves[indiceFilho - 1], cabecalho->tamanho_registro);
            pagina->posFilhos[0] = irmaoEsq->posFilhos[qtdEsq];

            memcpy(pai->chaves[indiceFilho - 1], irmaoEsq->chaves[qtdEsq - 1], cabecalho->tamanho_registro);

            irmaoEsq->posFilhos[qtdEsq] = -1;
            pagina->qtd_chaves_atuais++;
            irmaoEsq->qtd_chaves_atuais--;
        }

        escrevePagina(arquivo, cabecalho, pagina, escreveDado);
        escrevePagina(arquivo, cabecalho, irmaoEsq, escreveDado);
        escrevePagina(arquivo, cabecalho, pai, escreveDado);

        liberaPaginaRAM(irmaoEsq);

        if (irmaoDir != NULL) liberaPaginaRAM(irmaoDir);

        liberaPaginaRAM(pai);
        return true;
    }

    if (irmaoDir != NULL && irmaoDir->qtd_chaves_atuais > MIN_CHAVES) {
        int qtdPagina = pagina->qtd_chaves_atuais;
        int qtdDir = irmaoDir->qtd_chaves_atuais;

        if (pagina->ehFolha) {
            memcpy(pagina->chaves[qtdPagina], irmaoDir->chaves[0], cabecalho->tamanho_registro);
            pagina->posRegistro[qtdPagina] = irmaoDir->posRegistro[0];

            pagina->qtd_chaves_atuais++;

            for (int i = 0; i < qtdDir - 1; i++) {
                memcpy(irmaoDir->chaves[i], irmaoDir->chaves[i + 1], cabecalho->tamanho_registro);
                irmaoDir->posRegistro[i] = irmaoDir->posRegistro[i + 1];
            }

            irmaoDir->qtd_chaves_atuais--;

            memcpy(pai->chaves[indiceFilho], irmaoDir->chaves[0], cabecalho->tamanho_registro);
        }

        else {
            memcpy(pagina->chaves[qtdPagina], pai->chaves[indiceFilho], cabecalho->tamanho_registro);
            pagina->posFilhos[qtdPagina + 1] = irmaoDir->posFilhos[0];

            pagina->qtd_chaves_atuais++;

            memcpy(pai->chaves[indiceFilho], irmaoDir->chaves[0], cabecalho->tamanho_registro);

            for (int i = 0; i < qtdDir - 1; i++) {
                memcpy(irmaoDir->chaves[i], irmaoDir->chaves[i + 1], cabecalho->tamanho_registro);
            }

            for (int i = 0; i < qtdDir; i++) {
                irmaoDir->posFilhos[i] = irmaoDir->posFilhos[i + 1];
            }

            irmaoDir->posFilhos[qtdDir] = -1;
            irmaoDir->qtd_chaves_atuais--;
        }

        escrevePagina(arquivo, cabecalho, pagina, escreveDado);
        escrevePagina(arquivo, cabecalho, irmaoDir, escreveDado);
        escrevePagina(arquivo, cabecalho, pai, escreveDado);

        liberaPaginaRAM(irmaoDir);

        if (irmaoEsq != NULL) liberaPaginaRAM(irmaoEsq);

        liberaPaginaRAM(pai);
        return true;
    }

    if (irmaoEsq != NULL) {
        liberaPaginaRAM(irmaoEsq);
    }

    if (irmaoDir != NULL) {
        liberaPaginaRAM(irmaoDir);
    }

    liberaPaginaRAM(pai);
    return false;
}

void imprimeArvoreBplus(FILE* arquivo, Bplus* cabecalho, int posAtual, int profundidade, void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*)) {
    if (posAtual == -1) return;

    Pagina *p = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);

    for (int i = 0; i < profundidade; i++) {
        printf("        ");
    }

    if (cabecalho->raiz == posAtual)
        printf("[RAIZ | posicao: %d] ", p->posPagina);

    else

    if (p->ehFolha) 
        printf("[FOLHA | posicao: %d] ", p->posPagina);

    else
        printf("[INTERNO | posicao: %d] ", p->posPagina);

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

int buscaIntervaloBplus(FILE* arquivo, Bplus* cabecalho, void* limite_inferior, void* limite_superior, 
    bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), int posRegistros[], int capacidade) {
    
    int indiceBusca = 0;
    int qtdEncontrados = 0;
    bool encontrou = false;

    verificaArquivo(arquivo);

    if (cabecalho == NULL || cabecalho->raiz == -1 || posRegistros == NULL || capacidade <= 0) {
        return 0;
    }

    Pagina* paginaAtual = buscaDadoBplus(arquivo, cabecalho, limite_inferior, 
                          &indiceBusca, &encontrou, ehMenor, leituraDado);
    
    while (paginaAtual != NULL) {
        for (int i = indiceBusca; i < paginaAtual->qtd_chaves_atuais; i++) {

            // Intervalo fechado: para somente quando chaveAtual > limite_superior
            if (ehMenor(limite_superior, paginaAtual->chaves[i])) {
                liberaPaginaRAM(paginaAtual);
                return qtdEncontrados;
            }

            // Enquanto chaveAtual < limite_inferior, apenas ignora
            if (ehMenor(paginaAtual->chaves[i], limite_inferior)) continue;
    

            if (qtdEncontrados < capacidade) {
                posRegistros[qtdEncontrados] = paginaAtual->posRegistro[i];
                qtdEncontrados++;
            }

            else {
                liberaPaginaRAM(paginaAtual);
                return qtdEncontrados;
            }
        }
        
        if (paginaAtual->posProximo == -1) {
            liberaPaginaRAM(paginaAtual);
            return qtdEncontrados;
        } 

        int posProxPagina = paginaAtual->posProximo;
        liberaPaginaRAM(paginaAtual);

        paginaAtual = leituraPagina(arquivo, cabecalho, posProxPagina, leituraDado);
        indiceBusca = 0;
    }

    return qtdEncontrados;
}