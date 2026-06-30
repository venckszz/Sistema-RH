#include <stdio.h>
#include <string.h>
#include "Bplus.h"

/* ---------- Callbacks para uma chave simples: int ---------- */

int comparaInt(const void *a, const void *b) {
    int va = *(const int *)a;
    int vb = *(const int *)b;
    return (va > vb) - (va < vb); // retorna -1, 0 ou 1
}

size_t tamanhoInt(void) {
    return sizeof(int);
}

void escreveInt(const void *chave, void *buffer) {
    memcpy(buffer, chave, sizeof(int));
}

void leInt(const void *buffer, void *chaveDestino) {
    memcpy(chaveDestino, buffer, sizeof(int));
}

void liberaInt(void *chave) {
    // não há nada alocado dinamicamente para um int, então fica vazio
    (void)chave;
}

void imprimeInt(const void *chave) {
    printf("%d", *(const int *)chave);
}

void callbackIntervalo(const void *chave, long offset, void *contexto) {
    (void)contexto;
    printf("  -> chave=%d offsetRegistro=%ld\n", *(const int *)chave, offset);
}

/* ---------- Programa de teste ---------- */

int main(void) {
    printf("=== Etapa 1: abrir/criar arvore ===\n");
    Bplus *arvore = bplus_abrir(
        "teste.idx",
        4, // ordem pequena de propósito, pra forçar split com poucos elementos
        comparaInt,
        tamanhoInt,
        escreveInt,
        leInt,
        liberaInt
    );

    if (!arvore) {
        printf("ERRO: falha ao abrir a arvore.\n");
        return 1;
    }
    printf("Arvore aberta com sucesso.\n\n");

    printf("=== Etapa 2: insercoes simples (sem split esperado) ===\n");
    int chaves1[] = {10, 20, 5};
    for (int i = 0; i < 3; i++) {
        int r = bplus_inserir(arvore, &chaves1[i], 1000 + chaves1[i]);
        printf("Inserindo %d -> %s\n", chaves1[i], r ? "OK" : "JA EXISTIA");
    }
    printf("\nEstrutura apos insercoes simples:\n");
    bplus_imprimirEstrutura(arvore, imprimeInt);

    printf("\n=== Etapa 3: forcar split inserindo mais chaves ===\n");
    int chaves2[] = {15, 25, 30, 1, 7, 12, 50, 60};
    for (int i = 0; i < 8; i++) {
        int r = bplus_inserir(arvore, &chaves2[i], 1000 + chaves2[i]);
        printf("Inserindo %d -> %s\n", chaves2[i], r ? "OK" : "JA EXISTIA");
    }
    printf("\nEstrutura apos forcar splits (deve ter mais de 1 nivel):\n");
    bplus_imprimirEstrutura(arvore, imprimeInt);

    printf("\n=== Etapa 4: testar insercao duplicada ===\n");
    int duplicada = 20;
    int r = bplus_inserir(arvore, &duplicada, 9999);
    printf("Inserir %d de novo -> %s (esperado: JA EXISTIA)\n", duplicada, r ? "OK" : "JA EXISTIA");

    printf("\n=== Etapa 5: busca exata ===\n");
    int chavesBusca[] = {25, 999, 1};
    for (int i = 0; i < 3; i++) {
        long offset;
        int achou = bplus_buscar(arvore, &chavesBusca[i], &offset);
        if (achou)
            printf("Busca %d -> ENCONTRADO, offsetRegistro=%ld\n", chavesBusca[i], offset);
        else
            printf("Busca %d -> NAO ENCONTRADO\n", chavesBusca[i]);
    }

    printf("\n=== Etapa 6: busca por intervalo (5, 30) ===\n");
    int limiteA = 5, limiteB = 30;
    bplus_buscarIntervalo(arvore, &limiteA, &limiteB, callbackIntervalo, NULL);

    printf("\n=== Etapa 7: remocao simples ===\n");
    int remover1 = 7;
    int rr = bplus_remover(arvore, &remover1);
    printf("Removendo %d -> %s\n", remover1, rr ? "OK" : "NAO ENCONTRADO");

    printf("\n=== Etapa 8: remocao que pode forcar rebalanceamento ===\n");
    int removerVarios[] = {1, 5, 10, 12, 15};
    for (int i = 0; i < 5; i++) {
        int ok = bplus_remover(arvore, &removerVarios[i]);
        printf("Removendo %d -> %s\n", removerVarios[i], ok ? "OK" : "NAO ENCONTRADO");
    }
    printf("\nEstrutura apos remocoes (verifique rebalanceamento):\n");
    bplus_imprimirEstrutura(arvore, imprimeInt);

    printf("\n=== Etapa 9: confirmar que removidos nao sao mais encontrados ===\n");
    for (int i = 0; i < 5; i++) {
        long offset;
        int achou = bplus_buscar(arvore, &removerVarios[i], &offset);
        printf("Busca %d -> %s\n", removerVarios[i], achou ? "ENCONTRADO (ERRO!)" : "NAO ENCONTRADO (correto)");
    }

    printf("\n=== Etapa 10: fechar e reabrir, testando persistencia ===\n");
    bplus_fechar(arvore);

    arvore = bplus_abrir(
        "teste.idx",
        4,
        comparaInt,
        tamanhoInt,
        escreveInt,
        leInt,
        liberaInt
    );
    if (!arvore) {
        printf("ERRO: falha ao reabrir a arvore.\n");
        return 1;
    }

    int chaveTestePersistencia = 20;
    long offsetPersistencia;
    int achouPersistencia = bplus_buscar(arvore, &chaveTestePersistencia, &offsetPersistencia);
    printf("Busca %d apos reabrir -> %s\n", chaveTestePersistencia,
           achouPersistencia ? "ENCONTRADO (persistencia OK)" : "NAO ENCONTRADO (ERRO de persistencia)");

    bplus_fechar(arvore);

    printf("\n=== Fim dos testes ===\n");
    return 0;
}