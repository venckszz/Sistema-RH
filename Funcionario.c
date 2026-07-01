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

            // Como o dia mudou, reiniciamos o laço para garantir que a data e valida
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

bool ehMenorFuncionario(void *a, void *b) {
    Funcionario *f1 = (Funcionario *) a;
    Funcionario *f2 = (Funcionario *) b;

    int comparacao = strcmp(f1->nome, f2->nome);

    if (comparacao < 0)
        return true;

    if (comparacao > 0)
        return false;

    // desempata pela data de nascimento
    return dataEhMenor(f1->nascimento, f2->nascimento);
}

bool ehMenorDadoBusca(void* dado1, void* dado2) {
    dadoBusca* dado1_busca = (dadoBusca*) dado1;
    dadoBusca* dado2_busca = (dadoBusca*) dado2;
     
    if (!strcmp(dado1_busca->nome, dado2_busca->nome)) {
        return dataEhMenor(dado1_busca->dataNascimento, dado2_busca->dataNascimento);
    }
    
    return dado1_busca->nome < dado2_busca->nome;
}

void escreveFuncionario(void *dado, FILE *arquivo) {
    verificaArquivo(arquivo);
    
    if (dado == NULL) return;

    Funcionario *funcionario = (Funcionario *) dado;
    fwrite(funcionario, sizeof(Funcionario), 1, arquivo);
}

void leituraFuncionario(void *dado, FILE *arquivo) {
    verificaArquivo(arquivo);
    
    if (dado == NULL) return;

    Funcionario *funcionario = (Funcionario *) dado;
    fread(funcionario, sizeof(Funcionario), 1, arquivo);
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
    limpaBuffer();
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

    // Verifica se o funcionário possui idade para trabalhar
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