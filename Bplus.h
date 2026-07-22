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
 * @brief Função responsável por inicializar a estrutura da árvore, gravando o cabeçalho da árvore vazio no disco.
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
 * @brief Função responsável por ler o cabeçalho da Árvore Bplus diretamente do início do arquivo para a RAM.
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
 * @brief Função responsável por alocar dinamicamente e inicializar uma nova estrutura de página na memória RAM.
 * @param tamanho_dado Tamanho em bytes de cada chave genérica.
 * @param ehFolha Indica se a página será criada como folha (true) ou nó interno (false).
 * @return Pagina* Ponteiro para a página recém-alocada na RAM.
 */
Pagina* criaPagina(size_t tamanho_dado, bool ehFolha);

/**
 * @brief Função responsável por liberar toda a memória RAM alocada dinamicamente para uma página e suas chaves genéricas.
 * @param p Ponteiro para a página que será liberada da memória.
 */
void liberaPaginaRAM(Pagina *p);

/**
 * @brief Função responsável por marcar uma página como excluída logicamente no disco e a inserir na lista de reaproveitamento.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param pagina Ponteiro para a página que será desativada.
 * @param escreveDado Callback para gravar a chave genérica no disco ao reescrever a página.
 */
void liberaLogicamentePagina(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, void (*escreveDado)(void*, FILE*));

/**
 * @brief Função responsável por gravar fisicamente todos os dados e chaves genéricas de uma página da RAM para o arquivo em disco.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param p Ponteiro para a página com os dados na RAM.
 * @param escreveDado Callback responsável por gravar a chave genérica no arquivo.
 */
void escrevePagina(FILE* arquivo, Bplus* cabecalho, Pagina* p, void (*escreveDado)(void*, FILE*));

/**
 * @brief Função responsável por carregar uma página específica do disco para a memória RAM.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param posPagina posição física (índice) da página a ser lida do disco.
 * @param leituraDado Callback responsável por ler a chave genérica do arquivo para a RAM.
 * @return Pagina* Ponteiro para a página carregada na memória RAM.
 */
Pagina* leituraPagina(FILE* arquivo, Bplus* cabecalho, int posPagina, void (*leituraDado)(void*, FILE*));

/**
 * @brief Função responsável por realizar a busca por uma chave na árvore, navegando pelo disco até a folha correspondente.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param dadoBuscado Ponteiro para a chave genérica que se deseja encontrar.
 * @param indiceBusca Guarda o índice do vetor da folha onde a chave foi encontrada ou deve ser inserida.
 * @param encontrou Recebe 'true' se a chave exata foi localizada, ou 'false' caso contrário.
 * @param ehMenor Callback para comparação de ordem entre duas chaves genéricas.
 * @param leituraDado Callback para leitura das páginas e chaves a partir do disco.
 * @return Pagina* Ponteiro para a página folha na RAM onde a busca encerrou.
 */
Pagina* buscaDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoBuscado, int* indiceBusca, bool* encontrou, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Função responsável por inserir uma nova chave e seu respectivo índice de registro na árvore em disco, lidando com cisões.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novo_dado Ponteiro para a chave genérica a ser gravada.
 * @param posRegistro Posição física do registro correspondente no arquivo.
 * @param ehMenor Callback de comparação entre chaves.
 * @param escreveDado Callback para escrita de chaves no disco.
 * @param leituraDado Callback para leitura de chaves do disco.
 * @return int Valor 0 em caso de sucesso, 42 em caso de chave duplicada exata, ou -1 em caso de erro.
 */
int insereBplus(FILE* arquivo, Bplus* cabecalho, void* novo_dado, int posRegistro, bool (*ehMenor)(void*, void*), void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Função responsável por inserir ordenadamente uma chave e seu índice diretamente em um nó folha que ainda possui espaço na RAM.
 * @param pagina Ponteiro para a folha na RAM que receberá a inserção.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novo_dado Ponteiro para a chave a ser inserida.
 * @param indiceDado Índice do registro de dados associado a essa chave.
 * @param ehMenor Callback de comparação entre chaves.
 */
void insereNaFolha(Pagina* pagina, Bplus* cabecalho, void* novo_dado, int indiceDado, bool (*ehMenor)(void*, void*));

/**
 * @brief Função responsável por realizar a cisão (split) de uma página folha com overflow, transferindo metade das chaves para um novo nó folha.
 * @param pagina Ponteiro para a página folha que transbordou na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @return Pagina* Ponteiro para o novo nó folha gerado na direita contendo as chaves superiores.
 */
Pagina* splitFolha(Pagina* pagina, Bplus* cabecalho);

/**
 * @brief Função responsável por inserir uma nova chave para guiar a busca e o ponteiro para o filho direito em um nó interno (pai).
 * @param pai Ponteiro para o nó interno pai na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novo_dado Ponteiro para a chave guia que subiu.
 * @param indiceFilhoAntigo Índice do filho esquerdo no vetor de ponteiros do pai.
 * @param posNovoFilho Posição no disco do novo filho direito gerado após o split.
 */
void insereNoPai(Pagina* pai, Bplus* cabecalho, void* novo_dado, int indiceFilhoAntigo, int posNovoFilho);

/**
 * @brief Função responsável por realizar a cisão (split) de um nó interno com overflow, separando ponteiros e subindo a chave central.
 * @param pagina Ponteiro para o nó interno que transbordou na RAM.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @return Pagina* Ponteiro para o novo nó interno gerado à direita.
 */
Pagina* splitInterno(Pagina* pagina, Bplus* cabecalho);

/**
 * @brief Função responsável por remover uma chave da árvore, realizando rebalanceamento (redistribuição ou concatenação) se restarem chaves abaixo da quantidade mínima.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param dadoRemover Ponteiro para a chave genérica a ser removida.
 * @param ehMenor Callback de comparação entre chaves.
 * @param leituraDado Callback de leitura das chaves do disco.
 * @param escreveDado Callback de escrita das chaves no disco.
 * @return int Posição do registro no arquivo de dados principal que foi apagado, ou -1 se não encontrado.
 */
int removeDadoBplus(FILE* arquivo, Bplus* cabecalho, void* dadoRemover, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*));

/**
 * @brief Função responsável por atualizar a chave separadora de referência nos pais quando a primeira chave de uma página é alterada.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param novaChaveReferencia Ponteiro para a nova chave que deverá substituir a cópia antiga no pai ou ancestral.
 * @param posicoes_pais Vetor com as posições dos pais percorridos durante a busca.
 * @param indices_filhos Vetor com os índices dos filhos acessados em cada pai durante a busca.
 * @param alturaCaminho Quantidade de níveis armazenados no caminho percorrido até a folha.
 * @param leituraDado Callback de leitura das chaves do disco.
 * @param escreveDado Callback de escrita das chaves no disco.
 */
void atualizaChaveReferenciaPais(FILE* arquivo, Bplus* cabecalho, void* novaChaveReferencia, int posicoes_pais[], int indices_filhos[], int alturaCaminho, void (*leituraDado)(void*, FILE*), void (*escreveDado)(void*, FILE*));

/**
 * @brief Função responsável por concatenar duas páginas irmãs e remover a chave do pai quando o nó atinge o limite mínimo de elementos.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param pai Ponteiro para o nó pai na RAM.
 * @param indiceFilhoEsq Índice do ponteiro do irmão esquerdo no vetor do pai.
 * @param irmaoEsq Ponteiro para o nó irmão esquerdo na RAM.
 * @param irmaoDir Ponteiro para o nó irmão direito na RAM (que será logicamente liberado no disco).
 * @param escreveDado Callback para atualizar as páginas que restaram no disco.
 */
void concatenaBplus(FILE* arquivo, Bplus* cabecalho, Pagina* pai, int indiceFilhoEsq, Pagina* irmaoEsq, Pagina* irmaoDir, void (*escreveDado)(void*, FILE*));

/**
 * @brief Função responsável por tentar redistribuir as chaves emprestando de um irmão esquerdo ou direito para resolver um underflow sem concatenar.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param pagina Ponteiro para o nó em underflow na RAM.
 * @param posPai Posição no disco do nó pai.
 * @param indiceFilho Índice de ponteiro desta página no nó pai.
 * @param escreveDado Callback de escrita de chaves no disco.
 * @param leituraDado Callback de leitura de chaves no disco.
 * @return bool 'True' se a redistribuição foi feita, ou 'false' caso os irmãos não tenham chaves suficientes para emprestar.
 */
bool redistribui(FILE* arquivo, Bplus* cabecalho, Pagina* pagina, int posPai, int indiceFilho, void (*escreveDado)(void*, FILE*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Função responsável por imprimir a estrutura da árvore Bplus em disco.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param posAtual Índice da página por onde a impressão deve iniciar.
 * @param profundidade Nível atual da recursão para controle de formatação da árvore.
 * @param imprimeChave Callback para formatar e imprimir a chave genérica.
 * @param leituraDado Callback para ler as páginas do arquivo enquanto percorre a árvore.
 */
void imprimeArvoreBplus(FILE* arquivo, Bplus* cabecalho, int posAtual, int profundidade, void (*imprimeChave)(void*), void (*leituraDado)(void*, FILE*));

/**
 * @brief Função responsável por realizar a busca por intervalo, percorrendo as folhas encadeadas no disco entre dois dados.
 * @param arquivo Ponteiro para o arquivo da árvore.
 * @param cabecalho Ponteiro para o cabeçalho da árvore.
 * @param limite_inferior Ponteiro para a chave genérica que define o início do intervalo.
 * @param limite_superior Ponteiro para a chave genérica que define o final do intervalo.
 * @param ehMenor Callback de comparação entre chaves.
 * @param leituraDado Callback para leitura das páginas encadeadas do disco.
 * @param posRegistros Vetor de inteiros que receberá as posições (índices) dos registros encontrados no disco.
 * @param capacidade Quantidade máxima de registros que o vetor posRegistros consegue armazenar.
 * @return int Quantidade total de registros encontrados e armazenados dentro do intervalo.
 */
int buscaIntervaloBplus(FILE* arquivo, Bplus* cabecalho, void* limite_inferior, void* limite_superior, bool (*ehMenor)(void*, void*), void (*leituraDado)(void*, FILE*), int posRegistros[], int capacidade);