/**
 * @file Funcionario.c
 * @brief Arquivo do código fonte responsável por toda implementação de estrutura e funcionamento do tratamento e amarzenamento dos dados do funcionario.
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

void verificaData(Data* data) {
    bool dataValida = false;

    while (!dataValida) {

        // Validar ano
        while (data->ano < 1000 || data->ano > 2026) {
            printf("\n ERRO! O ano nao eh valido! Inserir novamente...\n");
            printf("Ano no formato (AAAA): ");
            scanf("%d", &data->ano);
            limpaBuffer();
        }

        // Validar Mês
        while (data->mes < 1 || data->mes > 12) {
            printf("\n ERRO! O mes nao eh valido! Inserir novamente...\n");
            printf("Mes no formato (MM): ");
            scanf("%d", &data->mes);
            limpaBuffer();
        }

        // Validar Dia baseado no mês e ano
        int diasNoMes = 31;

        if (data->mes == 4 || data->mes == 6 || data->mes == 9 || data->mes == 11) {
            diasNoMes = 30;
        } else if (data->mes == 2) {
            
            // Verifica se o ano e bissexto
            if ((data->ano % 4 == 0 && data->ano % 100 != 0) || data->ano % 400 == 0) {
                diasNoMes = 29;
            } else {
                diasNoMes = 28;
            }
        }

        if (data->dia < 1 || data->dia > diasNoMes) {
            printf("\n ERRO! O dia nao eh valido para este mes/ano! Inserir novamente...\n");
            printf("Dia no formato (DD): ");
            scanf("%d", &data->dia);
            limpaBuffer();

            // Como o dia mudou, reiniciamos o laço para garantir que a data é valida
            dataValida = false;

        } else {
            dataValida = true;
        }
    }
}

bool dataEhMenor(Data dataA, Data dataB) {
    if (dataA.ano < dataB.ano) return true;
    if (dataA.ano > dataB.ano) return false;

    if (dataA.mes < dataB.mes) return true;
    if (dataA.mes > dataB.mes) return false;
    
    if (dataA.dia < dataB.dia) return true;
    if (dataA.dia > dataB.dia) return false;

    return false;
}

bool ehMenorDadoBusca(void* dado1, void* dado2) {
    dadoBusca* dado1_busca = (dadoBusca*) dado1;
    dadoBusca* dado2_busca = (dadoBusca*) dado2;

    int comparacao = strcmp(dado1_busca->nome, dado2_busca->nome);

    if (comparacao < 0)
        return true;

    if (comparacao > 0)
        return false;

    // desempata pela data de nascimento
    return dataEhMenor(dado1_busca->dataNascimento, dado2_busca->dataNascimento);
}

void escreveDadoBusca(void *dado, FILE *arquivo) {
    dadoBusca *d = (dadoBusca*)dado;
    fwrite(d->nome, sizeof(char), 100, arquivo);
    fwrite(&d->dataNascimento, sizeof(Data), 1, arquivo);
    fflush(arquivo);
}

void leituraDadoBusca(void *dado, FILE *arquivo) {
    dadoBusca *d = (dadoBusca*)dado;
    fread(d->nome, sizeof(char), 100, arquivo);
    fread(&d->dataNascimento, sizeof(Data), 1, arquivo);
}

void imprimeDadosFuncionario(void *dado) {
    if (dado == NULL) {
        printf("\nERRO: Registro de funcionario vazio ou inexistente!\n");
        return;
    }

    Funcionario *f = (Funcionario*)dado;

    printf("\n========================================================\n");
    printf("                   FICHA DO FUNCIONARIO                 \n");
    printf("========================================================\n");
    printf("Posicao no Arquivo (posRegistro): %d\n", f->posRegistro);
    printf("Status no Disco: %s\n", f->registroAtivo ? "Ativo" : "Logicamente Excluido");
    printf("Situacao na Empresa: %s\n", f->atividade ? "Trabalhando (Contrato Ativo)" : "Desligado");
    printf("--------------------------------------------------------\n");
    printf("Nome: %s\n", f->nome);
    printf("Data de Nascimento: %02d/%02d/%04d\n", f->nascimento.dia, f->nascimento.mes, f->nascimento.ano);
    printf("Telefone: %s\n\n", f->telefone);
    printf("Filiacao:\n");
    printf("Mae: %s\n", f->nomeMae);
    printf("pai: %s\n\n", f->nomePai);
    printf("Endereço Residencial:\n");
    printf("Rua: %s, Nº: %d\n", f->residencia.rua, f->residencia.numero);
    printf("Bairro: %s\n\n", f->residencia.bairro);
    printf("Informacoes Contratuais:\n");
    printf("Data de Contratacao: %02d/%02d/%04d\n", f->contratacao.dia, f->contratacao.mes, f->contratacao.ano);
    
    // Imprime a data de desligamento somente se o funcionário tiver sido desligado
    if (!f->atividade) printf("Data de Desligamento: %02d/%02d/%04d\n", 
        f->desligamento.dia, f->desligamento.mes, f->desligamento.ano);
    
    else printf("Data de Desligamento: N/A (Contrato Ativo)\n\n");
    
    printf("\nHistorico de pagamentos:\n");

    if (f->qtdPagamentos == 0) printf("Nenhum pagamento cadastrado.\n");
    else {
        for (int i = 0; i < f->qtdPagamentos; i++) {
            printf("%dº pagamento mais recente: R$ %.2lf\n", 
                i + 1, f->historicoPagamentos[i]);
        }
    }
    printf("========================================================\n");
}

void imprimePrimeiroNomeDataFuncionario(void* dado) {
    if (dado == NULL) return;
    
    dadoBusca* funcionario = (dadoBusca*)dado;
    char primeiroNome[50];
    
    // Extrai apenas a primeira palavra da string 'nome'
    sscanf(funcionario->nome, "%s", primeiroNome); 
    printf("%s (%02d/%02d/%04d)", primeiroNome, funcionario->dataNascimento.dia, 
            funcionario->dataNascimento.mes, funcionario->dataNascimento.ano);
}

void imprimeChaveDadoBusca(void *dado) {
    if (dado == NULL) return;

    dadoBusca* d = (dadoBusca*)dado;
    printf("%s (%02d/%02d/%d)", d->nome, d->dataNascimento.dia, 
            d->dataNascimento.mes, d->dataNascimento.ano);
}

dadoBusca criaDadoBusca() {
    dadoBusca novo;
    memset(&novo, 0, sizeof(dadoBusca));
    
    printf("Nome: ");
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    Data dataNascimento;
    printf("Data de Nascimento no formato (DD/MM/AAAA): ");
    scanf("%d/%d/%d", &dataNascimento.dia, &dataNascimento.mes, &dataNascimento.ano);
    limpaBuffer();
    verificaData(&dataNascimento);
    novo.dataNascimento = dataNascimento;

    return novo;
}

Funcionario criaFuncionario(char nome[100], Data dataNascimento) {
    Funcionario novo;
    memset(&novo, 0, sizeof(Funcionario));
    novo.posRegistro = -1;
    novo.registroAtivo = true;
    novo.prox_pos_livre = -1;
    novo.qtdPagamentos = 0;
    
    strcpy(novo.nome, nome);
    novo.nascimento = dataNascimento;

    printf("Nome do pai: ");
    fgets(novo.nomePai, sizeof(novo.nomePai), stdin);
    novo.nomePai[strcspn(novo.nomePai, "\n")] = '\0';
    
    printf("Nome da mae: ");
    fgets(novo.nomeMae, sizeof(novo.nomeMae), stdin);
    novo.nomeMae[strcspn(novo.nomeMae, "\n")] = '\0';

    Endereco residencia;
    memset(&residencia, 0, sizeof(Endereco));
    printf("Rua da residencia: ");
    fgets(residencia.rua, sizeof(residencia.rua), stdin);
    residencia.rua[strcspn(residencia.rua, "\n")] = '\0';
    
    printf("Numero da residencia: ");
    scanf("%d", &residencia.numero);
    limpaBuffer();
    
    while (residencia.numero < 1) {
        printf("O numero da residencia eh invalido! Digite novamente...\n");
        printf("Numero da residencia: ");
        scanf("%d", &residencia.numero);
        limpaBuffer();
    }

    printf("Bairro da residencia: ");
    fgets(residencia.bairro, sizeof(residencia.bairro), stdin);
    residencia.bairro[strcspn(residencia.bairro, "\n")] = '\0';

    novo.residencia = residencia;

    printf("Telefone (11 algarismos, sem simbolos): ");
    fgets(novo.telefone, sizeof(novo.telefone), stdin);
    novo.telefone[strcspn(novo.telefone, "\n")] = '\0';

    Data dataContratacao;
    printf("Data de Contratacao no formato (DD/MM/AAAA): ");
    scanf("%d/%d/%d", &dataContratacao.dia, &dataContratacao.mes, &dataContratacao.ano);
    limpaBuffer();
    verificaData(&dataContratacao);
    novo.contratacao = dataContratacao;

    int statusContrato;
    printf("Status do contrato [1 - Ativo | 0 - Inativo]: ");
    scanf("%d", &statusContrato);
    limpaBuffer();

    while (statusContrato != 1 && statusContrato != 0){
        printf("ERRO! Entrada invalida, digite novamente...\n");
        printf("Status do contrato [1 - Ativo | 0 - Inativo]: ");
        scanf("%d", &statusContrato);
        limpaBuffer();
    }

    novo.atividade = (statusContrato == 1);

    if (novo.atividade) {
        novo.desligamento.ano = 0;
        novo.desligamento.mes = 0;
        novo.desligamento.dia = 0;
    }
    else {
        Data dataDesligamento;

        printf("Data de Desligamento no formato (DD/MM/AAAA): ");
        scanf("%d/%d/%d", &dataDesligamento.dia, &dataDesligamento.mes, &dataDesligamento.ano);
        limpaBuffer();

        while (dataEhMenor(dataDesligamento, novo.contratacao)) {
            printf("\nERRO! Data invalida para o desligamento! Deve ser apos a contratacao.\n");
            printf("Data de Desligamento no formato (DD/MM/AAAA): ");
            scanf("%d/%d/%d", &dataDesligamento.dia, &dataDesligamento.mes, &dataDesligamento.ano);
            limpaBuffer();
        }

        verificaData(&dataDesligamento);
        novo.desligamento = dataDesligamento;
    }

    memset(novo.historicoPagamentos, 0, sizeof(novo.historicoPagamentos));
    novo.qtdPagamentos = 0;

    int desejaCadastrarPagamento;

    printf("\nDeseja cadastrar pagamentos no historico? [1 - Sim | 0 - Nao]: ");
    scanf("%d", &desejaCadastrarPagamento);
    limpaBuffer();

    while (desejaCadastrarPagamento != 1 && desejaCadastrarPagamento != 0) {
        printf("Entrada invalida! Digite novamente.\n");
        printf("Deseja cadastrar pagamentos no historico? [1 - Sim | 0 - Nao]: ");
        scanf("%d", &desejaCadastrarPagamento);
        limpaBuffer();
    }

    if (desejaCadastrarPagamento == 1) {
        int quantidade;

        printf("Quantos pagamentos deseja cadastrar? [0 a 12]: ");
        scanf("%d", &quantidade);
        limpaBuffer();

        while (quantidade < 0 || quantidade > 12) {
            printf("Quantidade invalida! Digite um valor entre 0 e 12.\n");
            printf("Quantos pagamentos deseja cadastrar? [0 a 12]: ");
            scanf("%d", &quantidade);
            limpaBuffer();
        }

        for (int i = 0; i < quantidade; i++) {
            double valor;

            printf("Valor do pagamento %d: R$ ", i + 1);
            scanf("%lf", &valor);
            limpaBuffer();

            while (valor <= 0) {
                printf("Valor invalido! Digite um pagamento positivo: R$ ");
                scanf("%lf", &valor);
                limpaBuffer();
            }

            adicionaPagamento(&novo, valor);
        }
    }

    return novo;
}

void inicializaArquivoRegistros(FILE* arquivo) {
    Registros cabecalho;
    memset(&cabecalho, 0, sizeof(Registros));

    cabecalho.primeira_pos_livre = -1;
    cabecalho.qtd_registros = 0;

    rewind(arquivo);
    fwrite(&cabecalho, sizeof(Registros), 1, arquivo);
}

Registros* leituraCabecalhoRegistros(FILE* arquivo) {
    verificaArquivo(arquivo);

    Registros* novo = mallocSafe(sizeof(Registros));
    rewind(arquivo);
    fread(novo, sizeof(Registros), 1, arquivo);

    return novo;
}

void escreveCabecalhoRegistros(FILE* arquivo, Registros* cabecalho) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL) return;

    rewind(arquivo);
    fwrite(cabecalho, sizeof(Registros), 1, arquivo);
    fflush(arquivo);
}

int proxRegistro(FILE* arquivo, Registros* cabecalho) {
    verificaArquivo(arquivo);

    if (cabecalho == NULL) return -1;

    int proxPosLivre;

    if (cabecalho->primeira_pos_livre < 0) {
        proxPosLivre = cabecalho->qtd_registros;
        cabecalho->qtd_registros++;
    }
    else {
        Funcionario* livre = mallocSafe(sizeof(Funcionario));
        fseek(arquivo, sizeof(Registros) + cabecalho->primeira_pos_livre * sizeof(Funcionario), SEEK_SET);
        fread(livre, sizeof(Funcionario), 1, arquivo);

        proxPosLivre = cabecalho->primeira_pos_livre;
        cabecalho->primeira_pos_livre = livre->prox_pos_livre;
        free(livre);
    }

    escreveCabecalhoRegistros(arquivo, cabecalho);
    return proxPosLivre;
}

void adicionaPagamento(Funcionario* funcionario, double valor) {
    if (funcionario == NULL) return;

    if (valor <= 0) {
        printf("\nERRO! O valor do pagamento deve ser positivo.\n");
        return;
    }

    // Caso em que o funcionário tenha menos de 12 pagamentos registrados
    if (funcionario->qtdPagamentos < 12) {
        for (int i = funcionario->qtdPagamentos; i > 0; i--) {
            funcionario->historicoPagamentos[i] = funcionario->historicoPagamentos[i - 1];
        }

        funcionario->historicoPagamentos[0] = valor;
        funcionario->qtdPagamentos++;
    } 
    else {

        // Caso o funcionário já tem 12 pagamentos registrados, então o mais antigo é descartado
        for (int i = 11; i > 0; i--) {
            funcionario->historicoPagamentos[i] = funcionario->historicoPagamentos[i - 1];
        }

        funcionario->historicoPagamentos[0] = valor;
    }
}
