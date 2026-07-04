/**
 * @file Funcionario.h
 * @brief Arquivo cabeçalho responsável por guardar todas as assinaturas de funções e definições de estruturas utilizadas no código fonte do Funcionario.
 * @author Grupo 1: Jonathan Alves Bispo da Paz [2024200497], Leandro Brognoli Grazziotin [2024200523] e Victor da Rocha Toniato [2024200493].
 * @date 05/07/2026
 */

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Estrutura que representa a data (dia/mes/ano)
 */
typedef struct data {
    int dia;
    int mes;
    int ano;    
} Data;

/**
 * @brief Estrutura que representa o endereço
 */
typedef struct endereco {
    char rua[100];
    int numero;
    char bairro[100];
} Endereco;

/**
 * @brief Estrutura que representa a ficha de um funcionário
 */
typedef struct funcionario {
    int posRegistro; /* Inteiro que representa o posição do registro de funcionário no disco */
    bool registroAtivo; /* Booleano que indica se o registro do funcionário (índice) está ativo logicamente no disco */
    int prox_pos_livre; /* Inteiro que indica a próxima posicão (indice) livre no disco */

    char nome[100];
    char nomePai[100];
    char nomeMae[100];
    char telefone[15];
    Endereco residencia;
    Data nascimento;
    Data contratacao;
    Data desligamento;
    bool atividade; /* Booleano que indica o funcionário está ativo na empresa */

    double historicoPagamentos[12]; /* Vetor que armazena em cada índice o pagamento referente àquele mês */
    int qtdPagamentos; /* Inteiro que indica a quantidade de pagamentos realizados para o funcionário */
} Funcionario;

/**
 * @brief Estrutura que representa os dados do funcionário utilizados para busca
 */
typedef struct dadoBusca {
    char nome[100];
    Data dataNascimento;
} dadoBusca;

/**
 * @brief Estrutura que representa o cabeçalho do arquivo que armazena os dados de funcionários
 */
typedef struct registros {
    int qtd_registros; /* Inteiro que indica a quantidade de registros existentes no arquivo */
    int primeira_pos_livre; /* Inteiro que indica a próxima posicão (indice) livre no arquivo de registros */
} Registros;

/**
 * @brief Função responsável pela validação da data informada.
 * @param data Ponteiro para variável do tipo data (dia;mes;ano).
 */
void verificaData(Data* data);

/**
 * @brief Função responsável compara duas datas.
 * @param dataA Primeira data fornecida para a comparação.
 * @param dataB Segunda data fornecida para a comparação.
 * @return bool 'true' se a primeira data fornecida é menor que a segunda, caso contrário 'false'.
 */
bool dataEhMenor(Data dataA, Data dataB);

/**
 * @brief Função callback responsável por comparar dois dados(dadoBusca) fornecidos para busca de um funcionário.
 * @details A função analisa o nome e data de nascimento do funcionario. Faz a primeira comparação por nome e, caso sejam os nomes sejam iguais, uma comparação por data é realizada.
 * @param dado1 Ponteiro para o primeira dado fornecida para a comparação.
 * @param dado2 Ponteiro para o segundo dado fornecida para a comparação.
 * @return bool 'true' se a primeira dado fornecido é menor que a segunda e 'false' caso seja maior.
 */
bool ehMenorDadoBusca(void* dado1, void* dado2);

/**
 * @brief Função callback responsável por escrever um dado(dadoBusca) no arquivo.
 * @details A função analisa o nome e a data de nascimento do funcionário e os escreve no disco.
 * @param dado Ponteiro para o dado que será escrito.
 * @param arquivo Ponteiro para o arquivo em que será escrito o dado.
 */
void escreveDadoBusca(void *dado, FILE *arquivo);

/**
 * @brief Função callback responsável por ler um dado(dadoBusca) do disco.
 * @details A função pega o nome e a data de nascimento do funcionário do arquivo e os armazena no dadoBusca.
 * @param dado Ponteiro para o dado onde será armazenado.
 * @param arquivo Ponteiro para o arquivo em que será lido o dado.
 */
void leituraDadoBusca(void *dado, FILE *arquivo);

/**
 * @brief Função callback responsável por imprimir a ficha de dados do funcionário
 * @param dado Ponteiro para os dados do funcionário que será impresso.
 */
void imprimeDadosFuncionario(void* dado);

/**
 * @brief Função callback responsável por imprimir o primeiro nome e a data de nascimento do funcionário
 * @param dado Ponteiro para os dados do funcionário que será impresso.
 */
void imprimePrimeiroNomeDataFuncionario(void* dado);

/**
 * @brief Função callback responsável por imprimir o nome completo e a data de nascimento do funcionário
 * @param dado Ponteiro para os dados do funcionário que será impresso.
 */
void imprimeChaveDadoBusca(void *dado);

/**
 * @brief Função responsável por criar um dado(dadoBusca) de um funcionário.
 * @details A função pega o nome e a data de nascimento do funcionário (componentes do dadoBusca) fornecido e preenche dadoBusca.
 */
dadoBusca criaDadoBusca();

/**
 * @brief Função responsável por criar o registro de funcionário e preencher com os respectivos dados.
 * @param nome Vetor para armazenar o nome do funcionário.
 * @param dataNascimento Variável para armazenar a data de nascimento do funcionário.
 * @return Funcionario O funcionário preenchido com todos os dados.
 */
Funcionario criaFuncionario(char nome[100], Data dataNascimento);

/**
 * @brief Função responsável por criar, inicializar e gravar o cabeçalho do arquivo de registros de funcionários no disco.
 * @details A função separa o espaço para o cabeçalho em arquivos vazios. 
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 */
void inicializaArquivoRegistros(FILE* arquivo);

/**
 * @brief Função responsável por ler o cabeçalho do arquivo de registros de funcionários.
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 * @return Registros* Ponteiro para o cabeçalho do arquivo de registros de funcionários.
 */
Registros* leituraCabecalhoRegistros(FILE* arquivo);

/**
 * @brief Função responsável por escrever o cabeçalho modificado no arquivo de registros de funcionários.
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 * @param cabecalho Ponteiro para o cabeçalho já modificado que será escrito.
 */
void escreveCabecalhoRegistros(FILE* arquivo, Registros* cabecalho);

/**
 * @brief Função responsável por indicar o espaço onde o próximo registro de funcionário será inserido.
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 * @param cabecalho Ponteiro para o cabeçalho já modificado que será escrito.
 * @return int A posição no arquivo onde o registro pode ser inserido
 */
int proxRegistro(FILE* arquivo, Registros* cabecalho);

/**
 * @brief Função responsável por preencher os pagamentos mensais do funcionário.
 * @param funcionario Ponteiro para o funcionário que terá os pagamentos preencehidos.
 * @param valor Valor que corresponde o salário do funcionário
 */
void adicionaPagamento(Funcionario* funcionario, double valor);