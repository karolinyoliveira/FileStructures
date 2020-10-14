// Karoliny Oliveira Ozias Silva - 10368020
#include "headerOA.h"

int rrnToOffset(int RRN)
{
    return 128 + 128 * RRN;
}

void setCriterios(dados *criteriosBusca, char *campo, char *valor)
{
    switch (strlen(campo))
    {
    case 9: //cidadeMae ou estadoMae
        if (campo[0] == 'c')
        {
            criteriosBusca->tamCidadeMae = (int)strlen(valor);
            strcpy(criteriosBusca->cidadeMae, valor);
        }
        if (campo[0] == 'e')
            strcpy(criteriosBusca->estadoMae, valor);
        break;
    case 10: //cidadeBebe ou estadoBebe
        if (campo[0] == 'c')
        {
            criteriosBusca->tamCidadeBebe = (int)strlen(valor);
            strcpy(criteriosBusca->cidadeBebe, valor);
        }
        if (campo[0] == 'e')
            strcpy(criteriosBusca->estadoBebe, valor);
        break;
    case 12: //idNascimento
        criteriosBusca->idNascimento = (int)strtol(valor, NULL, 10);
        break;
    case 8: //idadeMae ou sexoBebe
        if (campo[0] == 'i')
        {
            criteriosBusca->idadeMae = (int)strtol(valor, NULL, 10);
            if (criteriosBusca->idadeMae == 0)
                criteriosBusca->idadeMae = -1;
        }
        if (campo[0] == 's')
            criteriosBusca->sexoBebe = valor[0];
        break;
    case 14: //dataNascimento
        strcpy(criteriosBusca->dataNascimento, valor);
        break;
    default:
        break;
    }
}

int comparaCampos(dados data, dados criterios)
{
    if (data.tamCidadeMae == -1)
        return 0;
    int camposAprovados = 0;
    if (strcmp(criterios.cidadeMae, "")) //cidadeMae
    {
        if (!strcmp(criterios.cidadeMae, data.cidadeMae))
            camposAprovados++;
    }
    else
    {
        camposAprovados++;
    }

    if (strcmp(criterios.cidadeBebe, "")) //cidadeBebe
    {
        if (!strcmp(criterios.cidadeBebe, data.cidadeBebe))
            camposAprovados++;
    }
    else
    {
        camposAprovados++;
    }

    if (criterios.idNascimento != 0) //idNascimento
    {
        if (criterios.idNascimento == data.idNascimento)
            camposAprovados++;
    }
    else
    {
        camposAprovados++;
    }

    if (criterios.idadeMae != 0) //idadeMae
    {
        if (criterios.idadeMae == data.idadeMae)
        {
            camposAprovados++;
        }
    }
    else
    {
        camposAprovados++;
    }

    if (strcmp(criterios.dataNascimento, "")) //dataNascimento
    {
        if (!strcmp(criterios.dataNascimento, data.dataNascimento))
            camposAprovados++;
    }
    else
    {
        camposAprovados++;
    }

    if (criterios.sexoBebe != 'X') //sexoBebe
    {
        if (criterios.sexoBebe == data.sexoBebe)
        {
            camposAprovados++;
        }
    }
    else
    {
        camposAprovados++;
    }

    if (strcmp(criterios.estadoMae, "")) //estadoMae
    {
        if (!strcmp(criterios.estadoMae, data.estadoMae))
        {
            camposAprovados++;
        }
    }
    else
    {
        camposAprovados++;
    }

    if (strcmp(criterios.estadoBebe, "")) //estadoBebe
    {
        if (!strcmp(criterios.estadoBebe, data.estadoBebe))
            camposAprovados++;
    }
    else
    {
        camposAprovados++;
    }
    return camposAprovados;
}

int avaliaArquivo(FILE *pointer, int versao)
{
    //Verifica se o status do arquivo está adequado para leitura
    char verificaStatus;
    verificaStatus = 'x';
    fread(&verificaStatus, sizeof(char), 1, pointer);
    if (verificaStatus == '0')
    {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    //Verifica se existe pelo menos um registro no .bin e calcula numero de registros no arquivo
    long bytes = 0;
    fseek(pointer, 0, SEEK_END);
    bytes = ftell(pointer);

    if (bytes == 128) //Só tem o header -> arquivo sem registros
    {
        if (versao == 2)
            printf("Registro inexistente.");
        return -42;
    }

    long qtdeRegistros = (bytes - 128) / 128;
    fseek(pointer, 128, SEEK_SET);
    return qtdeRegistros;
}

void insereCriterios(dados *criterios, int m)
{
    char NomeDoCampo[20];
    char valor[52];
    while (m != 0)
    {
        scanf("%s", NomeDoCampo);
        scan_quote_string(valor);
        setCriterios(criterios, NomeDoCampo, valor);
        m--;
    }
    return;
}

void gravaNovoRegistro(dados *data)
{
    char valor[52];

    scan_quote_string(valor); //cidadeMae
    data->tamCidadeMae = (int)strlen(valor);
    strcpy(data->cidadeMae, valor);

    scan_quote_string(valor); //cidadeBebe
    data->tamCidadeBebe = (int)strlen(valor);
    strcpy(data->cidadeBebe, valor);

    scan_quote_string(valor); //idNascimento
    data->idNascimento = (int)strtol(valor, 0, 10);

    scan_quote_string(valor); //idadeMae
    data->idadeMae = (int)strtol(valor, 0, 10);

    scan_quote_string(valor); //dataNascimento
    strcpy(data->dataNascimento, valor);

    scan_quote_string(valor); //sexoBebe
    if (!strlen(valor))
        data->sexoBebe = '0';
    else
        data->sexoBebe = valor[0];

    scan_quote_string(valor); //estadoMae
    strcpy(data->estadoMae, valor);

    scan_quote_string(valor); //estadoBebe
    strcpy(data->estadoBebe, valor);

    return;
}

void leituraHeader(FILE *fp, cabecalho *cab)
{
    fseek(fp, 0, SEEK_SET);

    //Atualização inicial do status para '0'
    atualizaHeaderRAM(cab, 's');
    fwrite(&cab->status, sizeof(char), 1, fp);

    //Leitura das informações de cabecalho
    fread(&cab->RRNproxRegistro, sizeof(int), 1, fp);
    fread(&cab->numeroRegistrosInseridos, sizeof(int), 1, fp);
    fread(&cab->numeroRegistrosRemovidos, sizeof(int), 1, fp);
    fread(&cab->numeroRegistrosAtualizados, sizeof(int), 1, fp);
    return;
}

int atualizaCampos(FILE *fp, dados data, int rrn)
{
    int tamCidadeMae, tamCidadeBebe;
    int marcador = 0; //uma tag para reconhecer o ultimo campo atualizado e evitar usos de fseeks desnecessarios
    char nulo = '\0';

    fseek(fp, rrnToOffset(rrn), SEEK_SET);
    fread(&tamCidadeMae, sizeof(int), 1, fp);
    if (tamCidadeMae == -1)
        return 0;
    if (data.tamCidadeMae > tamCidadeMae)
        data.tamCidadeMae = tamCidadeMae;

    fread(&tamCidadeBebe, sizeof(int), 1, fp);
    if (data.tamCidadeBebe > tamCidadeBebe)
        data.tamCidadeBebe = tamCidadeBebe;

    if (strcmp(data.cidadeMae, "X")) //cidadeMae
    {
        fwrite(data.cidadeMae, sizeof(char), data.tamCidadeMae, fp);
        fseek(fp, rrnToOffset(rrn), SEEK_SET);
        fwrite(&data.tamCidadeMae, sizeof(int), 1, fp);
        marcador = 1;
    }

    if (strcmp(data.cidadeBebe, "X")) //cidadeBebe
    {
        fseek(fp, rrnToOffset(rrn) + sizeof(int), SEEK_SET);
        fwrite(&data.tamCidadeBebe, sizeof(int), 1, fp);
        fseek(fp, tamCidadeMae, SEEK_CUR);
        fwrite(data.cidadeBebe, sizeof(char), data.tamCidadeBebe, fp);
        marcador = 2;
    }

    if (data.idNascimento != __INT_MAX__) //idNascimento
    {
        fseek(fp, rrnToOffset(rrn) + 105, SEEK_SET);
        fwrite(&data.idNascimento, sizeof(int), 1, fp);
        marcador = 3;
    }

    if (data.idadeMae != __INT_MAX__) //idadeMae
    {
        if (marcador != 3)
            fseek(fp, rrnToOffset(rrn) + 109, SEEK_SET);
        fwrite(&data.idadeMae, sizeof(int), 1, fp);
        marcador = 4;
    }

    if (strcmp(data.dataNascimento, "X")) //dataNascimento
    {
        if (marcador != 4)
            fseek(fp, rrnToOffset(rrn) + 113, SEEK_SET);
        if (strcmp(data.dataNascimento, ""))
            fwrite(data.dataNascimento, sizeof(char), strlen(data.dataNascimento), fp);
        else
            fwrite(&nulo, sizeof(char), 1, fp);
        marcador = 5;
    }

    if (data.sexoBebe != 'X') //sexoBebe
    {
        if (marcador != 5)
            fseek(fp, rrnToOffset(rrn) + 123, SEEK_SET);
        fwrite(&data.sexoBebe, sizeof(char), 1, fp);
        marcador = 6;
    }

    if (strcmp(data.estadoMae, "X")) //estadoMae
    {
        if (marcador != 6)
            fseek(fp, rrnToOffset(rrn) + 124, SEEK_SET);
        if (strcmp(data.estadoMae, ""))
            fwrite(data.estadoMae, sizeof(char), strlen(data.estadoMae), fp);
        else
            fwrite(&nulo, sizeof(char), 1, fp);

        marcador = 7;
    }

    if (strcmp(data.estadoBebe, "X")) //estadoBebe
    {
        if (marcador != 7)
            fseek(fp, rrnToOffset(rrn) + 126, SEEK_SET);
        if (strcmp(data.estadoBebe, ""))
            fwrite(data.estadoBebe, sizeof(char), strlen(data.estadoBebe), fp);
        else
            fwrite(&nulo, sizeof(char), 1, fp);
    }

    return 1;
}

void zeraDadosAtualizacao(dados *dados)
{
    dados->tamCidadeMae = __INT_MAX__;
    dados->tamCidadeBebe = __INT_MAX__;
    strcpy(dados->cidadeBebe, "X");
    strcpy(dados->cidadeMae, "X");
    dados->idNascimento = __INT_MAX__;
    dados->idadeMae = __INT_MAX__;
    strcpy(dados->dataNascimento, "X");
    dados->sexoBebe = 'X';
    strcpy(dados->estadoMae, "X");
    strcpy(dados->estadoBebe, "X");
    return;
}