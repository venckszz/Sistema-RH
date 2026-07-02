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
    // Atenção: como dadoBusca tem char*, gravar o ponteiro direto é erro!
    // Grave o conteúdo do nome (fixo) e depois a data.
    fwrite(d->nome, sizeof(char), 100, arquivo); // Exemplo assumindo nome[100]
    fwrite(&d->dataNascimento, sizeof(Data), 1, arquivo);
}

void leituraDadoBusca(void *dado, FILE *arquivo) {
    dadoBusca *d = (dadoBusca*)dado;
    fread(d->nome, sizeof(char), 100, arquivo);
    fread(&d->dataNascimento, sizeof(Data), 1, arquivo);
}

void imprimeChaveDadoBusca(void *dado) {
    dadoBusca *d = (dadoBusca*)dado;
    printf("%s (%02d/%02d/%d)", d->nome, d->dataNascimento.dia, d->dataNascimento.mes, d->dataNascimento.ano);
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

void imprimeDadosFuncionario(void *dado) {
    if (dado == NULL) {
        printf("\nERRO: Registro de funcionário vazio ou inexistente!\n");
        return;
    }
    // Convertendo o ponteiro genérico para a estrutura completa do Funcionário
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
    printf("Filiação:\n");
    printf("Mãe: %s\n", f->nomeMae);
    printf("pai: %s\n\n", f->nomePai);
    printf("Endereço Residencial:\n");
    printf("Rua: %s, Nº: %d\n", f->residencia.rua, f->residencia.numero);
    printf("Bairro: %s\n\n", f->residencia.bairro);
    printf("Informações Contratuais:\n");
    printf("Data de Contratação: %02d/%02d/%04d\n", f->contratacao.dia, f->contratacao.mes, f->contratacao.ano);
    
    // Só imprime a data de desligamento se o funcionário realmente tiver sido desligado
    if (!f->atividade) {
        printf("Data de Desligamento: %02d/%02d/%04d\n", f->desligamento.dia, f->desligamento.mes, f->desligamento.ano);
    } else {
        printf("Data de Desligamento: N/A (Contrato Ativo)\n\n");
    }
    printf("Historico de Pagamentos (ultimos 12 meses):\n");
    for (int i = 0; i < 12; i++) {
        printf("  - Mês %02d: R$ %.2f\n", i + 1, f->historicoPagamentos[i]);
    }
    printf("========================================================\n");
}

void imprimeChaveBasicaFuncionario(void* dado) {
    if (dado == NULL) return;
    
    dadoBusca* funcionario = (dadoBusca*)dado;
    
    char primeiroNome[50];
    
    // Extrai apenas a primeira palavra da string 'nome'
    sscanf(funcionario->nome, "%s", primeiroNome); 
    
    printf("%s (%02d/%02d/%04d)", primeiroNome, funcionario->dataNascimento.dia, funcionario->dataNascimento.mes, funcionario->dataNascimento.ano);
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
    
    strcpy(novo.nome, nome);
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

void inicializaArquivoRegistros(FILE* arquivo) {
    Registros cabecalho;
    cabecalho.primeira_pos_livre = -1; // -1 indica que não há espaços livres
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