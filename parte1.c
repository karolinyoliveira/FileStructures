// Karoliny Oliveira Ozias Silva - 10368020
#include "headerOA.h"

void trataLixo(char marcador, char *vetorLixo, int tamLixo)
{
    if (vetorLixo == NULL)
    {
        vetorLixo = (char *)malloc(sizeof(char) * tamLixo);
    }
    for (int i = 0; i < tamLixo; i++)
    {
        vetorLixo[i] = marcador;
    }
    return;
}

int escreveBin(FILE *fp, dados dados)
{
    //Verifica Arquivo Removido
    if (dados.tamCidadeMae == -1)
    {
        return 0;
    }
    //String para cópia em campos vazios
    char aux[] = "$$$$$$$$$";
    //escrita dos campos variaveis - indicador de tamanho
    fwrite(&dados.tamCidadeMae, sizeof(int), 1, fp);
    fwrite(&dados.tamCidadeBebe, sizeof(int), 1, fp);

    if (dados.tamCidadeMae != 0)
        fwrite(dados.cidadeMae, sizeof(char), strlen(dados.cidadeMae), fp);

    if (dados.tamCidadeBebe != 0)
        fwrite(dados.cidadeBebe, sizeof(char), strlen(dados.cidadeBebe), fp);

    //Tratamento/Inserção de Lixo
    int tamLixo = 105 - strlen(dados.cidadeMae) - strlen(dados.cidadeBebe) - 2 * sizeof(int);
    char *lixo;
    lixo = (char *)malloc(sizeof(char) * tamLixo);
    trataLixo('$', lixo, tamLixo);
    fwrite(lixo, sizeof(char), tamLixo, fp);
    free(lixo);

    //escrita dos campos fixos
    fwrite(&dados.idNascimento, sizeof(int), 1, fp);

    if (dados.idadeMae == 0)
        dados.idadeMae = -1;
    fwrite(&dados.idadeMae, sizeof(int), 1, fp);

    if (!strlen(dados.dataNascimento))
    {
        fwrite(dados.dataNascimento, sizeof(char), 1, fp);
        fwrite(aux, sizeof(char), 9, fp);
    }
    else
        fwrite(dados.dataNascimento, sizeof(char), 10, fp);

    fwrite(&dados.sexoBebe, sizeof(char), 1, fp);

    if (!strlen(dados.estadoMae))
    {
        fwrite(dados.estadoMae, sizeof(char), 1, fp);
        fwrite(aux, sizeof(char), 1, fp);
    }
    else
        fwrite(dados.estadoMae, sizeof(char), 2, fp);

    if (!strlen(dados.estadoBebe))
    {
        fwrite(dados.estadoBebe, sizeof(char), 1, fp);
        fwrite(aux, sizeof(char), 1, fp);
    }
    else
        fwrite(dados.estadoBebe, sizeof(char), 2, fp);
    return 1;
}

void printaBin(FILE *fp, dados dados)
{
    //Verifica se arquivo está removido
    if (dados.tamCidadeMae == -1)
    {
        printf("Registro Inexistente.\n");
        return;
    }

    //Interpreta o sexo do bebe
    char sexo[11];
    switch (dados.sexoBebe)
    {
    case '0':
        strcpy(sexo, "IGNORADO");
        break;
    case '1':
        strcpy(sexo, "MASCULINO");
        break;
    case '2':
        strcpy(sexo, "FEMININO");
        break;
    default:
        strcpy(sexo, "IGNORADO");
        break;
    }

    //Verifica entradas ausentes
    if (!strlen(dados.cidadeBebe))
        strcpy(dados.cidadeBebe, "-");
    if (strstr(dados.estadoBebe, "$"))
        strcpy(dados.estadoBebe, "-");
    if (strstr(dados.dataNascimento, "$"))
        strcpy(dados.dataNascimento, "-");

    //Print formatado
    printf("Nasceu em %s/%s, em %s, um bebê de sexo %s.\n", dados.cidadeBebe, dados.estadoBebe, dados.dataNascimento, sexo);
    return;
}

void initHeader(cabecalho *cab, FILE *fp) //Disco e RAM
{
    cab->status = '0';
    fwrite(&cab->status, sizeof(char), 1, fp);

    cab->RRNproxRegistro = 0;
    fwrite(&cab->RRNproxRegistro, sizeof(int), 1, fp);

    cab->numeroRegistrosInseridos = 0;
    fwrite(&cab->numeroRegistrosInseridos, sizeof(int), 1, fp);

    cab->numeroRegistrosRemovidos = 0;
    fwrite(&cab->numeroRegistrosRemovidos, sizeof(int), 1, fp);

    cab->numeroRegistrosAtualizados = 0;
    fwrite(&cab->numeroRegistrosAtualizados, sizeof(int), 1, fp);

    trataLixo('$', cab->lixo, 111);
    fwrite(cab->lixo, sizeof(char), 111, fp);
    return;
}

void atualizaHeaderDISCO(cabecalho cab, FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, fp);
    fwrite(&cab.RRNproxRegistro, sizeof(int), 1, fp);
    fwrite(&cab.numeroRegistrosInseridos, sizeof(int), 1, fp);
    fwrite(&cab.numeroRegistrosRemovidos, sizeof(int), 1, fp);
    fwrite(&cab.numeroRegistrosAtualizados, sizeof(int), 1, fp);
    return;
}

int leitorCsv(FILE *fp, dados *dados) //le apenas UM registro
{
    //Sinaliza o fim de um arquivo ou arquivo vazio
    char c;
    c = fgetc(fp);

    if (c == -1) //EOF
        return 1;

    //leitura dos campos variaveis - indicador de tamanho
    strcpy(dados->cidadeMae, "");
    while (c != ',')
    {
        strncat(dados->cidadeMae, &c, 1);
        c = fgetc(fp);
    }
 
    dados->tamCidadeMae = strlen(dados->cidadeMae);
 
    strcpy(dados->cidadeBebe, "");
    c = fgetc(fp);
    while (c != ',')
    {
        strncat(dados->cidadeBebe, &c, 1);
        c = fgetc(fp);
    }
 
    dados->tamCidadeBebe = strlen(dados->cidadeBebe);

    //Leitura dos campos fixos

    char charTOint[5]; //guarda o inteiro desse campo como string
    strcpy(charTOint, "");
    c = fgetc(fp);
    while (c != ',')
    {
        strncat(charTOint, &c, 1);
        c = fgetc(fp);
    }
    dados->idNascimento = (int)strtol(charTOint, NULL, 10);
 
    strcpy(charTOint, "");
    c = fgetc(fp);
    while (c != ',')
    {
        strncat(charTOint, &c, 1);
        c = fgetc(fp);
    }
    dados->idadeMae = (int)strtol(charTOint, NULL, 10);
 
    int contadorBytes = 0;
    strcpy(dados->dataNascimento, "");
    c = fgetc(fp);
    while (c != ',' && contadorBytes <= 10)
    {
        strncat(dados->dataNascimento, &c, 1);
        c = fgetc(fp);
        contadorBytes++;
    }
 
    contadorBytes = 0;
    c = fgetc(fp);
    while (c != ',' && contadorBytes <= 1)
    {
        dados->sexoBebe = c;
        c = fgetc(fp);
        contadorBytes++;
    }
 
    contadorBytes = 0;
    strcpy(dados->estadoMae, "");
    c = fgetc(fp);
    while (c != ',' && contadorBytes <= 2)
    {
        strncat(dados->estadoMae, &c, 1);
        c = fgetc(fp);
        contadorBytes++;
    }
 
    contadorBytes = 0;
    strcpy(dados->estadoBebe, "");
    c = fgetc(fp);
    while (c != '\n' && contadorBytes <= 2)
    {
        strncat(dados->estadoBebe, &c, 1);
        c = fgetc(fp);
        contadorBytes++;
    }
    return 0;
}

void atualizaHeaderRAM(cabecalho *cab, char alteracao)
{
    switch (alteracao)
    {
    case 's':
        if (cab->status == '0')
            cab->status = '1';
        else
            cab->status = '0';
        break;
    case 'p':
        cab->RRNproxRegistro++;
        break;
    case 'i':
        cab->numeroRegistrosInseridos++;
        break;
    case 'r':
        cab->numeroRegistrosInseridos--;
        cab->numeroRegistrosRemovidos++;
        break;
    case 'a':
        cab->numeroRegistrosAtualizados++;
        break;
    default:
        break;
    }
    return;
}

int leituraRegistro(FILE *fp, dados *dados, int i)
{
    //Tamanho Variável
    fseek(fp, rrnToOffset(i) - 128, SEEK_SET);
    fread(&dados->tamCidadeMae, sizeof(int), 1, fp);

    //verifica registro removido
    if (dados->tamCidadeMae == -1)
        return 0;

    fread(&dados->tamCidadeBebe, sizeof(int), 1, fp);

    //Bloco de leitura byte a byte de um campo
    char aux = '\0';
    while (dados->tamCidadeMae > 0)
    {
        fread(&aux, sizeof(char), 1, fp);
        strncat(dados->cidadeMae, &aux, 1);
        dados->tamCidadeMae--;
    }

    aux = '\0';
    while (dados->tamCidadeBebe > 0)
    {
        fread(&aux, sizeof(char), 1, fp);
        strncat(dados->cidadeBebe, &aux, 1);
        dados->tamCidadeBebe--;
    }

    //Tamanho Fixo
    fseek(fp, 128 * i + 105, SEEK_SET);
    fread(&dados->idNascimento, sizeof(int), 1, fp);
    fread(&dados->idadeMae, sizeof(int), 1, fp);
    aux = '\0';
    int contador = 10;
    while (contador > 0)
    {
        fread(&aux, sizeof(char), 1, fp);
        strncat(dados->dataNascimento, &aux, 1);
        contador--;
    }

    fread(&dados->sexoBebe, sizeof(char), 1, fp);

    aux = '\0';
    contador = 2;
    while (contador > 0)
    {
        fread(&aux, sizeof(char), 1, fp);
        strncat(dados->estadoMae, &aux, 1);
        contador--;
    }

    aux = '\0';
    contador = 2;
    while (contador > 0)
    {
        fread(&aux, sizeof(char), 1, fp);
        strncat(dados->estadoBebe, &aux, 1);
        contador--;
    }
    return 1;
}

void zeraDados(dados *dados, int versao)
{
    dados->tamCidadeMae = 0;
    dados->tamCidadeBebe = 0;
    strcpy(dados->cidadeBebe, "");
    strcpy(dados->cidadeMae, "");
    dados->idNascimento = 0;
    dados->idadeMae = 0;
    strcpy(dados->dataNascimento, "");
    if (versao == 1)
        dados->sexoBebe = '0';
    else
        dados->sexoBebe = 'X'; //valor aleatorio != 1, 2 e 0
    strcpy(dados->estadoMae, "");
    strcpy(dados->estadoBebe, "");
    return;
}

//--------------------FUNÇÕES FORNECIDAS PELA PROFA-----------------------

void binarioNaTela(char *nomeArquivoBinario)
{

    /* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb")))
    {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

void trim(char *str)
{

    /*
	*	Essa função arruma uma string de entrada "str".
	*	Manda pra ela uma string que tem '\r' e ela retorna sem.
	*	Ela remove do início e do fim da string todo tipo de espaçamento (\r, \n, \t, espaço, ...).
	*	Por exemplo:
	*
	*	char minhaString[] = "    \t TESTE  DE STRING COM BARRA R     \t  \r\n ";
	*	trim(minhaString);
	*	printf("[%s]", minhaString); // vai imprimir "[TESTE  DE STRING COM BARRA R]"
	*
	*/

    size_t len;
    char *p;

    // remove espaçamentos do fim
    for (len = strlen(str); len > 0 && isspace(str[len - 1]); len--)
        ;
    str[len] = '\0';

    // remove espaçamentos do começo
    for (p = str; *p != '\0' && isspace(*p); p++)
        ;
    len = strlen(p);

    memmove(str, p, sizeof(char) * (len + 1));
}

void scan_quote_string(char *str)
{

    /*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n')
    { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // ignorar aspas fechando
    }
    else if (R != EOF)
    { // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}
