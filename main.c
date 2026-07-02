#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Bplus.h"
#include "Funcionario.h"

int main(void) {

    int opcao, aux = 0;

    do {
        printf("\n =============== Menu de opcoes ============== \n");
        printf("1 - Inserir funcionario\n");
        printf("2 - Buscar funcionario\n");
        printf("3 - Excluir funcionario\n");
        printf("4 - Listagem por intervalo (Busca por Intervalo)\n");
        printf("5 - Exibir estrutura do indice\n");
        printf("6 - Sair\n");
        printf("================================================");
        printf("Entrada: ");

        scanf("%d", &opcao);
        getchar();

        switch (opcao) {

            case 1:
                printf("\n=== Iniciando o modulo de insercao de funcionario ===\n");
                dadoBusca dado = criaDadoBusca();
                FILE* funcionarios = fopen("funcionario.bin", "rb+");
                
                if (funcionarios == NULL) {
                    funcionarios = fopen("funcionario.bin", "wb+");
                    inicializaArquivoRegistros(funcionarios);
                }

                Registros* cabecalhoFuncionarios = leituraCabecalhoRegistros(funcionarios);

                FILE* arvore = fopen("bplus.bin", "rb+");
                if (arvore == NULL) arvore = fopen("bplus.bin", "wb+");

                Bplus* cabecalhoBplus = leituraCabecalhoBplus(arvore);

                if (cabecalhoBplus == NULL) {
                    abreBplus(arvore, sizeof(dadoBusca));
                    cabecalhoBplus = leituraCabecalhoBplus(arvore);
                }

                int indiceBusca = -1;
                bool encontrou = false;

                Pagina* pagBusca = buscaDadoBplus(arvore, cabecalhoBplus, &dado, &indiceBusca, &encontrou, ehMenorDadoBusca, leituraDadoBusca);

                if (encontrou) {
                    printf("\nO funcionário já existe no sistema!\n");
                    liberaPaginaRAM(pagBusca);
                }
                else {
                    Funcionario novoFuncionario = criaFuncionario(dado.nome, dado.dataNascimento);
                    fseek(registros, 0, SEEK_END)
                }
                break;
            
            case 2:
                
                break;
            
            case 3:
                
                break;
                
            case 4:
                
                break;

            case 5:

                break;
            
            case 6:
            
                break;
            
            default:

                break;
        }
    } while (opcao != 6);

    return 0;
}