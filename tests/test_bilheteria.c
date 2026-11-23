#include "../src/bilheteria.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void test_matriz_padrao(void) {
    const MatrizBilheteria matriz = {{
        {120, 150, 280, 420, 390}, /* 1360 */
        {140, 180, 310, 490, 460}, /* 1580 */
        {95,  130, 240, 360, 310}  /* 1135 */
    }};
    RelatorioBilheteria rel;

    assert(bilheteria_processar(&matriz, BILHETERIA_PRECO_PADRAO_CENTAVOS, &rel) == BILHETERIA_OK);

    assert(rel.total_por_filme[0] == 1360);
    assert(rel.total_por_filme[1] == 1580);
    assert(rel.total_por_filme[2] == 1135);

    assert(rel.total_por_dia[0] == 355);
    assert(rel.total_por_dia[1] == 460);
    assert(rel.total_por_dia[2] == 830);
    assert(rel.total_por_dia[3] == 1270);
    assert(rel.total_por_dia[4] == 1160);

    assert(rel.total_geral_ingressos == 4075);
    assert(rel.faturamento_filme_centavos[0] == 4080000);
    assert(rel.faturamento_filme_centavos[1] == 4740000);
    assert(rel.faturamento_filme_centavos[2] == 3405000);
    assert(rel.faturamento_total_centavos == 12225000);
    assert(fabs(rel.media_diaria - 815.0) < 1e-9);
    assert(rel.indice_filme_campeao == 1);
    assert(rel.indice_dia_pico == 3);

    double soma_share = rel.participacao_filme_pct[0] +
                        rel.participacao_filme_pct[1] +
                        rel.participacao_filme_pct[2];
    assert(fabs(soma_share - 100.0) < 1e-6);
}

static void test_matriz_zerada_e_empate(void) {
    const MatrizBilheteria zeros = {{{0}}};
    RelatorioBilheteria rel;

    assert(bilheteria_processar(&zeros, 3000, &rel) == BILHETERIA_OK);
    assert(rel.total_geral_ingressos == 0);
    assert(rel.faturamento_total_centavos == 0);
    assert(rel.indice_filme_campeao == 0);
    assert(rel.indice_dia_pico == 0);
    assert(rel.participacao_filme_pct[0] == 0.0);

    /* Empate entre filme 0 e filme 2 -> determinismo preserva menor indice (0). */
    const MatrizBilheteria empate = {{
        {10, 10, 10, 10, 10},
        {5,  5,  5,  5,  5},
        {10, 10, 10, 10, 10}
    }};
    assert(bilheteria_processar(&empate, 2500, &rel) == BILHETERIA_OK);
    assert(rel.indice_filme_campeao == 0);
    assert(rel.indice_dia_pico == 0);
}

static void test_validacoes_erro(void) {
    const MatrizBilheteria invalida = {{
        {10, 20, 30, 40, 50},
        {10, -1, 30, 40, 50},
        {10, 20, 30, 40, 50}
    }};
    const MatrizBilheteria valida = {{{1}}};
    RelatorioBilheteria rel;

    assert(bilheteria_processar(NULL, 3000, &rel) == BILHETERIA_ERR_ARGUMENTO_NULO);
    assert(bilheteria_processar(&valida, 3000, NULL) == BILHETERIA_ERR_ARGUMENTO_NULO);
    assert(bilheteria_processar(&valida, 0, &rel) == BILHETERIA_ERR_PRECO_INVALIDO);
    assert(bilheteria_processar(&valida, -100, &rel) == BILHETERIA_ERR_PRECO_INVALIDO);
    assert(bilheteria_processar(&invalida, 3000, &rel) == BILHETERIA_ERR_INGRESSO_NEGATIVO);
}

static void test_formatacao_reais(void) {
    char buf[32];
    bilheteria_formatar_reais(12225000, buf, sizeof(buf));
    assert(strcmp(buf, "122250.00") == 0);
}

int main(void) {
    test_matriz_padrao();
    test_matriz_zerada_e_empate();
    test_validacoes_erro();
    test_formatacao_reais();

    printf("[OK] Todos os testes unitarios de bilheteria_cinema passaram.\n");
    return 0;
}
