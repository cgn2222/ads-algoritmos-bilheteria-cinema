#include "bilheteria.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool ler_inteiro_nao_negativo(const char *prompt, int *valor_out) {
    char linha[64];
    for (;;) {
        printf("%s", prompt);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            return false;
        }
        char *p = linha;
        while (*p != '\0' && isspace((unsigned char)*p)) {
            p++;
        }
        if (*p == '\0') {
            printf("  [AVISO] Informe um numero inteiro >= 0.\n");
            continue;
        }
        errno = 0;
        char *fim = NULL;
        long val = strtol(p, &fim, 10);
        while (fim != NULL && *fim != '\0' && isspace((unsigned char)*fim)) {
            fim++;
        }
        if (errno != 0 || fim == p || (fim != NULL && *fim != '\0') ||
            val < 0 || val > INT_MAX) {
            printf("  [AVISO] Quantidade invalida. Digite um inteiro >= 0.\n");
            continue;
        }
        *valor_out = (int)val;
        return true;
    }
}

static void imprimir_relatorio(
    const char filmes[BILHETERIA_NUM_FILMES][32],
    const char dias[BILHETERIA_NUM_DIAS][16],
    const MatrizBilheteria *matriz,
    const RelatorioBilheteria *rel) {
    char preco_buf[32];
    char fat_buf[32];
    char total_fat_buf[32];

    bilheteria_formatar_reais(rel->preco_ingresso_centavos, preco_buf, sizeof(preco_buf));
    bilheteria_formatar_reais(rel->faturamento_total_centavos, total_fat_buf, sizeof(total_fat_buf));

    printf("=========================================================================================\n");
    printf("                  SISTEMA DE CONTROLE DE BILHETERIA E VENDAS DE CINEMA                   \n");
    printf("=========================================================================================\n");
    printf("Valor padrao do ingresso: R$ %s\n\n", preco_buf);

    printf("%-28s | %4s | %4s | %4s | %4s | %4s | %6s | %12s | %6s\n",
           "FILME / DIAS", "Qua", "Qui", "Sex", "Sab", "Dom", "TOTAL", "FATURAMENTO", "SHARE");
    printf("-----------------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < BILHETERIA_NUM_FILMES; i++) {
        bilheteria_formatar_reais(rel->faturamento_filme_centavos[i], fat_buf, sizeof(fat_buf));
        printf("%-28s |", filmes[i]);
        for (size_t j = 0; j < BILHETERIA_NUM_DIAS; j++) {
            printf(" %4d |", matriz->ingressos[i][j]);
        }
        printf(" %6" PRId64 " | R$ %9s | %5.1f%%\n",
               rel->total_por_filme[i],
               fat_buf,
               rel->participacao_filme_pct[i]);
    }

    printf("=========================================================================================\n");
    printf("%-28s |", "TOTAL POR DIA");
    for (size_t j = 0; j < BILHETERIA_NUM_DIAS; j++) {
        printf(" %4" PRId64 " |", rel->total_por_dia[j]);
    }
    printf(" %6" PRId64 " | R$ %9s | %5.1f%%\n",
           rel->total_geral_ingressos,
           total_fat_buf,
           rel->total_geral_ingressos > 0 ? 100.0 : 0.0);
    printf("=========================================================================================\n\n");

    char campeao_fat_buf[32];
    bilheteria_formatar_reais(rel->faturamento_filme_centavos[rel->indice_filme_campeao],
                              campeao_fat_buf,
                              sizeof(campeao_fat_buf));

    printf("------------------------------ METRICAS CONSOLIDADAS ------------------------------------\n");
    printf(" -> Total Geral de Ingressos Vendidos : %" PRId64 " ingressos\n", rel->total_geral_ingressos);
    printf(" -> Receita Bruta Total Acumulada     : R$ %s\n", total_fat_buf);
    printf(" -> Media de Publico por Dia          : %.2f espectadores/dia\n", rel->media_diaria);
    printf(" -> Filme Campeao de Bilheteria       : %s (%" PRId64 " ingressos | R$ %s)\n",
           filmes[rel->indice_filme_campeao],
           rel->total_por_filme[rel->indice_filme_campeao],
           campeao_fat_buf);
    printf(" -> Dia de Maior Movimento            : %s (%" PRId64 " ingressos)\n",
           dias[rel->indice_dia_pico],
           rel->total_por_dia[rel->indice_dia_pico]);
    printf("=========================================================================================\n");
}

int main(int argc, char **argv) {
    const char filmes[BILHETERIA_NUM_FILMES][32] = {
        "Avatar: O Caminho da Agua",
        "Vingadores: Ultimato",
        "Oppenheimer"
    };

    const char dias[BILHETERIA_NUM_DIAS][16] = {
        "Quarta-feira",
        "Quinta-feira",
        "Sexta-feira",
        "Sabado",
        "Domingo"
    };

    MatrizBilheteria matriz = {{
        {120, 150, 280, 420, 390},
        {140, 180, 310, 490, 460},
        {95,  130, 240, 360, 310}
    }};

    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("Uso: %s [--interativo | --help]\n", argv[0]);
        printf("  (sem argumentos)  Exibe o relatorio semanal com a matriz padrao 3x5.\n");
        printf("  --interativo      Permite informar manualmente os ingressos por filme e dia.\n");
        return EXIT_SUCCESS;
    }

    if (argc > 1 && strcmp(argv[1], "--interativo") == 0) {
        printf("Entrada interativa de bilheteria (%d filmes x %d dias):\n\n",
               BILHETERIA_NUM_FILMES, BILHETERIA_NUM_DIAS);
        for (size_t i = 0; i < BILHETERIA_NUM_FILMES; i++) {
            printf("--- %s ---\n", filmes[i]);
            for (size_t j = 0; j < BILHETERIA_NUM_DIAS; j++) {
                char prompt[128];
                snprintf(prompt, sizeof(prompt), "  Ingressos em %-12.15s: ", dias[j]);
                if (!ler_inteiro_nao_negativo(prompt, &matriz.ingressos[i][j])) {
                    fprintf(stderr, "\n[ERRO] Entrada encerrada antes do preenchimento da matriz.\n");
                    return EXIT_FAILURE;
                }
            }
            printf("\n");
        }
    }

    RelatorioBilheteria rel;
    BilheteriaStatus st = bilheteria_processar(&matriz, BILHETERIA_PRECO_PADRAO_CENTAVOS, &rel);
    if (st != BILHETERIA_OK) {
        fprintf(stderr, "[ERRO] %s\n", bilheteria_status_mensagem(st));
        return EXIT_FAILURE;
    }

    imprimir_relatorio(filmes, dias, &matriz, &rel);
    return EXIT_SUCCESS;
}
