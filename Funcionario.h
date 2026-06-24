typedef struct data {
    int dia;
    int mes;
    int ano;    
} Data;

typedef struct endereco {
    char rua[100];
    int numero;
    char bairro[100];
} Endereco;

typedef struct funcionario {
    char nome[100];

    Data nascimento;

    char nomePai[100];
    char nomeMae[100];

    Endereco residencia;
    char telefone[15];

    Data contratacao;
    bool atividade;
    Data desligamento;

    // Vetor que armazena em cada índice o pagamento referente àquele mês
    double historicoPagamentos[12];
} Funcionario;

void limpaBuffer();

void verificaData(Data* data);

bool dataEhMenor(Data a, Data b);

Funcionario criaFuncionario();