# Arquitetura inicial

## Objetivo

Construir um jogo de grande estratégia independente, inspirado no gênero Europa Universalis, priorizando profundidade de simulação e baixo consumo de recursos. O projeto não reutiliza código, dados ou recursos proprietários de outros jogos.

## Princípios

1. **Simulação separada da apresentação:** o núcleo não depende de SDL2, OpenGL ou de uma janela.
2. **Dados externos:** países, regiões, províncias e fronteiras são carregados de CSV.
3. **Sistemas pequenos:** calendário, economia, diplomacia, guerra e IA terão módulos próprios.
4. **Desempenho previsível:** evitar trabalho gráfico durante a atualização do mundo.
5. **Testabilidade:** regras podem ser verificadas sem iniciar a interface.

## Estrutura atual

- `include/core/Date.hpp`: calendário do jogo.
- `include/world/World.hpp`: países, regiões, províncias e relações de adjacência.
- `include/simulation/Game.hpp`: coordenação da partida.
- `src/World.cpp`: leitura e validação de CSVs.
- `src/Game.cpp`: avanço mensal, economia provisória, consultas e save.
- `src/main.cpp`: terminal interativo.
- `data/countries.csv`: países.
- `data/regions.csv`: regiões geográficas.
- `data/provinces.csv`: províncias com proprietário, região, desenvolvimento e terreno.
- `data/adjacencies.csv`: fronteiras terrestres entre províncias.
- `tests/TestCore.cpp`: testes do calendário e do modelo geográfico.

## Modelo geográfico

Cada província tem um identificador numérico estável, nome, país proprietário, região, desenvolvimento, terreno e lista de províncias vizinhas. As fronteiras são definidas uma única vez no CSV e carregadas como relações bidirecionais.

O carregador rejeita IDs repetidos, países ou regiões inexistentes, províncias inexistentes nas adjacências, ligações de uma província consigo mesma e ligações duplicadas.

A adjacência ainda não diferencia fronteira terrestre, estreito, rio ou rota marítima. Isso será necessário antes de implementar movimentação militar e comércio marítimo.

## Economia provisória

Cada país recebe mensalmente:

`desenvolvimento_total_das_províncias × 0,10 × modificador_de_impostos`

É uma regra temporária para validar o ciclo de simulação, não o modelo econômico final.

## Testes e integração contínua

O CTest verifica calendário, carregamento do cenário, quantidades de entidades, consultas por ID/tag, desenvolvimento agregado e adjacência bidirecional. O GitHub Actions compila e executa os testes em Ubuntu e Windows.

## Próximos módulos

1. Melhorar validação e diagnóstico de dados.
2. Adicionar tipos de fronteira e regiões/áreas históricas.
3. Economia modular com produção, comércio, despesas e manutenção.
4. Diplomacia, relações e tratados.
5. Exércitos, movimentação, combate e cercos.
6. IA estratégica por país.
7. Interface gráfica e mapa interativo.
