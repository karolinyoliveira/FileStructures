// Karoliny Oliveira Ozias Silva - 10368020
#ifndef HeaderOA_H
#define HeaderOA_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//Structs
typedef struct a
{
    char status;
    int RRNproxRegistro;
    int numeroRegistrosInseridos;
    int numeroRegistrosRemovidos;
    int numeroRegistrosAtualizados;
    char lixo[111];

} cabecalho;

typedef struct b
{
    int tamCidadeMae;
    int tamCidadeBebe;
    char cidadeMae[52];
    char cidadeBebe[52];
    int idNascimento;
    int idadeMae;
    char dataNascimento[11]; //  AAAA-MM-DD
    char sexoBebe;           // ‘0’(ignorado), ‘1’(masculino) e ‘2’(feminino).
    char estadoMae[3];
    char estadoBebe[3];
} dados;

typedef struct c
{
    char status;
    int noRaiz;
    int nroNiveis;
    int proxRRN;
    int nroChaves;
    char lixo[55];

} cabecalhoBTree;

typedef struct d
{
    int nivel; //nó-folha = 1
    int n;     //n elementos na página de disco
    int C[5];  //Chave de busca (idNascimento)
    int Pr[5]; //RRN do arquivo de Dados
    int P[6];  //RRN para subárvore

} dadosBTree;

//Funções
/**
 * @brief Coloca o marcador desejado em todo trecho considerado lixo
 * 
 * @param marcador : char a ser utilizado para espaço considerado lixo
 * @param vetorLixo : vetor que irá ser marcado como lixo
 * 
 * \return void
**/
void trataLixo(char marcador, char *vetorLixo, int tamLixo);

/**
 * @brief Inicia os valores de cabeçalho zerados e já dentro do arquivo binário
 * 
 * @param cab : Ponteiro para a struct de cabeçalho
 * @param fp : Ponteiro para o arquivo a ser escrito
 * 
 * \return void
 **/
void initHeader(cabecalho *cab, FILE *fp);

/**
 * @brief Realiza a leitura de 1 registro, campo a campo, por vez.
 * 
 * @param fp :Ponteiro p/ o arquivo a ser lido
 * @param dados :Ponteiro p/ a struct que armazena os campos na RAM
 * 
 * \return 1 caso encontre o fim do arquivo, caso contrário O.
 **/
int leitorCsv(FILE *fp, dados *dados);

/**
 * @brief Escreve os campos armazenados na struct dados em um arquivo.
 * 
 * @param fp :Ponteiro para o arquivo a ser lido
 * @param dados :Struct que armazena os campos de um registro na RAM
 * 
 * \return void
**/
int escreveBin(FILE *fp, dados dados);

/**
 * @brief Atualiza os valores de cabeçalho de acordo com a alteracao solicitada (apenas 1)
 * 
 * @param cab : Struct de cabeçalho
 * @param alteracao : Pode ser:'s' status;                     (altera para 1 ou 0)
 *                             'p' RRNproxregistro;            (incremento)
 *                             'i' numeroRegistrosInseridos;   (incremento)
 *                             'r' numeroRegistrosRemovidos;   (incremento de r, decremento de i)
 *                             'a' numeroRegistrosAtualizados. (incremento)
 * 
 * \return Retorna 1 quando a escrita foi bem sucedida e 0 em caso de registro removido
**/
void atualizaHeaderRAM(cabecalho *cab, char alteracao);

/**
 * @brief Atualiza todo o cabeçalho em disco
 * 
 * @param fp : Ponteiro para o arquivo a ter o cabeçalho atualizado
 * @param cab : Struct que armazena os campos de um registro na RAM
 * 
 * \return void
**/
void atualizaHeaderDISCO(cabecalho cab, FILE *fp);

/**
 * @brief Faz a leitura de um registro i para a struct de dados
 * 
 * @param fp : Ponteiro para o arquivo a ser lido
 * @param dados : Struct para armazenar registro na RAM
 * @param i : RRN do registro
 * 
 * \return 0 caso o registro esteja marcado como removido, caso contrário 1
**/
int leituraRegistro(FILE *fp, dados *dados, int i);

/**
 * @brief Verifica entradas vazias e printa (stdout) as informações necessárias.
 * 
 * @param fp : Ponteiro para o arquivo a ser lido
 * @param dados : Struct com os dados do registro
 * @param RegistroNum : RRN 
 * @param cab : struct de cabeçalho
 *
 * \return void
**/
void printaBin(FILE *fp, dados dados);

/**
 * @brief Reseta todos os valores da struct de campos do registro de acordo com a
 * versao: 1 (zero em tudo) 2 (modificado - p/ uso em funcoes de comparacao de campos)
 * 
 * @param dados : Ponteiro para a struct com os valores de cada campo do registro
 * @param versao : Define qual a versao da funcao 
 * 
 * \return void
**/
void zeraDados(dados *dados, int versao);

//---------------------------------- PARTE II ---------------------------------------

/**
 * @brief Converte o valor do RRN para o byte offset considerando registros de 128b
 * 
 * @param RRN : RRN desejado
 * 
 * \return valor do byte offset
**/
int rrnToOffset(int RRN);

/**
 * @brief Adiciona o valor ao campo correspondente em uma struct de dados
 * 
 * @param criteriosBusca : Struct de dados dos criterios a serem selecionados em cada registro
 * @param campo : Campo a ser acrescentando como criterio de busca
 * @param valor : Valor do criterio de busca
 * 
 * \return void
**/
void setCriterios(dados *criteriosBusca, char *campo, char *valor);

/**
 * @brief insere m criterios com o uso da funcao setCriterios
 * 
 * @param criterios : Ponteiro para a struct com os valores de cada campo do registro
 * @param m : numero de criterios desejados 
 * 
 * \return void
**/
void insereCriterios(dados *criterios, int m);

/**
 * @brief Compara as structs de dados de um registro com a struct de criterios de busca
 * 
 * @param data : Struct com os dados do registro
 * @param criterios : Struct com os criterios a serem buscados
 * 
 * \return o numero de campos que deram "match", max=8.
**/
int comparaCampos(dados data, dados criterios);

/**
 * @brief Insere na struct de dados, os campos de um registro da entrada padrão 
 * 
 * @param data : Ponteiro para a struct com os valores de cada campo do registro
 * 
 * \return void
**/
void gravaNovoRegistro(dados *data);

/**
 * @brief Verifica se o arquivo esta apropriado para uso (status 1) e se é um arquivo sem registros
 * 
 * @param pointer : Ponteiro para o arquivo a ser avaliado
 * @param versao : 1 - sem escrita na saída padrão, 2 - com escrita
 * 
 * \return a qtde de registros lidos
**/
int avaliaArquivo(FILE *pointer, int versao);

/**
 * @brief Le o cabecalho de um arquivo bin e guarda na struct de cabecalho
 * 
 * @param fp : Ponteiro para a struct com os valores de cada campo do registro
 * @param cab : Define qual a versao da funcao 
 * 
 * \return void
**/
void leituraHeader(FILE *fp, cabecalho *cab);

/**
 * @brief atualiza os campos adcionados na struct data no arquivo binario
 * 
 * @param fp : Ponteiro para o arquivo utilizado
 * @param data : Struct de dados usados 
 * @param rrn : RRN do registro a ser atualizado
 * 
 * \return 1 se a atualização ocorreu corretamente e 0 de registro removido
**/
int atualizaCampos(FILE *fp, dados data, int rrn);

/**
 * @brief Altera todos os campos da struct dados para um valor neutro ("X" - char(*) e INT_MAX - inteiros)
 * 
 * @param dados : Ponteiro para a struct com os valores de cada campo do registro
 * 
 * \return void
**/
void zeraDadosAtualizacao(dados *dados);

//---------------------------------- PARTE III --------------------------------------

/**
 * @brief Inicializa os valores do registro de cabeçalho do arquivo de índices da árvore-B
 * 
 * @param cab : Ponteiro para a struct com os valores de cada campo do registro
 * @param fp : 
 * 
 * \return void
**/
void initHeaderBTree(cabecalhoBTree *cab, FILE *fp);

/**
 * @brief Atualiza os valores de cabeçalho da árvore-B de acordo com a alteracao solicitada (apenas 1)
 * 
 * @param cab : Struct de cabeçalho
 * @param alteracao : Pode ser:'s' status;                     (altera para 1 ou 0)
 *                             'n' nroNiveis;                  (incremento)
 *                             'p' proxRRN;                    (incremento)
 *                             'c' nroChaves;                  (incremento)
 * 
 * \return void
**/
void atualizaHeaderRAMBTree(cabecalhoBTree *cab, char alteracao);

/**
 * @brief Le o cabecalho de um arquivo de índice da árvore-B e guarda na struct de cabecalho
 * 
 * @param fp : Ponteiro para a struct com os valores de cada campo do registro
 * @param cab : Define qual a versao da funcao 
 * 
 * \return void
**/
void leituraHeaderBTree(FILE *fp, cabecalhoBTree *cab);

/**
 * @brief Verifica se o arquivo esta apropriado para uso (status 1) e se é um arquivo sem registros
 * 
 * @param pointer : Ponteiro para o arquivo a ser avaliado
 * @param versao : 1 - sem escrita na saída padrão, 2 - com escrita
 * 
 * \return a qtde de registros lidos
**/
int avaliaIndice(FILE *pointer, int versao);

/**
 * @brief Realiza a busca por um valor no arquivo de índices da árvore-B
 * 
 * @param fp : Ponteiro para o arquivo de índices
 * @param cab : Registro de cabeçalho da árvore-B 
 * @param qtdePgs : Quantidade de páginas acessadas durante a busca
 * @param valor : Chave a ser buscada
 * 
 * \return -1 caso a chave não seja encontrada e o RRN do arquivo de dados caso seja encontrada
**/
int buscaBTree(FILE *fp, cabecalhoBTree cab, int *qtdePgs, int valor);

/**
 * @brief Traz as informações de um registro de dados da árvore-B para a RAM
 * 
 * @param fp : Ponteiro do arquivo de índices a ser lido
 * @param reg : Struct para armazenar registro na RAM
 * @param RRN : RRN do registro
 * 
 * \return void
**/
void leituraRegistroBTree(FILE *fp, dadosBTree *reg, int RRN);

/**
 * @brief Atualiza todo o cabeçalho em disco
 * 
 * @param fp : Ponteiro para o arquivo a ter o cabeçalho atualizado
 * @param cab : Struct que armazena os campos de um registro na RAM
 * 
 * \return void
**/
void atualizaHeaderDiscoBTREE(cabecalhoBTree cab, FILE *fp);

/**
 * @brief Lê a chave de busca do rrn desejado no arquivo de dados
 * 
 * @param fp : Ponteiro para o arquivo de dados
 * @param dados : Struct que armazena os campos de um registro na RAM
 * @param i : RRN desejado
 * 
 * \return 0 caso o registro tenha sido removido e 1 se foi possível realizar a leitura da Chave de Busca
**/
int leituraChaveDeBusca(FILE *fp, dados *dados, int i);

/**
 * @brief Insere uma chave no arquivo de índices da Árvore-B
 * 
 * @param fp : Ponteiro do arquivo de índices
 * @param header : Struct para armazenar registro na RAM
 * @param chave : chave a ser inserida (idNascimento)
 * @param rrn : rrr do registro no arquivo de dados
 * @param sinalizador : indica qual tipo de inserção vai ser feita (simples ou de promoção)
 * @param filhos : rrr dos filhos da esquerda [0] e direita [1] no arquivo de índice
 * @param trilha : vetor para armazenar os rrns percorridos até o nó folha de uma inserção
 * @param profundidade : profundidade da árvore-B no pior caso
 * 
 * \return void
**/
void insereBTREE(FILE *fp, cabecalhoBTree *header, int chave, int rrn, int *sinalizador, int filhos[2], int *trilha, int profundidade);

/**
 * @brief Busca o nó folha para inserção
 * 
 * @param fp : Ponteiro do arquivo de índices
 * @param cab : Struct de cabeçalho do arquivo de índices
 * @param valor : valor a ser inserido
 * @param trilha : vetor para armazenar os rrns percorridos até o nó folha de uma inserção
 * @param profundidade : profundidade da árvore-B no pior caso
 * 
 * \return rrn do nó folha
**/
int buscaNoFolha(FILE *fp, cabecalhoBTree cab, int valor, int *trilha, int profundidade);

/**
 * @brief Insere uma nova página de disco no arquivo de índices da Árvore-B
 * 
 * @param registro : Struct com os campos do nó
 * @param fp :  Ponteiro do arquivo de índices
 * @param rrn : rrr em que o registro deve ser inserido
 * 
 * \return void
**/
void insereNovaPagina(dadosBTree registro, FILE *fp, int rrn);

/**
 * @brief Lê um nó(página de disco) do arquivo de índices
 * 
 * @param registro : Struct para armazenamento dos campos do nó
 * @param fp :  Ponteiro do arquivo de índices
 * @param rrn : rrr do registro no arquivo de índice
 * 
 * \return void
**/
void leituraPaginaDeDisco(dadosBTree *registro, FILE *fp, int rrn);

/**
 * @brief Insertion sort apenas na ultima chave inserida
 * 
 * @param v : Struct de dados do arquivo de índice
 * @param n : Número de chaves no registro
 * 
 * \return void
**/
void Sorting(dadosBTree *v, int n);

//--------------------FUNÇOES FORNECIDAS PELA PROFA-----------------------
void binarioNaTela(char *nomeArquivoBinario);
void trim(char *str);
void scan_quote_string(char *str);
#endif