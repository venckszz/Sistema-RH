#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Bplus.h"
#include "Util.h"
#include "dadoBusca.h"

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

void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*)){
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

Pagina* leituraPagina(FILE* arquivo, Bplus* cabecalho, int posPagina, void (*leituraDado)(void*, FILE*)) {
    Pagina* p = mallocSafe(sizeof(Pagina));
    size_t posicaoPaginaArquivo = calculaDeslocamentoPagina(posPagina, cabecalho->tamanho_registro);
    fseek(arquivo, posicaoPaginaArquivo, SEEK_SET);

    // Lê os dados iniciais (não genéricos)
    size_t tamanho_fixo = sizeof(Pagina) - (sizeof(void*) * (ORDEM - 1));
    fread(p, tamanho_fixo, 1, arquivo);

    // Lê o restante da página (os dados genéricos)
    for (int i = 0; i < p->qtd_chaves_atuais; i++) {
        leituraDado(p->chaves[i], arquivo);
    }

    return p;
}

bool buscaBplus(FILE* arquivo, Bplus* cabecalho, void* dadoBuscado, bool (*ehMenor)(void*, void*)) {
    if (arquivo == NULL || cabecalho == NULL || cabecalho->raiz == -1) {
        return false;
    }

    int posAtual = cabecalho->raiz;
    Pagina* paginaAtual = (Pagina*) mallocSafe(sizeof(Pagina));

    size_t posicaoPagina = calculaDeslocamentoPagina(posAtual, cabecalho->tamanho_registro);

    while (posAtual != -1) {
        
        // Posiciona o ponteiro e lê a base da página do disco
        fseek(arquivo, posAtual, SEEK_SET);
        fread(paginaTemporaria, sizeof(Pagina), 1, arquivo);

        /* * =========================================================================
         * ALERTA ARQUITETURAL (LEITURA DE DISCO):
         * A sua struct 'Pagina' possui um array de ponteiros: void* chaves[ORDEM - 1].
         * Um fread() simples copia apenas os endereços de memória antigos gravados 
         * no disco, o que causará Falha de Segmentação (Segfault) ao tentar acessá-los.
         * * O documento do trabalho exige o uso de callbacks de escrita/leitura.
         * Você precisará alocar espaço dinamicamente para cada chave desta página e 
         * usar o callback de leitura para trazer os bytes reais do arquivo para a RAM 
         * antes de iniciar as comparações no laço abaixo.
         * =========================================================================
         */

        int i = 0;
        bool encontrou_caminho = false;

        // Varredura das chaves da página atual
        while (i < paginaTemporaria->qtd_chaves_atuais) {
            // Se a chave na posição 'i' NÃO for menor que o dado buscado, 
            // significa que dadoBuscado <= chave[i]. Achamos o caminho!
            if (!compara(paginaTemporaria->chaves[i], dadoBuscado)) {
                encontrou_caminho = true;
                break;
            }
            i++;
        }

        // 4. Validação se alcançamos o Nó Folha
        if (paginaTemporaria->ehFolha) {
            bool encontrado = false;
            
            // Verifica se o índice não ultrapassou o limite e checa a igualdade exata
            if (i < paginaTemporaria->qtd_chaves_atuais) {
                // É igual se não for menor, nem maior
                bool menor_que = compara(dadoBuscado, paginaTemporaria->chaves[i]);
                bool maior_que = compara(paginaTemporaria->chaves[i], dadoBuscado);
                
                if (!menor_que && !maior_que) {
                    encontrado = true;
                }
            }

            // Libera a memória temporária (não esqueça de liberar os void* internos depois!)
            free(paginaTemporaria);
            return encontrado;
        } 

        // 5. Continua descendo (Nó Interno)
        else {
            if (encontrou_caminho) {

                // Desce pelo ponteiro à esquerda da chave encontrada
                posAtual = paginaTemporaria->posFilhos[i];
            } else {

                // dadoBuscado é maior que todas as chaves, desce pelo último ponteiro
                posAtual = paginaTemporaria->posFilhos[paginaTemporaria->qtd_chaves_atuais];
            }
        }
    }

    // Liberação defensiva caso o laço quebre de forma anômala
    free(paginaTemporaria);
    return false;
}

void insereBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, bool (*ehMenor)(void*, void*), void (*escreve)(void*)) {
    verificaArquivo(arquivo);
    
    if (cabecalho->raiz == -1) {
        Bplus* raiz = criaPagina(cabecalho->tamanho_registro, true);
        
    }
    else {
        busca
    }


}

Pagina* criaPagina(size_t tamanho_dado, bool ehFolha) {
    Pagina* novaPagina = (Pagina*) mallocSafe(sizeof(Pagina));
    
    novaPagina->ehFolha = ehFolha;
    novaPagina->pai = -1;
    novaPagina->pagina_ativa = true;
    novaPagina->pos_prox_livre = -1;
    novaPagina->posPagina = -1;

    memset(novaPagina->posFilhos, -1, ORDEM * sizeof(int));

    // Aloca espaco para o dado generico
    for (int i = 0; i < ORDEM - 1; i++) {
        novaPagina->chaves[i] = mallocSafe(tamanho_dado);
    }

    return novaPagina;
}











