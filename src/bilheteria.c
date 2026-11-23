#include "bilheteria.h"

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

BilheteriaStatus bilheteria_processar(const MatrizBilheteria *matriz,
                                      int64_t preco_ingresso_centavos,
                                      RelatorioBilheteria *relatorio) {
    if (matriz == NULL || relatorio == NULL) {
        return BILHETERIA_ERR_ARGUMENTO_NULO;
    }
    if (preco_ingresso_centavos <= 0) {
        return BILHETERIA_ERR_PRECO_INVALIDO;
    }

    memset(relatorio, 0, sizeof(*relatorio));
    relatorio->preco_ingresso_centavos = preco_ingresso_centavos;

    /* Percurso em ordem row-major (filme -> dia) para melhor localidade de cache. */
    for (size_t i = 0; i < BILHETERIA_NUM_FILMES; i++) {
        for (size_t j = 0; j < BILHETERIA_NUM_DIAS; j++) {
            int qtd = matriz->ingressos[i][j];
            if (qtd < 0) {
                return BILHETERIA_ERR_INGRESSO_NEGATIVO;
            }
            int64_t q64 = (int64_t)qtd;
            if (relatorio->total_geral_ingressos > INT64_MAX - q64) {
                return BILHETERIA_ERR_OVERFLOW;
            }
            relatorio->total_por_filme[i] += q64;
            relatorio->total_por_dia[j] += q64;
            relatorio->total_geral_ingressos += q64;
        }

        if (relatorio->total_por_filme[i] > INT64_MAX / preco_ingresso_centavos) {
            return BILHETERIA_ERR_OVERFLOW;
        }
        relatorio->faturamento_filme_centavos[i] =
            relatorio->total_por_filme[i] * preco_ingresso_centavos;
    }

    if (relatorio->total_geral_ingressos > INT64_MAX / preco_ingresso_centavos) {
        return BILHETERIA_ERR_OVERFLOW;
    }
    relatorio->faturamento_total_centavos =
        relatorio->total_geral_ingressos * preco_ingresso_centavos;
    relatorio->media_diaria =
        (double)relatorio->total_geral_ingressos / (double)BILHETERIA_NUM_DIAS;

    /* Determina filme campeao e participacao percentual (desempate pelo menor indice). */
    relatorio->indice_filme_campeao = 0;
    for (size_t i = 0; i < BILHETERIA_NUM_FILMES; i++) {
        if (relatorio->total_geral_ingressos > 0) {
            relatorio->participacao_filme_pct[i] =
                ((double)relatorio->total_por_filme[i] /
                 (double)relatorio->total_geral_ingressos) *
                100.0;
        }
        if (relatorio->total_por_filme[i] >
            relatorio->total_por_filme[relatorio->indice_filme_campeao]) {
            relatorio->indice_filme_campeao = i;
        }
    }

    /* Determina dia de pico (desempate pelo menor indice). */
    relatorio->indice_dia_pico = 0;
    for (size_t j = 1; j < BILHETERIA_NUM_DIAS; j++) {
        if (relatorio->total_por_dia[j] >
            relatorio->total_por_dia[relatorio->indice_dia_pico]) {
            relatorio->indice_dia_pico = j;
        }
    }

    return BILHETERIA_OK;
}

void bilheteria_formatar_reais(int64_t centavos, char *buffer, size_t tamanho) {
    if (buffer == NULL || tamanho == 0) {
        return;
    }
    uint64_t abs_val = centavos < 0
                           ? (uint64_t)(-(centavos + 1)) + UINT64_C(1)
                           : (uint64_t)centavos;
    uint64_t reais = abs_val / UINT64_C(100);
    uint64_t frac = abs_val % UINT64_C(100);
    snprintf(buffer, tamanho, "%s%" PRIu64 ".%02" PRIu64,
             centavos < 0 ? "-" : "",
             reais,
             frac);
}

const char *bilheteria_status_mensagem(BilheteriaStatus status) {
    switch (status) {
        case BILHETERIA_OK:
            return "Relatorio processado com sucesso.";
        case BILHETERIA_ERR_ARGUMENTO_NULO:
            return "Argumento nulo recebido.";
        case BILHETERIA_ERR_PRECO_INVALIDO:
            return "Preco do ingresso deve ser maior que zero.";
        case BILHETERIA_ERR_INGRESSO_NEGATIVO:
            return "Quantidade de ingressos nao pode ser negativa.";
        case BILHETERIA_ERR_OVERFLOW:
            return "Overflow aritmetico detectado nos totalizadores.";
    }
    return "Erro desconhecido.";
}
