#!/usr/bin/env bash
set -euo pipefail

BIN="${1:-./build/bilheteria_cinema}"

echo "==> 1. Testando relatorio padrao"
OUT="$("$BIN")"
grep -q "Vingadores: Ultimato" <<<"$OUT"
grep -q "4075 ingressos" <<<"$OUT"
grep -q "R\$ 122250.00" <<<"$OUT"
grep -q "Sabado (1270 ingressos)" <<<"$OUT"
echo "[OK] Relatorio padrao validado"

echo "==> 2. Testando modo --interativo com validacao de entrada"
INTERACTIVE_OUT="$(
  "$BIN" --interativo <<'EOF'
abc
-5
10
20
30
40
50
1
2
3
4
5
100
0
0
0
0
EOF
)"
grep -q "Quantidade invalida" <<<"$INTERACTIVE_OUT"
grep -q "265 ingressos" <<<"$INTERACTIVE_OUT"
grep -q "R\$ 7950.00" <<<"$INTERACTIVE_OUT"
echo "[OK] Modo interativo validado"
