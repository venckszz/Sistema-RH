#include "Funcionario.h"

typedef struct dadoBusca {

    char* nome;
    Data dataNascimento;
} dadoBusca;


bool ehMenorDadoBusca(void** dado1, void** dado2);