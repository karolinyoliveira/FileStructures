// Karoliny Oliveira Ozias Silva - 10368020
#include "headerOA.h"

void initHeaderBTree(cabecalhoBTree *cab, FILE *fp)
{
	cab->status = '0';
	fwrite(&cab->status, sizeof(char), 1, fp);

	cab->noRaiz = -1;
	fwrite(&cab->noRaiz, sizeof(int), 1, fp);

	cab->nroNiveis = 0;
	fwrite(&cab->nroNiveis, sizeof(int), 1, fp);

	cab->proxRRN = 0;
	fwrite(&cab->proxRRN, sizeof(int), 1, fp);

	cab->nroChaves = 0;
	fwrite(&cab->nroChaves, sizeof(int), 1, fp);

	trataLixo('$', cab->lixo, 55);
	fwrite(cab->lixo, sizeof(char), 55, fp);
	return;
}

void atualizaHeaderRAMBTree(cabecalhoBTree *cab, char alteracao)
{
	switch (alteracao)
	{
	case 's':
		if (cab->status == '0')
			cab->status = '1';
		else
			cab->status = '0';
		break;
	case 'n':
		cab->nroNiveis++;
		break;
	case 'p':
		cab->proxRRN++;
		break;
	case 'c':
		cab->nroChaves++;
		break;
	default:
		break;
	}
	return;
}

int avaliaIndice(FILE *pointer, int versao)
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

	if (bytes == 72) //Só tem o header -> arquivo sem registros
	{
		if (versao == 2)
			printf("Registro inexistente.");
		return 0;
	}

	long qtdeRegistros = (bytes - 72) / 72;
	fseek(pointer, 72, SEEK_SET);
	return qtdeRegistros;
}

void leituraHeaderBTree(FILE *fp, cabecalhoBTree *cab)
{
	fseek(fp, 0, SEEK_SET);

	fread(&cab->status, sizeof(char), 1, fp);
	fread(&cab->noRaiz, sizeof(int), 1, fp);
	fread(&cab->nroNiveis, sizeof(int), 1, fp);
	fread(&cab->proxRRN, sizeof(int), 1, fp);
	fread(&cab->nroChaves, sizeof(int), 1, fp);
	return;
}

void leituraRegistroBTree(FILE *fp, dadosBTree *reg, int RRN)
{
	fseek(fp, 72 * (RRN + 1), SEEK_SET);

	fread(&reg->nivel, sizeof(int), 1, fp);
	fread(&reg->n, sizeof(int), 1, fp);

	//leitura de n chaves e ponteiros de referencia ao arq de dados
	for (int i = 0; i < 5; i++)
	{
		fread(&reg->C[i], sizeof(int), 1, fp);
		fread(&reg->Pr[i], sizeof(int), 1, fp);
	}

	for (int i = 0; i < 6; i++)
	{
		fread(&reg->P[i], sizeof(int), 1, fp);
	}

	return;
}

int buscaBTree(FILE *fp, cabecalhoBTree cab, int *qtdePgs, int valor)
{
	dadosBTree reg;
	leituraRegistroBTree(fp, &reg, cab.noRaiz); //toda busca começa no nó raiz

	//Verifica as chaves do nó raiz e vai descendo a árvore até encontrar o valor ou retorna -1
	int i = 0;
	while (*qtdePgs <= cab.nroNiveis) //loop de niveis
	{
		while (valor >= reg.C[i] && i < 5 && reg.C[i] != -1) //loop de chaves de um nó
		{
			if (valor == reg.C[i])
			{
				return reg.Pr[i];
			}
			i++;
		}
		if (reg.P[i] == -1)
			return -1;
		leituraRegistroBTree(fp, &reg, reg.P[i]);
		(*qtdePgs)++;
		i = 0;
	}
	return -1;
}

void atualizaHeaderDiscoBTREE(cabecalhoBTree cab, FILE *fp)
{
	fseek(fp, 0, SEEK_SET);
	fwrite(&cab.status, sizeof(char), 1, fp);
	fwrite(&cab.noRaiz, sizeof(int), 1, fp);
	fwrite(&cab.nroNiveis, sizeof(int), 1, fp);
	fwrite(&cab.proxRRN, sizeof(int), 1, fp);
	fwrite(&cab.nroChaves, sizeof(int), 1, fp);

	return;
}

int leituraChaveDeBusca(FILE *fp, dados *dados, int i)
{
	//verifica registro removido
	fseek(fp, rrnToOffset(i) - 128, SEEK_SET);
	fread(&dados->tamCidadeMae, sizeof(int), 1, fp);
	if (dados->tamCidadeMae == -1)
		return 0;

	//leitura da chave de busca (IdNascimento)
	fseek(fp, 128 * i + 105, SEEK_SET);
	fread(&dados->idNascimento, sizeof(int), 1, fp);

	return 1;
}

void insereBTREE(FILE *fp, cabecalhoBTree *header, int chave, int rrn, int *sinalizador, int filhos[2], int *trilha, int profundidade)
{
	dadosBTree primeiroNo = {0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	if (header->noRaiz == -1) //Primeiro nó
	{
		//Atualização do cabeçalho (RAM)
		header->noRaiz = 0;
		header->nroNiveis = 1;
		header->proxRRN++;

		//Inserção de registro de dados (DISCO)
		primeiroNo.nivel = 1;
		primeiroNo.n = 1;
		primeiroNo.C[0] = chave;
		primeiroNo.Pr[0] = rrn;
		insereNovaPagina(primeiroNo, fp, 0);
		return;
	}

	//Encontra o nó "folha" para inserção da chave e armazena os rrns que foram percorridos até ele
	int rrnInsercao = 0;
	if (*sinalizador == 0) //Quando não é uma inserção para promoção de chave
	{
		rrnInsercao = buscaNoFolha(fp, *header, chave, trilha, profundidade);
		leituraPaginaDeDisco(&primeiroNo, fp, rrnInsercao);
		(*sinalizador) = -1;
	}
	else
	{
		int i = 0;
		while (trilha[i] != -1 && i <= profundidade)
			i++;
		if (i > 0)
			i--;

		leituraPaginaDeDisco(&primeiroNo, fp, trilha[i]);
		rrnInsercao = trilha[i];
		trilha[i] = -1;
	}

	//Inserção
	if (primeiroNo.n < 5) //tem espaço no nó
	{
		primeiroNo.C[primeiroNo.n] = chave;
		primeiroNo.Pr[primeiroNo.n] = rrn;
		primeiroNo.P[primeiroNo.n + 1] = filhos[1];
		primeiroNo.n++;
		Sorting(&primeiroNo, primeiroNo.n);
		insereNovaPagina(primeiroNo, fp, rrnInsercao);

		//Reset do valor dos nó filhos
		filhos[0] = -1;
		filhos[1] = -1;
		return;
	}
	else //Não tem espaço no nó
	{
		//Split 1-to-2 (Chaves 0 1 2 no nó antingo - 3 é promovido - 4 e 5 no nó novo)

		//Ordenação de todas as chaves/rrns envolvidos
		int chaves[6];
		int rrns[6];
		int aux;
		int P[7];
		for (int i = 0; i < 5; i++) //Parte já ordenada
		{
			chaves[i] = primeiroNo.C[i];
			rrns[i] = primeiroNo.Pr[i];
			P[i] = primeiroNo.P[i];
		}

		chaves[5] = chave;
		rrns[5] = rrn;
		P[5] = primeiroNo.P[5];
		P[6] = filhos[1];

		//Sorting da chave, rrn e P inseridos
		for (int j = 5; j > 0 && chaves[j - 1] > chaves[j]; j--)
		{
			//sorting da chave
			aux = chaves[j];
			chaves[j] = chaves[j - 1];
			chaves[j - 1] = aux;

			//sorting do rrn
			aux = rrns[j];
			rrns[j] = rrns[j - 1];
			rrns[j - 1] = aux;

			//sorting dos ponteiros das subárvores
			aux = P[j + 1];
			P[j + 1] = P[j];
			P[j] = aux;
		}

		//Criação do novo nó
		dadosBTree novoNo = {0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
		for (int i = 0; i < 2; i++)
		{
			novoNo.C[i] = chaves[i + 4];
			novoNo.Pr[i] = rrns[i + 4];
			novoNo.P[i] = P[i + 4];
			primeiroNo.P[i + 4] = -1;
		}
		novoNo.P[2] = P[6];
		novoNo.n = 2;
		novoNo.nivel = primeiroNo.nivel;

		insereNovaPagina(novoNo, fp, header->proxRRN);
		filhos[1] = header->proxRRN; //Filho da direita
		header->proxRRN++;

		//atualiza nó antigo
		for (int i = 0; i < 3; i++)
		{
			primeiroNo.C[i] = chaves[i];
			primeiroNo.Pr[i] = rrns[i];
			primeiroNo.P[i] = P[i];
		}
		primeiroNo.P[3] = P[3];

		for (int i = 0; i < 2; i++)
		{
			primeiroNo.C[i + 3] = -1;
			primeiroNo.Pr[i + 3] = -1;
		}
		primeiroNo.n = 3;
		insereNovaPagina(primeiroNo, fp, rrnInsercao);
		filhos[0] = rrnInsercao; //Filho da esquerda

		//Chave e RRN promovidos ficam no indice 3

		//Promoção - Nova Raiz
		primeiroNo.n = 3;
		if (header->noRaiz == rrnInsercao)
		{
			dadosBTree noRaiz = {0, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

			//atualizações de cabeçalho
			header->nroNiveis++;
			header->noRaiz = header->proxRRN;
			rrnInsercao = header->proxRRN;
			header->proxRRN++;

			//Inserção da chave
			noRaiz.n = 1;
			noRaiz.nivel = header->nroNiveis;
			noRaiz.C[0] = chaves[3];
			noRaiz.Pr[0] = rrns[3];

			//Inserção dos ponteiros da subárvore
			noRaiz.P[0] = filhos[0];
			noRaiz.P[1] = filhos[1];

			insereNovaPagina(noRaiz, fp, rrnInsercao);
			//Reset do valor dos nó filhos
			filhos[0] = -1;
			filhos[1] = -1;
			return;
		}

		// Promoção
		insereBTREE(fp, header, chaves[3], rrns[3], sinalizador, filhos, trilha, profundidade);
		return;
	}
}

void insereNovaPagina(dadosBTree registro, FILE *fp, int rrn)
{
	fseek(fp, 72 * (1 + rrn), SEEK_SET);
	fwrite(&registro.nivel, sizeof(int), 1, fp);
	fwrite(&registro.n, sizeof(int), 1, fp);
	for (int i = 0; i < 5; i++)
	{
		fwrite(&registro.C[i], sizeof(int), 1, fp);
		fwrite(&registro.Pr[i], sizeof(int), 1, fp);
	}
	for (int i = 0; i < 6; i++)
	{
		fwrite(&registro.P[i], sizeof(int), 1, fp);
	}
	return;
}

void leituraPaginaDeDisco(dadosBTree *registro, FILE *fp, int rrn)
{
	fseek(fp, 72 * (1 + rrn), SEEK_SET);
	fread(&registro->nivel, sizeof(int), 1, fp);
	fread(&registro->n, sizeof(int), 1, fp);
	for (int i = 0; i < 5; i++)
	{
		fread(&registro->C[i], sizeof(int), 1, fp);
		fread(&registro->Pr[i], sizeof(int), 1, fp);
	}
	for (int i = 0; i < 6; i++)
	{
		fread(&registro->P[i], sizeof(int), 1, fp);
	}
	return;
}

int buscaNoFolha(FILE *fp, cabecalhoBTree cab, int valor, int *trilha, int profundidade)
{
	dadosBTree reg;
	leituraRegistroBTree(fp, &reg, cab.noRaiz); //Leitura dos dados do nó raiz
	int i = 0, j = 0, rrnNoFolha = 0, aux;
	while (reg.nivel != 1 && reg.nivel != -208) //loop de niveis
	{
		while (valor > reg.C[i] && i < 5 && reg.C[i] != -1) //loop de chaves
		{
			i++;
		}
		aux = rrnNoFolha;
		rrnNoFolha = reg.P[i];
		trilha[j] = aux;
		j++;
		
		leituraRegistroBTree(fp, &reg, reg.P[i]);
		i = 0;
	}
	trilha[0] = cab.noRaiz;
	return rrnNoFolha;
}

void Sorting(dadosBTree *v, int n)
{
	int aux;
	if (n == 0)
		return;

	for (int j = (n - 1); j > 0 && v->C[j - 1] > v->C[j]; j--)
	{
		//sorting da chave
		aux = v->C[j];
		v->C[j] = v->C[j - 1];
		v->C[j - 1] = aux;

		//sorting do rrn do arquivo de dados
		aux = v->Pr[j];
		v->Pr[j] = v->Pr[j - 1];
		v->Pr[j - 1] = aux;

		//sorting dos ponteiros das subárvores
		aux = v->P[j + 1];
		v->P[j + 1] = v->P[j];
		v->P[j] = aux;
	}

	return;
}

