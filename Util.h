/**
 * @file Util.h
 * @brief Arquivo cabeçalho responsável por guardar todas as assinaturas de funções auxiliares utilizadas nos códigos fontes: Bplus e Funcionario.
 * @author Grupo 1: Jonathan Alves Bispo da Paz [2024200497], Leandro Brognoli Grazziotin [2024200523] e Victor da Rocha Toniato [2024200493].
 * @date 05/07/2026
 */
#include <stdio.h>
#include <stdlib.h>

void* mallocSafe(size_t nbytes);

void verificaArquivo(FILE* arquivo);

size_t calculaDeslocamentoPagina(int posPagina, size_t tamanho_registro);

void limpaBuffer();


