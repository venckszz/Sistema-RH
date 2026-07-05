/**
 * @file Util.h
 * @brief Arquivo cabeçalho responsável por guardar todas as assinaturas de funções auxiliares utilizadas nos códigos fontes: Bplus e Funcionario.
 * @author Grupo 1: Jonathan Alves Bispo da Paz [2024200497], Leandro Brognoli Grazziotin [2024200523] e Victor da Rocha Toniato [2024200493].
 * @date 05/07/2026
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Aloca um bloco de memória na RAM de forma segura, abortando o programa em caso de falta de memória.
 * @param nbytes Quantidade de bytes a ser alocada via malloc.
 * @return void* Ponteiro genérico para a região de memória recém-alocada.
 */
void* mallocSafe(size_t nbytes);

/**
 * @brief Verifica se o ponteiro de um arquivo aberto é válido (diferente de NULL), abortando com erro caso contrário.
 * @param arquivo Ponteiro de arquivo a ser validado.
 */
void verificaArquivo(FILE* arquivo);

/**
 * @brief Calcula o deslocamento exato em bytes (offset) a partir do início do arquivo para saltar até uma página com fseek.
 * @details Leva em consideração o tamanho em bytes do cabeçalho da Árvore, o tamanho fixo da estrutura Pagina e o tamanho variável dos dados genéricos de acordo com a ORDEM.
 * @param posPagina Índice físico (posição/número da página) que se deseja acessar no disco.
 * @param tamanho_registro Tamanho em bytes da chave genérica configurada na árvore.
 * @return size_t Deslocamento total em bytes que deve ser feito.
 */
size_t calculaDeslocamentoPagina(int posPagina, size_t tamanho_registro);

/**
 * @brief Limpa o buffer de entrada do terminal, pegando os caracteres que restaram até a quebra de linha (\n) ou EOF.
 */
void limpaBuffer();


