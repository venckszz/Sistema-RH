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
                Funcionario novoFuncionario = criaFuncionario();
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