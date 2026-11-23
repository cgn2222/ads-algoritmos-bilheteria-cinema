#ifndef BILHETERIA_H
#define BILHETERIA_H

#include <stddef.h>
#include <stdint.h>

#define BILHETERIA_NUM_FILMES 3
#define BILHETERIA_NUM_DIAS 5
#define BILHETERIA_PRECO_PADRAO_CENTAVOS ((int64_t)3000) /* R$ 30.00 */

typedef enum {
    BILHETERIA_OK = 0,
    BILHETERIA_ERR_ARGUMENTO_NULO = 1,
    BILHETERIA_ERR_PRECO_INVALIDO = 2,
    BILHETERIA_ERR_INGRESSO_NEGATIVO = 3,
    BILHETERIA_ERR_OVERFLOW = 4
} BilheteriaStatus;

typedef struct {
    int ingressos[BILHETERIA_NUM_FILMES][BILHETERIA_NUM_DIAS];
} MatrizBilheteria;

typedef struct {
    int64_t preco_ingresso_centavos;
    int64_t total_por_filme[BILHETERIA_NUM_FILMES];
    int64_t total_por_dia[BILHETERIA_NUM_DIAS];
    int64_t faturamento_filme_centavos[BILHETERIA_NUM_FILMES];
    double participacao_filme_pct[BILHETERIA_NUM_FILMES];
    int64_t total_geral_ingressos;
    int64_t faturamento_total_centavos;
    double media_diaria;
    size_t indice_filme_campeao;
    size_t indice_dia_pico;
} RelatorioBilheteria;

BilheteriaStatus bilheteria_processar(const MatrizBilheteria *matriz,
                                      int64_t preco_ingresso_centavos,
                                      RelatorioBilheteria *relatorio);

void bilheteria_formatar_reais(int64_t centavos, char *buffer, size_t tamanho);
const char *bilheteria_status_mensagem(BilheteriaStatus status);

#endif
