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
    char nomePai[100];
    char nomeMae[100];
    char telefone[15];
    double salario;
    Endereco residencia;
    Data nascimento;
    Data contratacao;
    Data desligamento;
    bool atividade;

    // Vetor que armazena em cada índice o pagamento referente àquele mês
    double historicoPagamentos[12];
} Funcionario;

// Usado para busca de funcionários
typedef struct dadoBusca {
    char* nome;
    Data dataNascimento;
} dadoBusca;

void verificaData(Data* data);

bool dataEhMenor(Data a, Data b);

bool ehMenorFuncionario(void *a, void *b);

void escreveFuncionario(void *dado, FILE *arquivo);

void leituraFuncionario(void *dado, FILE *arquivo);

Funcionario criaFuncionario();