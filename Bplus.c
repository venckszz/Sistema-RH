#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Bplus.h"
#include "Util.h"

#define ORDEM 5

Bplus* criaCabecalhoBplus(FILE* arquivo, size_t tamanho_dado) {
    verificaArquivo(arquivo);

    Bplus* cabecalho = (Bplus*) mallocSafe(sizeof(Bplus));
    cabecalho->ordem = ORDEM;
    cabecalho->prox_pos_livre = -1;
    cabecalho->qtd_paginas = 0;
    cabecalho->raiz = -1;

    // Armazena o tamanho do tipo de dado genérico a ser tratado
    cabecalho->tamanho_registro = tamanho_dado;

    rewind(arquivo);
    fwrite(cabecalho, sizeof(Bplus), 1, arquivo);
    return cabecalho;
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

    // Aloca espaço para o dado genérico
    for (int i = 0; i < ORDEM - 1; i++) {
        novaPagina->chaves[i] = mallocSafe(tamanho_dado);
    }

    return novaPagina;
}

// Função não finalizada
int alocaPagina(FILE* arquivo, Bplus* cabecalho) {
    verificaArquivo(arquivo);

    if (cabecalho->prox_pos_livre != -1) {
        Pagina* pagina = leituraPagina(arquivo, cabecalho, cabecalho->prox_pos_livre, leituraDado)
    }

    int pos = cabecalho->qtd_paginas;
    cabecalho->qtd_paginas++;

    return pos;
}



// Insere uma pagina da RAM no arquivo
void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*)){
    verificaArquivo(arquivo);
    if (cabecalho == NULL || p == NULL) return;

    size_t posicaoPaginaArquivo = calculaDeslocamentoPagina(p->posPagina, cabecalho->tamanho_registro);
    fseek(arquivo, posicaoPaginaArquivo, SEEK_SET);
    
    // Escreve primeiro as informações iniciais da página em questão, sem considerar o dado genérico
    size_t tamanho_fixo = sizeof(Pagina) - (sizeof(void*) * (ORDEM - 1));
    fwrite(p, tamanho_fixo, 1, arquivo);

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

    // Lê os dados iniciais (não genéricos)
    size_t tamanho_fixo = sizeof(Pagina) - (sizeof(void*) * (ORDEM - 1));
    if (fread(p, tamanho_fixo, 1, arquivo) != 1){
        for (int i = 0; i < ORDEM - 1; i++) {
            free(p->chaves[i]);
        }

        free(p);
        return NULL;
    }

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
        while (i < paginaAtual->qtd_chaves_atuais && !ehMenor(paginaAtual->chaves[i], dadoBuscado)) {
            i++;
        }
        
        // Atualiza a posição atual para o ponteiro do filho encontrado
        posAtual = paginaAtual->posFilhos[i];
        
        // Libera a página interna da RAM antes de descer de nível
        liberaPagina(paginaAtual);
        
        // Lê a página filha do disco para a RAM
        paginaAtual = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);
        
        // Tratamento de segurança caso a leitura falhe
        if (paginaAtual == NULL) return NULL; 
    }
    
    // Busca na página folha
    int i = 0;
    
    // O laço para de avançar assim que achar um elemento que NÃO É menor que o dado buscado.
    while (i < paginaAtual->qtd_chaves_atuais && ehMenor(paginaAtual->chaves[i], dadoBuscado)) {
        i++;
    }
    
    // Salva a posição exata onde o dado está ou DEVERÁ ser inserido
    *indiceBusca = i; 

    // Verifica se o elemento nessa posição é uma correspondência exata
    if (i < paginaAtual->qtd_chaves_atuais) {
        
        // Se X não é menor que K, e K não é menor que X, então X == K.
        if (!ehMenor(dadoBuscado, paginaAtual->chaves[i]) && !ehMenor(paginaAtual->chaves[i], dadoBuscado)) {
            *encontrou = true;
        }
    }

    return paginaAtual;
}

int insereDadoBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, bool (*ehMenor)(void*, void*), void (*escreve)(void*), void (*leituraDado)(void*, FILE*)) {
    verificaArquivo(arquivo);
    
    int posAtual;
    int raiz = cabecalho->raiz;
    
    // Valor para definir o tamanho do array para salvar os offsets a cada descida
    int TAM_MAX = cabecalho->qtd_paginas;
    
    //Caso a árvore não tenha nehuma página
    if (raiz == -1) {
        Pagina* pagina_raiz = criaPagina(cabecalho->tamanho_registro, true);
        fwrite(pagina_raiz, sizeof(Bplus), 1, arquivo);
        cabecalho->raiz = pagina_raiz->posPagina;
        pagina_raiz->chaves[0] = novo_dado;
        return 0;
    }

    Pagina* paginaAtual = leituraPagina(arquivo, cabecalho, raiz, leituraDado);

    // Arrays para guardar o caminho de descida
    int offsets_pais[TAM_MAX];
    int offsets_filhos[TAM_MAX];
    int aux = 0;

    while (!paginaAtual->ehFolha) {
        int i = 0;
        
        while (i < paginaAtual->qtd_chaves_atuais && !ehMenor(paginaAtual->chaves[i], novo_dado)) {
            i++;
        }

        offsets_pais[aux] = posAtual;
        offsets_filhos[aux] = i;
        posAtual = paginaAtual->posFilhos[i];
        liberaPagina(paginaAtual);
        paginaAtual = leituraPagina(arquivo, cabecalho, posAtual, leituraDado);
                
        if (paginaAtual == NULL) return -1; 

        while (i < paginaAtual->qtd_chaves_atuais && ehMenor(paginaAtual->chaves[i], novo_dado)) {
            i++;
        }
        
        // Retorno especifico para tratar o caso dos dados serem iguais
        if (paginaAtual->chaves[i] == novo_dado) {
            return 42;
        }

        for (int j = paginaAtual->qtd_chaves_atuais; j > i; j--) {
            paginaAtual->chaves[j] = paginaAtual->chaves[j - 1];
            paginaAtual->posFilhos[j] = paginaAtual->posFilhos[j - 1];
        }

        paginaAtual->chaves[i] = memcpy();
        
        
    }



}

// Pagina* divideFolha(FILE* arquivo, Bplus* cabecalho, Pagina* folha, void* novo_dado, int indice_insercao, void* chave_promovida, bool (*ehMenor)(void*, void*)) {
//     verificaArquivo(arquivo);
//     if (cabecalho == NULL || folha == NULL) return NULL;

//     Pagina* novaFolha = criaPagina(cabecalho->tamanho_registro, true);
//     novaFolha->posPagina = alocaPagina(arquivo, cabecalho);

//     void* temp[ORDEM];

//     for (int i = 0; i < ORDEM; i++) {
//         temp[i] = mallocSafe(cabecalho->tamanho_registro);
//     }

//     int posInserção = -1;

//     Pagina
// }

void liberaPagina(Pagina *p) {
    if (p == NULL) return;

    for (int i = 0; i < ORDEM - 1; i++) {
        free(p->chaves[i]);
    }

    free(p);
}

bool removeDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoRemover, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*)) {
    

}











