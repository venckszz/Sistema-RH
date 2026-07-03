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
    int posRegistro;
    bool registroAtivo;
    int prox_pos_livre;

    char nome[100];
    char nomePai[100];
    char nomeMae[100];
    char telefone[15];
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
    char nome[100];
    Data dataNascimento;
} dadoBusca;

typedef struct registros {
    int qtd_registros;
    int primeira_pos_livre;
} Registros;

void verificaData(Data* data);

bool dataEhMenor(Data a, Data b);

bool ehMenorDadoBusca(void* dado1, void* dado2);

void escreveDadoBusca(void *dado, FILE *arquivo);

void leituraDadoBusca(void *dado, FILE *arquivo);

void escreveFuncionario(void *dado, FILE *arquivo);

void leituraFuncionario(void *dado, FILE *arquivo);

void imprimeDadosFuncionario(void* dado);

void imprimePrimeiroNomeDataFuncionario(void* dado);

void imprimeChaveDadoBusca(void *dado);

dadoBusca criaDadoBusca();

Funcionario criaFuncionario(char nome[100], Data dataNascimento);

void inicializaArquivoRegistros(FILE* arquivo);

Registros* leituraCabecalhoRegistros(FILE* arquivo);

void escreveCabecalhoRegistros(FILE* arquivo, Registros* cabecalho);

int proxRegistro(FILE* arquivo, Registros* cabecalho);