#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "dadoBusca.h"
#include "Funcionario.h"

bool ehMenorDadoBusca(void* dado1, void* dado2) {

    dadoBusca* dado1_busca = (dadoBusca*) dado1;
    dadoBusca* dado2_busca = (dadoBusca) dado2;
    
    if (strcmp(dado1_busca->nome, dado2_busca->nome)) {
        return dataEhMenor(dado1_busca, dado2_busca);
    }

    return dado1_busca->nome < dado2_busca->nome;
}