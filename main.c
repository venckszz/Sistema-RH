#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Bplus.h"
#include "Funcionario.h"
#include "Util.h"

int main(void) {

    int opcao, aux = 0;
    
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
        abreBplus(arvore, sizeof(dadoBusca));
        cabecalhoBplus = leituraCabecalhoBplus(arvore);
    }
    
    do {
        printf("------------------------------------------------------\n");
        printf("|             SISTEMA DE RECURSOS HUMANOS            |\n");
        printf("|                                                    |\n");
        printf("|  [ 1 ] INSERIR FUNCIONARIO                         |\n");
        printf("|  [ 2 ] BUSCAR FUNCIONARIO                          |\n");
        printf("|  [ 3 ] EXCLUIR FUNCIONARIO                         |\n");
        printf("|  [ 4 ] BUSCAR INTERVALO DE FUNCIONARIOS            |\n");
        printf("|  [ 5 ] IMPRIMIR ESTRUTURA DA ARVORE                |\n");
        printf("|  [ 6 ] SAIR                                        |\n");
        printf("|                                                    |\n");
        printf("------------------------------------------------------\n");
        printf("\nEntrada: ");

        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {

            case 1: {
                printf("\n=== Iniciando insercao de funcionario ===\n");
                dadoBusca dado = criaDadoBusca();

                int indiceBusca = -1;
                bool encontrou = false;

                Pagina* paginaBuscada = buscaDadoBplus(arvore, cabecalhoBplus, &dado, &indiceBusca, &encontrou, ehMenorDadoBusca, leituraDadoBusca);

                if (encontrou) {
                    printf("\nO funcionario buscado ja esta cadastrado no sistema!\n");

                    printf("Deseja atualizar as informações do funcionario? [1 - Sim | 2 - Não]: ");
                    int resposta;
                    scanf("%d", &resposta);

                    
                    while (resposta != 1 && resposta != 2) {
                        printf("Resposta invalida! Digite novamente [1 - Sim | 2 - Não]: ");
                        scanf("%d", &resposta);
                    }

                    if (resposta == 1) {
                        int posBusca = paginaBuscada->posRegistro[indiceBusca];

                        Funcionario atualiza = criaFuncionario(dado.nome, dado.dataNascimento);

                        fseek(funcionarios, sizeof(Registros) + posBusca * sizeof(Funcionario), SEEK_SET);
                        fwrite(&atualiza, sizeof(Funcionario), 1, funcionarios);
                    }
                    else {
                        printf("\nEncerrando a insercao de funcionario...\n");
                    }
                    
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

                printf("\n=== Iniciando Busca por Funcionario ===\n");

                printf("Insira o nome do funcionario buscado: ");
                fgets(consulta.nome, sizeof(consulta.nome), stdin);

                consulta.nome[strcspn(consulta.nome, "\n")] = '\0';

                consulta.dataNascimento.dia = 1;
                consulta.dataNascimento.mes = 1;
                consulta.dataNascimento.ano = 1000;
                
                Pagina* paginaAtual = buscaDadoBplus(arvore, cabecalhoBplus, &consulta, &indiceBusca, &encontrou, ehMenorDadoBusca, leituraDadoBusca);

                // Vetores para armazenar temporariamente os homónimos encontrados nas folhas
                int posicoesEncontradas[50];
                dadoBusca chavesEncontradas[50];
                int qtdEncontrados = 0;
                
                // 4. Varredura horizontal pelas folhas (aproveitando a lista encadeada da Árvore B+)
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
                if (qtdEncontrados == 1) {
                    escolha = 0; // Só existe um funcionário com este nome
                } else {
                    // Cenário com homónimos: lista as opções para desempate
                    printf("\nForam encontrados %d funcionarios com o nome '%s':\n", qtdEncontrados, consulta.nome);
                    for (int i = 0; i < qtdEncontrados; i++) {
                        printf("[%d] ", i + 1); 
                        imprimeChaveDadoBusca(&chavesEncontradas[i]);
                        printf("\n");
                    }
                    
                    printf("Selecione o numero do funcionario que deseja visualizar: ");
                    scanf("%d", &escolha);
                    limpaBuffer();
                    escolha--; // Ajusta do formato 1..N para o índice do vetor 0..N-1
                    
                    if (escolha < 0 || escolha >= qtdEncontrados) {
                        printf("\nOpcao invalida! Retornando ao menu...\n");
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
                printf("\n=== Excluindo Funcionario ===\n");
                dadoBusca consulta;
                memset(&consulta, 0, sizeof(dadoBusca));
                printf("Digite o nome do funcionario a excluir: ");
                limpaBuffer();
                fgets(consulta.nome, sizeof(consulta.nome), stdin);
                consulta.nome[strcspn(consulta.nome, "\n")] = '\0';

                consulta.dataNascimento.dia = 1;
                consulta.dataNascimento.mes = 1;
                consulta.dataNascimento.ano = 1000;

                int indice;
                bool encontrou;

                // Busca a primeira ocorrência na árvore
                Pagina* p = buscaDadoBplus(arvore, cabecalhoBplus, &consulta, &indice, &encontrou, ehMenorDadoBusca, leituraDadoBusca);
                
                if (encontrou) {
                    printf("\nForam encontrados os seguintes registros para '%s':\n", consulta.nome);
                    
                    // --- Listagem de homônimos ---
                    Pagina* atual = p;
                    int i = indice;
                    bool continuarBusca = true;
                    int cont = 0;
                    
                    // Loop para listar todos os homônimos
                    while (atual != NULL) {

                        for (; i < atual->qtd_chaves_atuais; i++) {
                            dadoBusca* chave = (dadoBusca*)atual->chaves[i];
                            if (strcmp(chave->nome, consulta.nome) == 0) {
                                Funcionario f;
                                fseek(funcionarios, sizeof(Registros) + atual->posRegistro[i] * sizeof(Funcionario), SEEK_SET);
                                fread(&f, sizeof(Funcionario), 1, funcionarios);
                                printf("[%d] Nascimento: %02d/%02d/%d\n", cont++, f.nascimento.dia, f.nascimento.mes, f.nascimento.ano);
                            } else {
                                break;
                            }
                        }

                        int proximaPos = atual->posProximo;
                        liberaPaginaRAM(atual);

                        if (proximaPos != -1) {
                            atual = leituraPagina(arvore, cabecalhoBplus, proximaPos, leituraDadoBusca); // Função que lê página pelo índice
                            i = 0;
                        } 
                        else atual = NULL;
                    }

                    printf("\nDigite a data de nascimento do registro que deseja remover (DD MM AAAA): ");
                    scanf("%d %d %d", &consulta.dataNascimento.dia, &consulta.dataNascimento.mes, &consulta.dataNascimento.ano);
                    
                    // Nota: A função removeDadoBplus deve usar o nome + data agora preenchida para localizar o nó exato
                    int posRemovida = removeDadoBplus(arvore, cabecalhoBplus, NULL, &consulta, ehMenorDadoBusca, leituraDadoBusca, escreveDadoBusca);
                    
                    if (posRemovida != -1) {
                        Funcionario f;
                        fseek(funcionarios, sizeof(Registros) + posRemovida * sizeof(Funcionario), SEEK_SET);
                        fread(&f, sizeof(Funcionario), 1, funcionarios);
                        f.registroAtivo = false;
                        f.prox_pos_livre = cabecalhoFuncionarios->primeira_pos_livre;
                        cabecalhoFuncionarios->primeira_pos_livre = posRemovida;
                        fseek(funcionarios, sizeof(Registros) + posRemovida * sizeof(Funcionario), SEEK_SET);
                        fwrite(&f, sizeof(Funcionario), 1, funcionarios);
                        fflush(funcionarios);
                        escreveCabecalhoRegistros(funcionarios, cabecalhoFuncionarios);
                        printf("Funcionario removido com sucesso.\n");
                    }

                } else {
                    liberaPaginaRAM(p);
                    printf("Nenhum funcionario encontrado com esse nome.\n");
                }

                break;
            }

            case 4: {
                printf("\n=== Iniciando busca de funcionarios por intervalo ===\n");
                dadoBusca inf, sup;
                memset(&inf, 0, sizeof(dadoBusca));
                memset(&sup, 0, sizeof(dadoBusca));

                printf("Insira o nome do primeiro funcionario (limite inferior): ");
                limpaBuffer();
                fgets(inf.nome, sizeof(inf.nome), stdin);
                inf.nome[strcspn(inf.nome, "\n")] = '\0'; // Remove o \n

                printf("Insira o nome do segundo funcionario (limite superior): ");
                fgets(sup.nome, sizeof(sup.nome), stdin);
                sup.nome[strcspn(sup.nome, "\n")] = '\0'; // Remove o \n

                //Inserção de data qualquer para iniciar a comparação
                inf.dataNascimento.dia = inf.dataNascimento.mes = 1; 
                inf.dataNascimento.ano = 1000;
                sup.dataNascimento.dia = sup.dataNascimento.mes = 1; 
                sup.dataNascimento.ano = 1000;

                if (strcmp(inf.nome, sup.nome) <= 0) {
                    printf("\nFuncionarios encontrados no intervalo:\n");
                    buscaIntervaloBplus(arvore, cabecalhoBplus, &inf, &sup, ehMenorDadoBusca, imprimePrimeiroNomeDataFuncionario, leituraDadoBusca);
                } 
                else printf("\nErro: O nome deve ser alfabeticamente menor ou igual ao segundo.\n");

                break;
            }

            case 5: {
                printf("\n--- Estrutura da Arvore em Disco ---\n");
                if (cabecalhoBplus->raiz == -1) printf("Arvore vazia!\n");

                else imprimeArvoreBplus(arvore, cabecalhoBplus, cabecalhoBplus->raiz, 0, imprimePrimeiroNomeDataFuncionario, leituraDadoBusca);
                
                break;
            }

            case 6: {
                printf("Encerrando o programa...\n");
                free(cabecalhoBplus);
                free(cabecalhoFuncionarios);
                fclose(funcionarios);
                fclose(arvore);
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