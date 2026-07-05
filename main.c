/**
 * @file main.c
 * @brief Código fonte principal responsável por promover o funcinamento do Sistema RH.
 * @author Grupo 1: Jonathan Alves Bispo da Paz [2024200497], Leandro Brognoli Grazziotin [2024200523] e Victor da Rocha Toniato [2024200493].
 * @date 05/07/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Bplus.h"
#include "Funcionario.h"
#include "Util.h"

int main(void) {

    int opcao;
    
    FILE* funcionarios = fopen("funcionario.bin", "rb+");
    if (funcionarios == NULL) {
        funcionarios = fopen("funcionario.bin", "wb+");
        inicializaArquivoRegistros(funcionarios);
    }
    
    Registros* cabecalhoFuncionarios = leituraCabecalhoRegistros(funcionarios);

    FILE* arvore = fopen("bplus.bin", "rb+");
    if (arvore == NULL) 
        arvore = fopen("bplus.bin", "wb+");
    
    Bplus* cabecalhoBplus = leituraCabecalhoBplus(arvore);
    if (cabecalhoBplus == NULL) {
        inicializaBplus(arvore, sizeof(dadoBusca));
        cabecalhoBplus = leituraCabecalhoBplus(arvore);
    }
    
    do {
        printf("\n");
        printf("+----------------------------------------------------+\n");
        printf("|             SISTEMA DE RECURSOS HUMANOS            |\n");
        printf("|                                                    |\n");
        printf("|  [ 1 ] INSERIR FUNCIONARIO                         |\n");
        printf("|  [ 2 ] BUSCAR FUNCIONARIO                          |\n");
        printf("|  [ 3 ] EXCLUIR FUNCIONARIO                         |\n");
        printf("|  [ 4 ] BUSCAR INTERVALO DE FUNCIONARIOS            |\n");
        printf("|  [ 5 ] IMPRIMIR ESTRUTURA DA ARVORE                |\n");
        printf("|  [ 6 ] SAIR                                        |\n");
        printf("|                                                    |\n");
        printf("+----------------------------------------------------+\n");
        printf("\nEntrada: ");

        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {

            case 1: {
                printf("\n========== Iniciando insercao de funcionario ==========\n");
                dadoBusca dado = criaDadoBusca();

                int indiceBusca = -1;
                bool encontrou = false;

                Pagina* paginaBuscada = buscaDadoBplus(arvore, cabecalhoBplus, &dado, &indiceBusca, &encontrou, ehMenorDadoBusca, leituraDadoBusca);

                if (encontrou) {
                    printf("\nO funcionario ja existe no sistema!\n");

                    int posBusca = paginaBuscada->posRegistro[indiceBusca];

                    Funcionario atual;
                    memset(&atual, 0, sizeof(Funcionario));
                    fseek(funcionarios, sizeof(Registros) + posBusca * sizeof(Funcionario), SEEK_SET);
                    fread(&atual, sizeof(Funcionario), 1, funcionarios);

                    printf("\nDados atuais do funcionario:\n");
                    imprimeDadosFuncionario(&atual);

                    int resposta;
                    printf("\nDeseja atualizar os dados? [1 - Sim | 0 - Nao]: ");
                    scanf("%d", &resposta);
                    limpaBuffer();

                    if (resposta == 1) {
                        Funcionario atualiza = criaFuncionario(dado.nome, dado.dataNascimento);

                        atualiza.posRegistro = posBusca;
                        atualiza.registroAtivo = true;
                        atualiza.prox_pos_livre = -1;

                        fseek(funcionarios, sizeof(Registros) + posBusca * sizeof(Funcionario), SEEK_SET);
                        fwrite(&atualiza, sizeof(Funcionario), 1, funcionarios);
                        fflush(funcionarios);

                        printf("\nFuncionario atualizado com sucesso!\n");
                    }

                    else printf("\nAtualizacao cancelada.\n");
                }
                
                else {
                    Funcionario novo = criaFuncionario(dado.nome, dado.dataNascimento);

                    novo.prox_pos_livre = -1;
                    novo.registroAtivo = true;
                    novo.posRegistro = proxRegistro(funcionarios, cabecalhoFuncionarios);

                    fseek(funcionarios, sizeof(Registros) + novo.posRegistro * sizeof(Funcionario), SEEK_SET);
                    fwrite(&novo, sizeof(Funcionario), 1, funcionarios);
                    escreveCabecalhoRegistros(funcionarios, cabecalhoFuncionarios);

                    int resultado = insereBplus(arvore, cabecalhoBplus, &dado, novo.posRegistro, ehMenorDadoBusca, escreveDadoBusca, leituraDadoBusca);

                    if (resultado == 0) printf("\nInsercao realizada com sucesso! Retornando ao menu...\n");
                
                    else if (resultado == 42) printf("\nErro: funcionario com mesmo nome e mesma data ja existe!\n");
                    else printf("\nErro ao inserir!\n");
                }

                liberaPaginaRAM(paginaBuscada);
                break;
            }

            case 2: {
                int indiceBusca = -1;
                bool encontrou = false;

                dadoBusca consulta;
                memset(&consulta, 0, sizeof(dadoBusca));

                printf("\n========== Iniciando Busca por Funcionario ==========\n");

                printf("Insira o nome completo do funcionario buscado: ");
                fgets(consulta.nome, sizeof(consulta.nome), stdin);

                consulta.nome[strcspn(consulta.nome, "\r\n")] = '\0';

                consulta.dataNascimento.dia = 1;
                consulta.dataNascimento.mes = 1;
                consulta.dataNascimento.ano = 1000;
                
                Pagina* paginaAtual = buscaDadoBplus(arvore, cabecalhoBplus, &consulta, &indiceBusca, &encontrou, ehMenorDadoBusca, leituraDadoBusca);

                // Vetores para armazenar temporariamente os homónimos encontrados nas folhas
                int posicoesEncontradas[50];
                dadoBusca chavesEncontradas[50];
                int qtdEncontrados = 0;
                
                // Percorre as chaves através da lista ordenada encadeada entre as folhas
                while (paginaAtual != NULL) {
                    for (int i = indiceBusca; i < paginaAtual->qtd_chaves_atuais; i++) {
                        dadoBusca* chaveAtual = (dadoBusca*)paginaAtual->chaves[i];
                        
                        // Verifica se o nome corresponde ao buscado
                        if (strcmp(chaveAtual->nome, consulta.nome) == 0) {
                            posicoesEncontradas[qtdEncontrados] = paginaAtual->posRegistro[i];
                            chavesEncontradas[qtdEncontrados] = *chaveAtual;
                            qtdEncontrados++;
                        } else {
                            // Como a árvore é ordenada, se o nome mudou, não há mais homónimos à frente
                            liberaPaginaRAM(paginaAtual);
                            paginaAtual = NULL;
                            break;
                        }
                    }
                    
                    // Se a página acabou e o nome ainda era igual, salta para o irmão direito
                    if (paginaAtual != NULL) {
                        int prox = paginaAtual->posProximo;
                        liberaPaginaRAM(paginaAtual);
                        
                        if (prox != -1) {
                            paginaAtual = leituraPagina(arvore, cabecalhoBplus, prox, leituraDadoBusca);
                            indiceBusca = 0; // Na nova folha, começamos a ler desde o índice 0
                        } else {
                            paginaAtual = NULL;
                        }
                    }
                }
                
                // Tratamento dos resultados da pesquisa
                if (qtdEncontrados == 0) {
                    printf("\nFuncionario '%s' nao encontrado no sistema!\n", consulta.nome);
                    break;
                }
                
                int escolha = 0;

                if (qtdEncontrados == 1) escolha = 0;

                else {
                    printf("\nForam encontrados %d funcionarios com o nome '%s':\n", qtdEncontrados, consulta.nome);

                    for (int i = 0; i < qtdEncontrados; i++) {
                        printf("[%d] ", i + 1); 
                        imprimeChaveDadoBusca(&chavesEncontradas[i]);
                        printf("\n");
                    }

                    Data dataEscolhida;
                    printf("Digite a data de nascimento desejada (DD/MM/AAAA): ");
                    scanf("%d/%d/%d", &dataEscolhida.dia, &dataEscolhida.mes, &dataEscolhida.ano);
                    limpaBuffer();
                    verificaData(&dataEscolhida);

                    escolha = -1;

                    for (int i = 0; i < qtdEncontrados; i++) {
                        if (chavesEncontradas[i].dataNascimento.dia == dataEscolhida.dia &&
                            chavesEncontradas[i].dataNascimento.mes == dataEscolhida.mes &&
                            chavesEncontradas[i].dataNascimento.ano == dataEscolhida.ano) {
                            escolha = i;
                            break;
                        }
                    }

                    if (escolha == -1) {
                        printf("\nNenhum funcionario com essa data foi encontrado.\n");
                        break;
                    }
                }
                
                // Carrega o registro completo do funcionario
                int posFinal = posicoesEncontradas[escolha];
                Funcionario ficha;
                
                fseek(funcionarios, sizeof(Registros) + posFinal * sizeof(Funcionario), SEEK_SET);
                fread(&ficha, sizeof(Funcionario), 1, funcionarios);
                
                // Exibe a ficha cadastral completa
                imprimeDadosFuncionario(&ficha);
                
                break;
            }

            case 3: {
                printf("\n========== Excluindo Funcionario ==========\n");

                dadoBusca consulta;
                memset(&consulta, 0, sizeof(dadoBusca));

                printf("Digite o nome completo do funcionario a excluir: ");
                fgets(consulta.nome, sizeof(consulta.nome), stdin);
                consulta.nome[strcspn(consulta.nome, "\r\n")] = '\0';

                consulta.dataNascimento.dia = 1;
                consulta.dataNascimento.mes = 1;
                consulta.dataNascimento.ano = 1000;

                int indiceBusca = -1;
                bool encontrou = false;

                Pagina* paginaAtual = buscaDadoBplus(arvore, cabecalhoBplus, &consulta, &indiceBusca, &encontrou, ehMenorDadoBusca, leituraDadoBusca);

                int posicoesEncontradas[50];
                dadoBusca chavesEncontradas[50];
                int qtdEncontrados = 0;

                while (paginaAtual != NULL) {
                    bool parar = false;

                    for (int i = indiceBusca; i < paginaAtual->qtd_chaves_atuais; i++) {
                        dadoBusca* chaveAtual = (dadoBusca*)paginaAtual->chaves[i];

                        if (strcmp(chaveAtual->nome, consulta.nome) == 0) {
                            posicoesEncontradas[qtdEncontrados] = paginaAtual->posRegistro[i];
                            chavesEncontradas[qtdEncontrados] = *chaveAtual;
                            qtdEncontrados++;
                        } 
                        
                        else {
                            parar = true;
                            break;
                        }
                    }

                    int prox = paginaAtual->posProximo;
                    liberaPaginaRAM(paginaAtual);

                    if (parar || prox == -1) paginaAtual = NULL;
                    
                    else {
                        paginaAtual = leituraPagina(arvore, cabecalhoBplus, prox, leituraDadoBusca);
                        indiceBusca = 0;
                    }
                }

                if (qtdEncontrados == 0) {
                    printf("\nNenhum funcionario encontrado com esse nome.\n");
                    break;
                }

                int escolhido = 0;

                if (qtdEncontrados == 1) consulta.dataNascimento = chavesEncontradas[0].dataNascimento;
                
                else {
                    printf("\nForam encontrados %d funcionarios com o nome '%s':\n", qtdEncontrados, consulta.nome);

                    for (int i = 0; i < qtdEncontrados; i++) {
                        printf("[%d] ", i + 1);
                        imprimeChaveDadoBusca(&chavesEncontradas[i]);
                        printf("\n");
                    }

                    printf("\nDigite a data de nascimento do funcionario que deseja remover (DD/MM/AAAA): ");
                    scanf("%d/%d/%d", &consulta.dataNascimento.dia, &consulta.dataNascimento.mes, &consulta.dataNascimento.ano);
                    limpaBuffer();
                    verificaData(&consulta.dataNascimento);

                    escolhido = -1;

                    for (int i = 0; i < qtdEncontrados; i++) {
                        if (strcmp(chavesEncontradas[i].nome, consulta.nome) == 0 &&
                            chavesEncontradas[i].dataNascimento.dia == consulta.dataNascimento.dia &&
                            chavesEncontradas[i].dataNascimento.mes == consulta.dataNascimento.mes &&
                            chavesEncontradas[i].dataNascimento.ano == consulta.dataNascimento.ano) {
                            escolhido = i;
                            break;
                        }
                    }

                    if (escolhido == -1) {
                        printf("\nNenhum funcionario com essa data foi encontrado.\n");
                        break;
                    }
                }

                Funcionario f;
                memset(&f, 0, sizeof(Funcionario));
                fseek(funcionarios, sizeof(Registros) + posicoesEncontradas[escolhido] * sizeof(Funcionario), SEEK_SET);
                fread(&f, sizeof(Funcionario), 1, funcionarios);

                printf("\nDados do funcionario selecionado:\n");
                imprimeDadosFuncionario(&f);

                int confirma;
                printf("\nConfirmar exclusao? [1 - Sim | 0 - Nao]: ");
                scanf("%d", &confirma);
                limpaBuffer();

                if (confirma != 1) {
                    printf("\nExclusao cancelada.\n");
                    break;
                }

                int posRemovida = removeDadoBplus(arvore, cabecalhoBplus, &consulta, ehMenorDadoBusca, leituraDadoBusca, escreveDadoBusca);

                if (posRemovida != -1) {
                    fseek(funcionarios, sizeof(Registros) + posRemovida * sizeof(Funcionario), SEEK_SET);
                    fread(&f, sizeof(Funcionario), 1, funcionarios);

                    f.registroAtivo = false;
                    f.prox_pos_livre = cabecalhoFuncionarios->primeira_pos_livre;
                    cabecalhoFuncionarios->primeira_pos_livre = posRemovida;

                    fseek(funcionarios, sizeof(Registros) + posRemovida * sizeof(Funcionario), SEEK_SET);
                    fwrite(&f, sizeof(Funcionario), 1, funcionarios);
                    fflush(funcionarios);

                    escreveCabecalhoRegistros(funcionarios, cabecalhoFuncionarios);

                    printf("\nFuncionario removido com sucesso.\n");
                } 

                else printf("\nErro ao remover funcionario da arvore.\n");

                break;
            }

            case 4: {
                printf("\n========== Iniciando busca de funcionarios por intervalo ==========\n");
                dadoBusca inf, sup;
                memset(&inf, 0, sizeof(dadoBusca));
                memset(&sup, 0, sizeof(dadoBusca));

                printf("Insira o nome do primeiro funcionario (limite inferior): ");
                fgets(inf.nome, sizeof(inf.nome), stdin);
                inf.nome[strcspn(inf.nome, "\r\n")] = '\0'; // Remove o \n (e o \r quando necessário)

                printf("Insira o nome do segundo funcionario (limite superior): ");
                fgets(sup.nome, sizeof(sup.nome), stdin);
                sup.nome[strcspn(sup.nome, "\r\n")] = '\0'; // Remove o \n (e o \r quando necessário)

                // Como o intervalo é fechado por nome, usa-se a menor data possível no limite inferior
                // e a maior data possível no limite superior, garantindo a inclusão de todos os homônimos.
                inf.dataNascimento.dia = 1;
                inf.dataNascimento.mes = 1;
                inf.dataNascimento.ano = 1000;

                sup.dataNascimento.dia = 31;
                sup.dataNascimento.mes = 12;
                sup.dataNascimento.ano = 9999;

                if (strcmp(inf.nome, sup.nome) <= 0) {
                    int posRegistros[1000];

                    int qtdEncontrados = buscaIntervaloBplus(arvore,cabecalhoBplus, &inf, &sup, ehMenorDadoBusca, leituraDadoBusca, posRegistros, 1000);

                    if (qtdEncontrados == 0) printf("\nNenhum funcionario encontrado no intervalo informado.\n");
                    
                    else {
                        printf("\nFuncionarios encontrados no intervalo fechado:\n");

                        for (int i = 0; i < qtdEncontrados; i++) {
                            Funcionario ficha;
                            memset(&ficha, 0, sizeof(Funcionario));

                            fseek(funcionarios, sizeof(Registros) + posRegistros[i] * sizeof(Funcionario), SEEK_SET);

                            if (fread(&ficha, sizeof(Funcionario), 1, funcionarios) == 1 && ficha.registroAtivo) {
                                imprimeDadosFuncionario(&ficha);
                                printf("\n--------------------------------------------------------\n");
                            }
                        }
                    }
                }

                else printf("\nErro: O nome deve ser alfabeticamente menor ou igual ao segundo.\n");

                break;
            }

            case 5: {
                printf("\n========== Estrutura da Arvore em Disco ==========\n");
                if (cabecalhoBplus->raiz == -1) printf("Arvore vazia!\n");

                else imprimeArvoreBplus(arvore, cabecalhoBplus, cabecalhoBplus->raiz, 0, imprimePrimeiroNomeDataFuncionario, leituraDadoBusca);
                
                break;
            }

            case 6: {
                printf("\nEncerrando o programa...\n");
                
                fechaBplus(arvore, cabecalhoBplus);
                fclose(funcionarios);
                free(cabecalhoBplus);
                free(cabecalhoFuncionarios);
                break;
            }
            
            default: {
                printf("Opcao invalida! Tente novamente...");
                break;
            }
        }
    } while (opcao != 6);

    return 0;
}