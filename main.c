// Karoliny Oliveira Ozias Silva - 10368020
#include "headerOA.h"

int main(void)
{
    //Inicialização das Structs utilizadas
    cabecalho header;
    dados data;
    cabecalhoBTree headerBTree = {'0', 0, 0, 0, 0};
    dadosBTree dataBTree;

    //Recebimento de valores de Entrada
    int opcao;
    char entrada1[30];
    char entrada2[30];
    scanf("%d", &opcao);

    //Abertura/Criação dos arquivos csv e bin
    FILE *fpCsv;   //Ponteiro para arquivo csv
    FILE *fpBin;   //Ponteiro para arquivo bin
    FILE *fpIndex; //Ponteiro para arquivo de índice da árvore B

    if (opcao == 1)
    {
        //Inicializa ponteiro de arquivo csv
        scanf(" %s", entrada1);
        if (!(fpCsv = fopen(entrada1, "r")))
        {
            printf("Falha no carregamento do arquivo.\n");
            return EXIT_SUCCESS;
        }
    }

    //Abertura dos arquivos de dados
    scanf(" %s", entrada2);
    fpBin = fopen(entrada2, "rb+");
    if (fpBin == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return EXIT_SUCCESS;
    }

    //Abertura dos arquivos de índices da árvore-B
    if (opcao == 8)
    {
        scanf(" %s", entrada1);
        if (!(fpIndex = fopen(entrada1, "wb+")))
        {
            printf("Falha no processamento do arquivo.\n");
            return EXIT_SUCCESS;
        }
    }

    if (opcao == 9 || opcao == 10)
    {
        scanf(" %s", entrada1);
        if (!(fpIndex = fopen(entrada1, "rb+")))
        {
            printf("Falha no carregamento do arquivo.\n");
            return EXIT_SUCCESS;
        }
    }

    switch (opcao)
    {
    case 1: //Armazenamento de dados
    {
        //Leitura e armazenamento inicial do cabecalho
        initHeader(&header, fpBin);

        //Verifica se o arquivo csv está vazio
        fseek(fpCsv, 0, SEEK_END);
        if (ftell(fpCsv) == 0)
        {
            header.status = '1';
            fseek(fpBin, 0, SEEK_SET);
            fwrite(&header.status, sizeof(char), 1, fpBin);
            fclose(fpBin);
            binarioNaTela(entrada2);
            return EXIT_SUCCESS;
        }
        fseek(fpCsv, 88, SEEK_SET);

        //Leitura e armazenamento em disco
        int flag = 0;
        flag = leitorCsv(fpCsv, &data); //Armazena 1º registro em RAM
        while (!flag)
        {
            escreveBin(fpBin, data);         //escreve no arquivo .bin (também verifica e modifica campos nulos)
            atualizaHeaderRAM(&header, 'p'); //atualiza RRNproxregistro em RAM
            atualizaHeaderRAM(&header, 'i'); //atualiza numeroRegistrosInseridos em RAM
            zeraDados(&data, 1);             //reseta struct de campos do registro
            flag = leitorCsv(fpCsv, &data);  //Armazena na RAM o novo registro a ser escrito
        }

        //Caso a leitura seja feita corretamente, atualiza-se o status em RAM
        if (flag == 1)
        {
            atualizaHeaderRAM(&header, 's'); //atualiza status em RAM
        }

        //Atualização do cabeçalho em disco
        atualizaHeaderDISCO(header, fpBin);

        //Fechamento de arquivos
        fclose(fpCsv);
        fclose(fpBin);
        binarioNaTela(entrada2);
        break;
    }
    case 2: //Recuperação de TODOS os dados
    {
        //Verifica se o status do arquivo está adequado para leitura
        long qtdeRegistros = avaliaArquivo(fpBin, 1);
        if (!qtdeRegistros)
            return EXIT_SUCCESS;

        //Realiza o processamento dos dados solicitados em [2]
        for (int i = 1; i <= qtdeRegistros; i++)
        {
            zeraDados(&data, 1);              //reseta struct de campos do registro
            leituraRegistro(fpBin, &data, i); //le os campos e coloca na struct de dados
            printaBin(fpBin, data);           //verifica campos ausentes e printa (stdout)
        }

        //Fechamento do arquivo
        fclose(fpBin);
        break;
    }
    case 3: //Recuperação de dados - Criterios de Busca
    {
        //Leitura dos criterios
        int m;
        dados criteriosBusca;
        zeraDados(&criteriosBusca, 2);
        scanf("%d", &m);
        insereCriterios(&criteriosBusca, m);

        //Comparação e print stdout (Execucação da funcionalidade 2 com comparacao dos criterios de busca)
        long qtdeRegistros = avaliaArquivo(fpBin, 1);
        if (!qtdeRegistros)
            return EXIT_SUCCESS;

        //Realiza o processamento/comparacao dos dados solicitados
        int temRegistro = 0;
        for (int i = 1; i <= qtdeRegistros; i++)
        {
            zeraDados(&data, 2);                          //reseta struct de campos do registro
            leituraRegistro(fpBin, &data, i);             //le os campos e coloca na struct de dados
            if (comparaCampos(data, criteriosBusca) != 8) //Compara os dados com os criterios de busca
                continue;
            temRegistro = 1;
            printaBin(fpBin, data); //verifica campos ausentes e printa (stdout)
        }

        if (!temRegistro)
        {
            printf("Registro Inexistente.");
            return EXIT_SUCCESS;
        }

        //Fechamento do arquivo
        fclose(fpBin);
        break;
    }
    case 4: //Recuperação de dados - RRN
    {
        //Verifica se o status do arquivo está adequado para leitura
        if (!avaliaArquivo(fpBin, 1))
            return EXIT_SUCCESS;

        int RRN = 0;
        scanf("%d", &RRN);
        zeraDados(&data, 1); //reseta struct de campos do registro
        leituraRegistro(fpBin, &data, RRN + 1);
        printaBin(fpBin, data);

        //Fechamento do arquivo
        fclose(fpBin);
        break;
    }
    case 5: //Remoção Lógica de registro - Abordagem Estática
    {
        //Verifica se o status do arquivo está adequado para leitura
        if (!avaliaArquivo(fpBin, 1))
            return EXIT_SUCCESS;

        //leitura do registro de cabecalho e atualizacao de status
        leituraHeader(fpBin, &header);

        //Inicialização de variaveis
        int n, m;
        scanf("%d", &n);
        dados criteriosBusca;

        for (int i = 0; i < n; i++)
        {
            zeraDados(&criteriosBusca, 2);
            scanf("%d", &m);
            insereCriterios(&criteriosBusca, m);

            for (int rrn = 1; rrn <= header.RRNproxRegistro; rrn++)
            {
                zeraDados(&data, 2);                          //reseta struct de campos do registro
                leituraRegistro(fpBin, &data, rrn);           //le os campos e coloca na struct de dados
                if (comparaCampos(data, criteriosBusca) == 8) //Compara dados e criterios de busca
                {
                    data.tamCidadeMae = -1;
                    fseek(fpBin, 128 * rrn, SEEK_SET);
                    fwrite(&data.tamCidadeMae, sizeof(int), 1, fpBin);
                    atualizaHeaderRAM(&header, 'r');
                }
            }
        }

        //Atualização de Status e escrita de todos os registros de cabecalho em disco
        atualizaHeaderRAM(&header, 's');
        atualizaHeaderDISCO(header, fpBin);

        //Fechamento do arquivo
        fclose(fpBin);
        binarioNaTela(entrada2);
        break;
    }
    case 6: //Inserção de registros - Abordagem Estática
    {
        //Verifica se o status do arquivo está adequado para leitura
        if (!avaliaArquivo(fpBin, 1))
            return EXIT_SUCCESS;

        //leitura do registro de cabecalho e atualizacao de status
        leituraHeader(fpBin, &header);
        fseek(fpBin, 0, SEEK_END);

        int n;
        scanf("%d", &n);

        for (int i = 0; i < n; i++)
        {
            //Grava registro a ser inserido
            zeraDados(&data, 1);
            gravaNovoRegistro(&data);

            //Insere registro no disco
            escreveBin(fpBin, data);

            //Atualizacoes de cabecalho
            atualizaHeaderRAM(&header, 'i');
            atualizaHeaderRAM(&header, 'p');
        }

        //Atualização de Status e escrita de todos os registros de cabecalho em disco
        atualizaHeaderRAM(&header, 's');
        atualizaHeaderDISCO(header, fpBin);

        //Fechamento do arquivo
        fclose(fpBin);
        binarioNaTela(entrada2);
        break;
    }
    case 7: //Atualização de Campos de um registro - RRN
    {
        dados atualizacoes;
        int m;

        //Verifica se o status do arquivo está adequado para leitura
        if (!avaliaArquivo(fpBin, 1))
            return EXIT_SUCCESS;

        //leitura do registro de cabecalho e atualizacao de status
        leituraHeader(fpBin, &header);

        int n, RRN;
        scanf("%d", &n);
        for (int i = 0; i < n; i++)
        {
            zeraDadosAtualizacao(&atualizacoes);
            scanf("%d", &RRN);
            scanf("%d", &m);
            insereCriterios(&atualizacoes, m);
            if (!atualizaCampos(fpBin, atualizacoes, RRN))
                continue;
            atualizaHeaderRAM(&header, 'a');
        }

        //Atualização de status e escrita de todos os registros de cabecalho em disco
        atualizaHeaderRAM(&header, 's');
        atualizaHeaderDISCO(header, fpBin);

        //Fechamento do arquivo
        fclose(fpBin);
        binarioNaTela(entrada2);
        break;
    }
    case 8: //Criação de arquivo de índices a partir dos registros de dados (Árvore-B)
    {

        //Verifica se o status do arquivo está adequado para leitura
        if (!avaliaArquivo(fpBin, 1))
            return EXIT_SUCCESS;

        //leitura do registro de cabecalho e atualizacao de status
        leituraHeader(fpBin, &header);

        //Cálculo da profundidade no pior caso para as N entradas do arquivo de dados
        int profundidade = 1 + (int)(log((header.numeroRegistrosInseridos + 1) / 2) / log(3));
        if (profundidade <= 0)
            profundidade = 1;

        //armazena e inicializa espaço para o caminho percorrido até o nó folha
        int *trilha;
        trilha = (int *)malloc(profundidade * sizeof(int));
        for (int i = 0; i < profundidade; i++)
        {
            trilha[i] = -1;
        }

        //Leitura e armazenamento inicial do cabecalho do arquivo de índices
        initHeaderBTree(&headerBTree, fpIndex);

        //Leitura e armazenamento em disco
        int i = 1;
        int sinalizador = 0;
        int filhos[2] = {-1, -1};
        while (i < header.numeroRegistrosInseridos+1)
        {
            if (leituraChaveDeBusca(fpBin, &data, i)) //evita inserção de registros removidos
            {
                insereBTREE(fpIndex, &headerBTree, data.idNascimento, i-1, &sinalizador, filhos, trilha, profundidade);
                atualizaHeaderRAMBTree(&headerBTree, 'c');
            }
            sinalizador = 0;
            i++;
        }

        //Caso a leitura seja feita corretamente, atualiza status de ambos os arquivos
        atualizaHeaderRAMBTree(&headerBTree, 's');
        atualizaHeaderDiscoBTREE(headerBTree, fpIndex);

        atualizaHeaderRAM(&header, 's');
        atualizaHeaderDISCO(header, fpBin);

        //Fechamento do arquivo
        fclose(fpBin);
        fclose(fpIndex);
        binarioNaTela(entrada1);
        break;
    }
    case 9: //Recuperação de registros - Critério de busca (Árvore-B)
    {
        //Recebimento dos dados desejados
        char campo[15]; //sempre "idNascimento"
        int valor;
        scanf("%s %d", campo, &valor);

        //Verifica se os arquivos possuem status adequado e pelo menos um registro de dados
        if (avaliaArquivo(fpBin, 2) <= 0)
            return EXIT_SUCCESS;

        if (!avaliaIndice(fpIndex, 2))
            return EXIT_SUCCESS;

        //Leitura do registro de cabecalho e atualizacao de status
        leituraHeaderBTree(fpIndex, &headerBTree);

        //Busca no Arquivo de índice
        int RRN = -1;
        int qtdePaginasAcessadas = 1; //considerando leitura do nó raiz
        if (headerBTree.noRaiz != -1)
            RRN = buscaBTree(fpIndex, headerBTree, &qtdePaginasAcessadas, valor);

        //Print do registro encontrado e qtde de Paginas Acessadas
        if (RRN != -1)
        {
            zeraDados(&data, 1); //reseta struct de campos do registro de dados
            if (leituraRegistro(fpBin, &data, RRN + 1))
            {
                printaBin(fpBin, data);
                printf("Quantidade de paginas da arvore-B acessadas: %d", qtdePaginasAcessadas);
            }
            else
                printf("Registro inexistente.");
        }
        else
            printf("Registro inexistente.");
        //Fechamento do arquivo
        fclose(fpBin);
        fclose(fpIndex);
        break;
    }
    case 10: //Inserção de registros - Abordagem Estática + Inserção arquivo de índices (Árvore-B)
    {
        //Verifica se o status dos arquivos estao adequados para leitura
        if (!avaliaArquivo(fpBin, 1))
            return EXIT_SUCCESS;

        //leitura do registro de cabecalho e atualizacao de status
        leituraHeader(fpBin, &header);
        fseek(fpBin, 0, SEEK_END);

        //Cálculo da profundidade no pior caso para as N entradas do arquivo de dados
        int profundidade = 1 + (int)(log((header.numeroRegistrosInseridos + 1) / 2) / log(3));
        if (profundidade <= 0)
            profundidade = 1;

        //armazena o caminho percorrido até o nó folha, para cada inserção
        int *trilha;
        trilha = (int *)malloc(profundidade * sizeof(int));
        for (int i = 0; i < profundidade; i++)
        {
            trilha[i] = -1;
        }

        //Leitura e armazenamento inicial do cabecalho do arquivo de índices
        leituraHeaderBTree(fpIndex, &headerBTree);
        atualizaHeaderRAMBTree(&headerBTree, 's');

        //Inserção
        int n;
        scanf("%d", &n);

        int sinalizador = 0; //Indica para a InsereBTREE se é uma inserção simples ou de promoção
        int filhos[2] = {-1, -1};
        for (int i = 0; i < n; i++)
        {
            //Grava registro a ser inserido
            zeraDados(&data, 1);
            gravaNovoRegistro(&data);

            //Insere registro no arquivo de dados
            escreveBin(fpBin, data);

            //Insere registro no arquivo de índice
            insereBTREE(fpIndex, &headerBTree, data.idNascimento, header.RRNproxRegistro, &sinalizador, filhos, trilha, profundidade);
            atualizaHeaderRAMBTree(&headerBTree, 'c');

            //Atualizacoes de cabecalho
            atualizaHeaderRAM(&header, 'i');
            atualizaHeaderRAM(&header, 'p');

            sinalizador = 0;
        }

        //Atualização de Status e escrita de todos os registros de cabecalho em disco
        atualizaHeaderRAM(&header, 's');
        atualizaHeaderDISCO(header, fpBin);

        atualizaHeaderRAMBTree(&headerBTree, 's');
        atualizaHeaderDiscoBTREE(headerBTree, fpIndex);
        //printCabecalho(headerBTree);

        //Fechamento do arquivo
        fclose(fpBin);
        fclose(fpIndex);
        binarioNaTela(entrada1);
        break;
    }
    default:
        return EXIT_SUCCESS;
        break;
    }
    return EXIT_SUCCESS;
}
