# Arquitetura inicial

## Objetivo

Construir um jogo de grande estratégia inspirado no gênero de Europa Universalis, priorizando profundidade de simulação e baixo consumo de recursos. O projeto é independente e não reutiliza código, dados ou recursos proprietários de outros jogos.

## Princípios

1. **Simulação separada da apresentação:** o núcleo não depende de SDL2, OpenGL ou de uma janela.
2. **Dados externos:** países e províncias começam em CSV; conteúdo histórico poderá crescer sem recompilar o motor.
3. **Sistemas pequenos:** calendário, economia, diplomacia, guerra e IA terão módulos próprios.
4. **Desempenho previsível:** evitar alocações e trabalho gráfico desnecessários durante cada atualização.
5. **Testabilidade:** regras de simulação devem poder ser testadas sem iniciar a interface.

## Estrutura atual

- `include/core/Date.hpp`: calendário do jogo.
- `include/world/World.hpp`: estruturas de país, província e consultas do mundo.
- `include/simulation/Game.hpp`: fachada que coordena a partida.
- `src/World.cpp`: leitura e validação dos CSVs.
- `src/Game.cpp`: avanço mensal, economia inicial, listagem e gravação de save.
- `src/main.cpp`: terminal interativo para testar o motor.
- `data/`: cenário mínimo de teste.

## Economia provisória

Cada país recebe mensalmente:

`desenvolvimento_total_das_províncias × 0,10 × modificador_de_impostos`

É apenas uma regra de teste para validar o ciclo mensal. Não representa o modelo econômico final.

## Próximos módulos

1. Testes automatizados e carregamento de configuração.
2. Identificadores estáveis e relações entre províncias (adjacências).
3. Regiões, áreas e mapa geográfico.
4. Produção, comércio, manutenção e orçamento detalhado.
5. Diplomacia, relações e tratados.
6. Exércitos, movimentação, combate e cercos.
7. IA estratégica e planejamento por país.
8. Interface gráfica e mapa interativo.
