#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cidade.h"

// Função de comparação para a ordenação das cidades
int compararCidades(const void *a, const void *b) {
    Cidade *cidadeA = (Cidade *)a;
    Cidade *cidadeB = (Cidade *)b;
    return cidadeA->Posicao - cidadeB->Posicao;
}

// Função para ler o arquivo e inicializar a estrutura Estrada
Estrada *getEstrada(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return NULL;
    }

    Estrada *estrada = (Estrada *)malloc(sizeof(Estrada));
    if (estrada == NULL) {
        printf("Erro ao alocar memória para a estrada.\n");
        fclose(arquivo);
        return NULL;
    }

    if (fscanf(arquivo, "%d", &estrada->T) != 1 || fscanf(arquivo, "%d", &estrada->N) != 1) {
        printf("Erro ao ler T ou N.\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    // Verificar as restrições
    if (estrada->T < 3 || estrada->T > 1000000 || estrada->N < 2 || estrada->N > 10000) {
        printf("Valores de T ou N fora das restrições.\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    estrada->C = (Cidade *)malloc(estrada->N * sizeof(Cidade));
    if (estrada->C == NULL) {
        printf("Erro ao alocar memória para as cidades.\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    int *posicoes = (int *)malloc(estrada->N * sizeof(int));
    if (posicoes == NULL) {
        printf("Erro ao alocar memória para verificação de posições.\n");
        free(estrada->C);
        free(estrada);
        fclose(arquivo);
        return NULL;
    }
	
	int i;
    for (i = 0; i < estrada->N; i++) {
        if (fscanf(arquivo, "%d %[^\n]", &estrada->C[i].Posicao, estrada->C[i].Nome) != 2) {
            printf("Erro ao ler posição e nome da cidade.\n");
            free(posicoes);
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }

        // Verificar se a posição está dentro dos limites e se é distinta
        if (estrada->C[i].Posicao <= 0 || estrada->C[i].Posicao >= estrada->T) {
            printf("Posição da cidade fora das restrições.\n");
            free(posicoes);
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }
		
		int j;
        for (j = 0; j < i; j++) {
            if (estrada->C[i].Posicao == posicoes[j]) {
                printf("Posições das cidades não são distintas.\n");
                free(posicoes);
                free(estrada->C);
                free(estrada);
                fclose(arquivo);
                return NULL;
            }
        }
        posicoes[i] = estrada->C[i].Posicao;
    }

    free(posicoes);
    fclose(arquivo);

    // Ordenar as cidades pela posição
    qsort(estrada->C, estrada->N, sizeof(Cidade), compararCidades);

    return estrada;
}

// Função para calcular a vizinhança entre duas posições
double calcularVizinhanca(int posicao1, int posicao2) {
    return fabs(posicao1 - posicao2) / 2.0;
}

// Função para calcular a menor vizinhança entre as cidades
double calcularMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        return -1;
    }

    double menorVizinhanca = INFINITY;

    // Calcular vizinhança entre cidades consecutivas
    double mediaVizinhanca = calcularVizinhanca(estrada->C[0].Posicao, estrada->C[1].Posicao);
    double primeiraVizinhanca = estrada->C[0].Posicao + mediaVizinhanca;

    menorVizinhanca = primeiraVizinhanca;

    if (estrada->N > 2) {
        double vizinhancaAtual;
        int i;
        for (i = 1; i < estrada->N - 1; i++) {
            vizinhancaAtual = calcularVizinhanca(estrada->C[i].Posicao, estrada->C[i + 1].Posicao) + mediaVizinhanca;
            if (vizinhancaAtual < menorVizinhanca) {
                menorVizinhanca = vizinhancaAtual;
            }
            mediaVizinhanca = calcularVizinhanca(estrada->C[i].Posicao, estrada->C[i + 1].Posicao);
        }

        // Calcular vizinhança da última cidade com o final da estrada
        vizinhancaAtual = (estrada->T - estrada->C[estrada->N - 1].Posicao) + mediaVizinhanca;
        if (vizinhancaAtual < menorVizinhanca) {
            menorVizinhanca = vizinhancaAtual;
        }
    } else {
        double segundaVizinhanca = (estrada->T - estrada->C[1].Posicao) + mediaVizinhanca;
        if (primeiraVizinhanca > segundaVizinhanca) {
            menorVizinhanca = segundaVizinhanca;
        }
    }

    free(estrada->C);
    free(estrada);

    return menorVizinhanca;
}

// Função para encontrar a cidade com a menor vizinhança
char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        return NULL;
    }

    int indiceMenorVizinhanca = 0;
    double menorVizinhanca = INFINITY;

    double mediaVizinhanca = calcularVizinhanca(estrada->C[0].Posicao, estrada->C[1].Posicao);
    double primeiraVizinhanca = estrada->C[0].Posicao + mediaVizinhanca;

    menorVizinhanca = primeiraVizinhanca;

    if (estrada->N > 2) {
        double vizinhancaAtual;
        int i;
        for (i = 1; i < estrada->N - 1; i++) {
            vizinhancaAtual = calcularVizinhanca(estrada->C[i].Posicao, estrada->C[i + 1].Posicao) + mediaVizinhanca;
            if (vizinhancaAtual < menorVizinhanca) {
                menorVizinhanca = vizinhancaAtual;
                indiceMenorVizinhanca = i;
            }
            mediaVizinhanca = calcularVizinhanca(estrada->C[i].Posicao, estrada->C[i + 1].Posicao);
        }

        vizinhancaAtual = (estrada->T - estrada->C[estrada->N - 1].Posicao) + mediaVizinhanca;
        if (vizinhancaAtual < menorVizinhanca) {
            menorVizinhanca = vizinhancaAtual;
            indiceMenorVizinhanca = estrada->N - 1;
        }
    } else {
        double segundaVizinhanca = (estrada->T - estrada->C[1].Posicao) + mediaVizinhanca;
        if (primeiraVizinhanca > segundaVizinhanca) {
            menorVizinhanca = segundaVizinhanca;
            indiceMenorVizinhanca = 1;
        } else {
            menorVizinhanca = primeiraVizinhanca;
            indiceMenorVizinhanca = 0;
        }
    }

    char *cidadeMenorVizinhanca = strdup(estrada->C[indiceMenorVizinhanca].Nome);

    free(estrada->C);
    free(estrada);

    return cidadeMenorVizinhanca;
}
