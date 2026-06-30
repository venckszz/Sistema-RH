#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Bplus.h"
#include "Funcionario.h"
#include "Util.h"

void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

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

        // Validar Mes
        while (data->mes < 1 || data->mes > 12) {
            printf("\n ERRO! O mes nao eh valido! Inserir novamente...\n");
            printf("Mes no formato (MM): ");
            scanf("%d", &data->mes);
            limpaBuffer();
        }

        // Validar Dia baseado no mes e ano
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
            // Como o dia mudou, reiniciamos o laco para garantir que a data e valida
            dataValida = false;
        } else {
            dataValida = true;
        }
    }
}

bool dataEhMenor(Data a, Data b) {
    if (a.ano < b.ano) return true;
    if (a.ano > b.ano) return false;

    if (a.mes < b.mes) return true;
    if (a.mes > b.mes) return false;

    if (a.dia < b.dia) return true;
    if (a.dia > b.dia) return false;

    return false;
}

Funcionario criaFuncionario() {
    Funcionario novo;
    memset(&novo, 0, sizeof(Funcionario));
    
    printf("Nome: ");
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    Data dataNascimento;
    printf("Data de Nascimento no formato (DD/MM/AAAA): ");
    scanf("%d/%d/%d", &dataNascimento.dia, &dataNascimento.mes, &dataNascimento.ano);
    limpaBuffer(); // Limpa logo após o scanf
    verificaData(&dataNascimento);
    novo.nascimento = dataNascimento;

    printf("Nome do pai: ");
    fgets(novo.nomePai, sizeof(novo.nomePai), stdin);
    novo.nomePai[strcspn(novo.nomePai, "\n")] = '\0';
    
    printf("Nome da mae: ");
    fgets(novo.nomeMae, sizeof(novo.nomeMae), stdin);
    novo.nomeMae[strcspn(novo.nomeMae, "\n")] = '\0';

    Endereco residencia;
    printf("Rua da residencia: ");
    fgets(residencia.rua, sizeof(residencia.rua), stdin);
    residencia.rua[strcspn(residencia.rua, "\n")] = '\0';
    
    printf("Numero da residencia: ");
    scanf("%d", &residencia.numero);
    limpaBuffer(); // Limpa logo após o scanf
    
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
    limpaBuffer(); // Limpa logo apos o scanf
    verificaData(&dataContratacao);

    // Verifica se o funcionario possui idade para trabalhar
    Data maioridade;
    maioridade.dia = novo.nascimento.dia;
    maioridade.mes = novo.nascimento.mes;
    maioridade.ano = novo.nascimento.ano + 18;
    
    while (dataEhMenor(dataContratacao, maioridade)) {
        printf("\nERRO! O funcionario nao possui idade minima (18 anos) para trabalhar!\n");
        printf("Data de Contratacao (DD/MM/AAAA): ");
        scanf("%d/%d/%d", &dataContratacao.dia, &dataContratacao.mes, &dataContratacao.ano);
        limpaBuffer();
        verificaData(&dataContratacao);
    }
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
            printf("\n ERRO! Data invalida para o desligamento! Deve ser apos a contratacao. Inserir novamente...\n");
            printf("Data de Desligamento no formato (DD/MM/AAAA): ");
            scanf("%d/%d/%d", &dataDesligamento.dia, &dataDesligamento.mes, &dataDesligamento.ano);
            limpaBuffer();
        }

        verificaData(&dataDesligamento);
        novo.desligamento = dataDesligamento;
    }

    memset(novo.historicoPagamentos, 0, sizeof(novo.historicoPagamentos));

    return novo;
}