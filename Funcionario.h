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
    int posRegistro; /**< Inteiro que representa a posição física (índice) do registro de funcionário no disco */
    bool registroAtivo; /**< Booleano que indica se o registro do funcionário (índice) está ativo ou se foi logicamente excluído no disco */
    int prox_pos_livre; /**< Inteiro que indica a próxima posicão (indice) livre no disco */

    char nome[100];
    char nomePai[100];
    char nomeMae[100];
    char telefone[15];
    Endereco residencia;
    Data nascimento;
    Data contratacao;
    Data desligamento;
    bool atividade; /**< Booleano que indica o funcionário está ativo em função na empresa */

    double historicoPagamentos[12]; /**< Vetor que armazena o histórico dos últimos até 12 meses de salários */
    int qtdPagamentos; /**< Inteiro que indica a quantidade de pagamentos realizados para o funcionário */
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
    int qtd_registros; /**< Inteiro que indica a quantidade de registros existentes no arquivo */
    int primeira_pos_livre; /**< Inteiro que indica a próxima posicão (indice) livre no arquivo de registros */
} Registros;

/**
 * @brief Função responsável pela validação da data informada pelo usuário.
 * @param data Ponteiro para variável do tipo data (dia, mês e ano) que será validada.
 */
void verificaData(Data* data);

/**
 * @brief Função responsável por comparar duas datas.
 * @param dataA Primeira data fornecida para a comparação.
 * @param dataB Segunda data fornecida para a comparação.
 * @return bool 'True' se a primeira data (dataA) fornecida é anterior à segunda (dataB), caso contrário 'false'.
 */
bool dataEhMenor(Data dataA, Data dataB);

/**
 * @brief Função callback responsável por comparar dois dados de busca (dadoBusca: Nome e Nascimento) fornecidos para busca de um funcionário.
 * @details A função analisa o nome e data de nascimento do funcionario. Primeiramente, uma comparação por nome é feita e, caso os nomes sejam iguais, uma comparação por data é realizada.
 * @param dado1 Ponteiro genérico para o primeiro dado fornecido para a comparação.
 * @param dado2 Ponteiro genérico para o segundo dado fornecido para a comparação.
 * @return bool 'True' se o primeiro dado fornecido é menor que o segundo e 'false' caso seja maior.
 */
bool ehMenorDadoBusca(void* dado1, void* dado2);

/**
 * @brief Função callback responsável por escrever um dado(dadoBusca) no arquivo.
 * @details A função analisa o nome e a data de nascimento do funcionário e os escreve no disco.
 * @param dado Ponteiro genérico para a estrutura dadoBusca que será escrita.
 * @param arquivo Ponteiro genérico para o arquivo em que será escrito o dado.
 */
void escreveDadoBusca(void *dado, FILE *arquivo);

/**
 * @brief Função callback responsável por ler um dado(dadoBusca) do disco para a memória RAM.
 * @details A função pega o nome e a data de nascimento do funcionário do arquivo e os armazena no dadoBusca.
 * @param dado Ponteiro genérico para a estrutura dadoBusca onde os dados serão carregados.
 * @param arquivo Ponteiro para o arquivo em que será lido o dado.
 */
void leituraDadoBusca(void *dado, FILE *arquivo);

/**
 * @brief Função callback responsável por imprimir a ficha cadastral completa de dados do funcionário
 * @param dado Ponteiro genérico para os dados do funcionário que serão impressos.
 */
void imprimeDadosFuncionario(void* dado);

/**
 * @brief Função callback responsável por imprimir de forma resumida o primeiro nome e a data de nascimento do funcionário.
 * @details Utilizada na impressão da estrutura da árvore.
 * @param dado Ponteiro genérico para a chave (dadoBusca) que será impressa.
 */
void imprimePrimeiroNomeDataFuncionario(void* dado);

/**
 * @brief Função callback responsável por imprimir o nome completo e a data de nascimento do funcionário.
 * @details Utilizada nos menus para o desempate quando homônimos são localizados.
 * @param dado Ponteiro genérico para a chave (dadoBusca) que será impressa.
 */
void imprimeChaveDadoBusca(void *dado);

/**
 * @brief Função responsável por criar um dado(dadoBusca) de um funcionário.
 * @details A função pega o nome e a data de nascimento do funcionário (componentes do dadoBusca) fornecidos pelo usuário e preenche dadoBusca.
 */
dadoBusca criaDadoBusca();

/**
 * @brief Função responsável por criar o registro de funcionário e preencher com os respectivos dados.
 * @param nome Vetor para armazenar o nome do funcionário.
 * @param dataNascimento Variável do tipo data para armazenar a data de nascimento do funcionário.
 * @return Funcionario O funcionário preenchido com todos os dados contendo endereço, filiação, contratos e histórico pago.
 */
Funcionario criaFuncionario(char nome[100], Data dataNascimento);

/**
 * @brief Função responsável por criar, inicializar e gravar o cabeçalho do arquivo de registros de funcionários no disco.
 * @details A função separa o espaço para o cabeçalho em arquivos vazios. 
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 */
void inicializaArquivoRegistros(FILE* arquivo);

/**
 * @brief Função responsável por ler o cabeçalho do arquivo de registros de funcionários do disco para a memória RAM.
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 * @return Registros* Ponteiro para a estrutura do cabeçalho do arquivo alocada na memória RAM.
 */
Registros* leituraCabecalhoRegistros(FILE* arquivo);

/**
 * @brief Função responsável por atualizar o cabeçalho modificado no arquivo de registros de funcionários.
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 * @param cabecalho Ponteiro para o cabeçalho já modificado que será escrito no disco.
 */
void escreveCabecalhoRegistros(FILE* arquivo, Registros* cabecalho);

/**
 * @brief Função responsável por indicar o espaço (índice) no disco onde o próximo registro de funcionário será inserido, priorizando o reaproveitamento de registros inativos.
 * @param arquivo Ponteiro para o arquivo de registros de funcionários.
 * @param cabecalho Ponteiro para o cabeçalho já modificado que será escrito.
 * @return int A posição física (índice) no arquivo onde o novo registro de funcionário pode ser inserido
 */
int proxRegistro(FILE* arquivo, Registros* cabecalho);

/**
 * @brief Função responsável por preencher os pagamentos mensais do funcionário.
 * @details Mantém no máximo 12 pagamentos. Se o histórico atingir o limite, realiza um deslocamento (shift) descartando o pagamento mais antigo.
 * @param funcionario Ponteiro para a estrutura funcionário que receberá o registro de pagamento.
 * @param valor Valor que corresponde ao pagamento mensal do funcionário.
 */
void adicionaPagamento(Funcionario* funcionario, double valor);