# 🧾 Sistema RH com Árvore B+

Um sistema de gerenciamento de funcionários desenvolvido em C, utilizando uma Árvore B+ genérica em disco para indexação eficiente dos registros. O projeto permite cadastrar, buscar, listar por intervalo, remover e visualizar a estrutura do índice, mantendo os dados persistentes em arquivos binários.

## ✨ Technologies

- C
- GCC
- Makefile
- Arquivos binários
- Árvore B+
- Estruturas genéricas com `void*`

## 🚀 Features

- Cadastro de funcionários com ficha completa
- Busca por nome com tratamento de homônimos
- Chave composta por nome e data de nascimento
- Remoção lógica de funcionários
- Reaproveitamento de posições livres no arquivo
- Listagem por intervalo fechado
- Impressão textual da estrutura da Árvore B+
- Persistência dos dados após encerrar o programa
- Histórico dos últimos 12 pagamentos do funcionário

## 🌳 O Processo

O projeto foi desenvolvido com o objetivo de integrar uma estrutura de indexação em disco a um sistema de Recursos Humanos. A Árvore B+ foi implementada de forma genérica, permitindo que a comparação, leitura, escrita e impressão das chaves sejam controladas por funções de callback. Dessa forma, a estrutura da árvore não depende diretamente dos dados de funcionários, mantendo uma separação entre o índice e os registros armazenados.

No sistema, cada funcionário é salvo no arquivo `funcionario.bin`, enquanto a Árvore B+ é mantida no arquivo `bplus.bin`. A chave utilizada no índice é composta pelo nome do funcionário e pela data de nascimento, permitindo diferenciar homônimos e impedir duplicidades exatas. As folhas da árvore armazenam as chaves e as posições dos registros no arquivo de funcionários, possibilitando que o sistema recupere a ficha completa quando necessário.

As operações de inserção, busca, remoção e listagem por intervalo foram implementadas considerando o funcionamento da Árvore B+ em disco, incluindo divisão de páginas, propagação de chaves, redistribuição, concatenação, atualização de separadores e ajuste da raiz em casos extremos.

## 🧩 Estruturas Principais

- `Funcionario`: armazena a ficha completa do funcionário.
- `Data`: representa datas de nascimento, contratação e desligamento.
- `Endereco`: armazena o endereço residencial do funcionário.
- `dadoBusca`: representa a chave composta usada na Árvore B+.
- `Registros`: cabeçalho do arquivo de funcionários.
- `Pagina`: representa uma página ou nó da Árvore B+.
- `Bplus`: cabeçalho da Árvore B+ em disco.

## 📁 Estrutura do projeto

```text
Sistema-RH/
├── Bplus.c
├── Bplus.h
├── Funcionario.c
├── Funcionario.h
├── Util.c
├── Util.h
├── main.c
├── Makefile
├── bplus.bin
└── funcionario.bin
