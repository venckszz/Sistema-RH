/**
 * @file Bplus.h
 * @brief Arquivo cabeçalho responsável por guardar todas as assinaturas de funções e estruturas utilizadas no código fonte da Bplus(árvore B+).
 * @author Grupo 1: Jonathan Alves Bispo da Paz [2024200497], Leandro Brognoli Grazziotin [2024200523] e Victor da Rocha Toniato [2024200493].
 * @date 05/07/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/** @brief Indica a ordem utilizada para estruturar a árvore Bplus (máximo de ponteiros/filhos por nó) */
#define ORDEM 5

/**
 * @brief Estrutura que representa uma página (nó interno ou folha) da Árvore Bplus na RAM.
 */
typedef struct pagina {
    bool pagina_ativa; /**< Indica se a página está ativa ou se foi logicamente removida */
    int pos_prox_livre; /**< Próxima página livre no arquivo (utilizado em lista encadeada de páginas inativas, -1 se ativa) */
    int posPagina; /**< Posição física (índice) da página no disco */
    int qtd_chaves_atuais; /**< Quantidade atual de chaves armazenadas nesta página */
    bool ehFolha; /**< Booleano que indica se é uma página folha (true) ou nó interno (false) */
   
    int posFilhos[ORDEM + 1]; /**< Vetor de posições físicas (índices) para os nós filhos no disco (usado apenas em nós internos) */
    int posProximo; /**< Posição (índice) no disco da próxima folha na lista encadeada (usado apenas em folhas, -1 em internas) */
    int posRegistro[ORDEM]; /**< Vetor de índices para os registros de dados no arquivo principal (usado apenas em folhas) */
    void* chaves[ORDEM]; /**< Vetor de ponteiros genéricos para as chaves armazenadas na página */
} Pagina;

/**
 * @brief Estrutura que representa o cabeçalho da Árvore Bplus no início do arquivo.
 */
typedef struct Bplus {
    int raiz; /**< Posição física (índice da página) da raiz da árvore no arquivo (-1 se vazia) */
    int ordem; /**< Ordem da árvore (número máximo de filhos de um nó interno) */

    size_t tamanho_registro; /**< Tamanho em bytes da chave genérica armazenada na árvore */
    int qtd_paginas; /**< Quantidade total de páginas já salvas no disco */
    int prox_pos_livre; /**< Cabeça da lista encadeada de páginas removidas para reaproveitamento (-1 se nenhuma) */
} Bplus;

/**
 * @brief Função responsável por inicializa a estrutura da árvore, gravando o cabeçalho da árvore vazio no disco.
 * @param arquivo Ponteiro para o arquivo da árvore já aberto.
 * @param tamanho_dado Tamanho em bytes da chave genérica que será utilizada.
 */
void inicializaBplus(FILE* arquivo, size_t tamanho_dado);

/**
 * @brief Função responsável por fechar de forma segura o arquivo da árvore binária.
 * @param arquivo Ponteiro para o arquivo que será fechado.
 * @param cabecalho Ponteiro para o cabeçalho atual da árvore na RAM.
 */
void fechaBplus(FILE* arquivo, Bplus* cabecalho);

/**
 * @brief Função responsável por lê o cabeçalho da Árvore Bplus diretamente do início do arquivo para a RAM.
 * @param arquivo Ponteiro para o arquivo de onde o cabeçalho será lido.
 * @return Bplus* Ponteiro para a estrutura de cabeçalho alocada dinamicamente, ou NULL em caso de erro.
 */
Bplus* leituraCabecalhoBplus(FILE* arquivo);

/**
 * @brief Função responsável por gravar ou atualizar a estrutura do cabeçalho no início (posição 0) do arquivo.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro com os dados mais recentes do cabeçalho na RAM.
 */
void escreveCabecalhoBplus(FILE* arquivo, Bplus* cabecalho);

/**
 * @brief Função responsável por determinar a posição no arquivo (índice) para alocar uma nova página, priorizando o reaproveitamento de blocos livres.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param leituraDado Callback de leitura (usado para ler páginas livres e atualizar a lista encadeada de reaproveitamento).
 * @return int Posição física (índice) onde a nova página deve ser gravada no disco.
 */
int ProxPagina(FILE* arquivo, Bplus* cabecalho, void (*leituraDado)(void*, FILE*));

/**
 * @brief Aloca dinamicamente e inicializa uma nova estrutura de página na memória RAM.
 * @param tamanho_dado Tamanho em bytes de cada chave genérica.
 * @param ehFolha Indica se a página será criada como folha (true) ou nó interno (false).
 * @return Pagina* Ponteiro para a página recém-alocada na RAM.
 */
Pagina* criaPagina(size_t tamanho_dado, bool ehFolha);

/**
 * @brief Libera toda a memória RAM alocada dinamicamente para uma página e suas chaves genéricas.
 * @param p Ponteiro para a página que será liberada da memória.
 */
void liberaPaginaRAM(Pagina *p);

/**
 * @brief Marca uma página como excluída logicamente no disco e a insere na lista de reaproveitamento.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param pagina Ponteiro para a página que será desativada.
 * @param escreveDado Callback para gravar a chave genérica no disco ao reescrever a página.
 */
void liberaLogicamentePagina(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, void (*escreveDado)(void*, FILE*));

/**
 * @brief Grava fisicamente todos os dados e chaves genéricas de uma página da RAM para o arquivo em disco.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param p Ponteiro para a página com os dados na RAM.
 * @param escreveDado Callback responsável para gravar a chave genérica no arquivo.
 */
void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*));

/**
 * @brief Carrega uma página específica do disco binário para a memória RAM.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param posPagina Índice físico (posição) da página a ser lida do disco.
 * @param leituraDado Callback delegado para desserializar a chave genérica do arquivo para a RAM.
 * @return Pagina* Ponteiro para a página carregada na memória RAM.
 */
Pagina* leituraPagina(FILE* arquivo, Bplus* cabecalho, int posPagina, void (*leituraDado)(void*, FILE*));

/**
 * @brief Realiza a busca por uma chave na árvore, navegando pelo disco até a folha correspondente.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param dadoBuscado Ponteiro para a chave genérica que se deseja encontrar.
 * @param indiceBusca [Saída] Recebe o índice no vetor da folha onde a chave foi encontrada ou deveria ser inserida.
 * @param encontrou [Saída] Retorna true se a chave exata foi localizada, ou false caso contrário.
 * @param ehMenor Callback para comparação de ordem entre duas chaves genéricas.
 * @param leituraDado Callback para leitura das páginas e chaves a partir do disco.
 * @return Pagina* Ponteiro para a página folha na RAM onde a busca encerrou (deve ser liberada via liberaPaginaRAM após o uso).
 */
Pagina* buscaDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoBuscado, int* indiceBusca, bool* encontrou, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Insere uma nova chave e seu respectivo índice de registro na Árvore B+ em disco, lidando com cisões (splits).
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novo_dado Ponteiro para a chave genérica a ser indexada.
 * @param posRegistro Posição física do registro correspondente no arquivo de dados de negócios.
 * @param ehMenor Callback de comparação entre chaves.
 * @param escreveDado Callback para escrita de chaves no disco.
 * @param leituraDado Callback para leitura de chaves do disco.
 * @return int Retorna 0 em caso de sucesso, 42 em caso de chave duplicada exata, ou -1 em caso de erro.
 */
int insereBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, int posRegistro, bool (*ehMenor)(void*, void*), void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Insere ordenadamente uma chave e seu índice diretamente em um nó folha que ainda possui espaço na RAM.
 * @param pagina Ponteiro para a folha alvo na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novo_dado Ponteiro para a chave a ser inserida.
 * @param indiceDado Índice do registro de dados associado a essa chave.
 * @param ehMenor Callback de comparação entre chaves.
 */
void insereNaFolha(Pagina* pagina, Bplus* cabecalho, void* novo_dado, int indiceDado, bool (*ehMenor)(void*, void*));

/**
 * @brief Realiza a cisão (split) de uma página folha com overflow, transferindo metade das chaves para um novo nó folha.
 * @param pagina Ponteiro para a página folha que transbordou na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @return Pagina* Ponteiro para o novo nó folha gerado na direita contendo as chaves superiores.
 */
Pagina* splitFolha(Pagina* pagina, Bplus* cabecalho);

/**
 * @brief Insere uma nova chave de roteamento e o ponteiro para o filho direito em um nó interno (pai).
 * @param pai Ponteiro para o nó interno pai na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novo_dado Ponteiro para a chave de roteamento promovida.
 * @param indiceFilhoAntigo Índice do filho esquerdo no vetor de ponteiros do pai.
 * @param posNovoFilho Posição no disco do novo filho direito gerado após o split.
 */
void insereNoPai(Pagina* pai, Bplus* cabecalho, void* novo_dado, int indiceFilhoAntigo, int posNovoFilho);

/**
 * @brief Realiza a cisão (split) de um nó interno com overflow, separando ponteiros e promovendo a chave central.
 * @param pagina Ponteiro para o nó interno que transbordou na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @return Pagina* Ponteiro para o novo nó interno gerado à direita.
 */
Pagina* splitInterno(Pagina* pagina, Bplus* cabecalho);

/**
 * @brief Remove uma chave da Árvore B+, realizando rebalanceamento (redistribuição ou concatenação) se houver underflow.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param dadoRemover Ponteiro para a chave genérica a ser removida.
 * @param ehMenor Callback de comparação entre chaves.
 * @param leituraDado Callback de leitura das chaves do disco.
 * @param escreveDado Callback de escrita das chaves no disco.
 * @return int Retorna a posição do registro no arquivo de dados principal que foi desindexado, ou -1 se não encontrado.
 */
int removeDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoRemover, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*));

/**
 * @brief Concatena (mescla) dois nós irmãos e remove a chave divisora do nó pai em situações de underflow severo.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param pai Ponteiro para o nó pai na RAM.
 * @param indiceFilhoEsq Índice do ponteiro do irmão esquerdo no vetor do pai.
 * @param irmaoEsq Ponteiro para o nó irmão esquerdo na RAM.
 * @param irmaoDir Ponteiro para o nó irmão direito na RAM (que será logicamente liberado no disco).
 * @param escreveDado Callback para atualizar as páginas remanescentes no disco.
 */
void concatenaBplus(FILE* arquivo, Bplus* cabecalho, Pagina* pai, int indiceFilhoEsq, Pagina* irmaoEsq, Pagina* irmaoDir, void (*escreveDado)(void*, FILE*));

/**
 * @brief Tenta redistribuir chaves emprestando de um irmão esquerdo ou direito para resolver um underflow sem concatenar.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param pagina Ponteiro para o nó em underflow na RAM.
 * @param posPai Posição no disco do nó pai.
 * @param indiceFilho Índice de ponteiro desta página no nó pai.
 * @param escreveDado Callback de escrita de chaves no disco.
 * @param leituraDado Callback de leitura de chaves no disco.
 * @return bool Retorna true se a redistribuição foi bem-sucedida, ou false caso os irmãos não tenham chaves suficientes para emprestar.
 */
bool redistribui(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, int posPai, int indiceFilho, void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Imprime no terminal a hierarquia da Árvore B+ em disco (indicações de Raiz, Interno e Folhas com recuos visuais).
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param posAtual Índice da página por onde a impressão deve iniciar (geralmente a raiz).
 * @param profundidade Nível atual da recursão para controle de formatação (indentação de texto).
 * @param imprimeChave Callback para formatar e imprimir a chave genérica no terminal.
 * @param leituraDado Callback para ler os blocos do disco durante a travessia.
 */
void imprimeArvoreBplus(FILE* arquivo, Bplus* cabecalho, int posAtual, int profundidade, void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Realiza a busca por intervalo, percorrendo as folhas encadeadas no disco entre um limite inferior e superior.
 * @param arquivo Ponteiro para o arquivo binário da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param limite_inferior Ponteiro para a chave genérica que define o início do intervalo.
 * @param limite_superior Ponteiro para a chave genérica que define o final do intervalo.
 * @param ehMenor Callback de comparação entre chaves.
 * @param imprimeChave Callback de impressão para exibir cada chave encontrada no intervalo.
 * @param leituraDado Callback para leitura dos blocos encadeados do disco.
 */
int buscaIntervaloBplus(FILE* arquivo, Bplus* cabecalho, void* limite_inferior, void* limite_superior, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), int posRegistros[], int capacidade);