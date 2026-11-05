# HANDOFF — Europa Universalis Lite

## Repositório e estado

- Repositório: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite
- Branch principal: `main`
- Branch de pesquisa e robustez: `research/subsystem-audit-and-world-hardening`
- Stack: C++17 + CMake.
- Objetivo: motor independente de grande estratégia, com simulação desacoplada da apresentação e desempenho em hardware modesto.

Leia também:
- `README.md`
- `docs/ARCHITECTURE.md`
- `docs/REVERSE_ENGINEERING_AUDIT.md`

## Estado do motor em main

- Calendário mensal iniciado em 11/11/1444.
- Quatro países, três regiões e dez províncias de demonstração.
- Adjacências bidirecionais simples.
- Economia provisória: desenvolvimento total × 0,10 × modificador tributário.
- CLI com status, lista de províncias, avanço de meses e gravação de save textual.
- Parser CSV com aspas, vírgulas internas e aspas escapadas.
- Validação de cabeçalhos, referências básicas e rejeição de NaN/infinito.
- Testes CTest e CI Windows/Linux.

A CI da etapa anterior passou em Windows e Ubuntu no run:
https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite/actions/runs/42033054808

Isso não valida automaticamente a branch de pesquisa abaixo.

## Alterações na branch research/subsystem-audit-and-world-hardening

1. **Carga transacional de World:** a carga ocorre em um objeto candidato. O mundo existente só é substituído depois que todos os arquivos e referências são aceitos. Uma falha mantém o estado anterior.
2. **Índice de províncias durante leitura das adjacências:** a montagem das arestas usa ID -> índice do vetor, evitando percorrer todas as províncias para cada conexão.
3. **BOM UTF-8:** o BOM é removido no cabeçalho de cada CSV.
4. **Testes novos:** cabeçalho com BOM e preservação do mundo depois de uma carga inválida.
5. **Auditoria técnica:** `docs/REVERSE_ENGINEERING_AUDIT.md` documenta as fontes, os subsistemas, a arquitetura proposta e as prioridades de integração.

**Status:** PR #2 foi integrado à `main`. GitHub Actions run 42081910845 passou em Ubuntu e Windows (Configure, Build e Test).

## O que os projetos de referência realmente oferecem

- `eu4-parser`: extração de províncias, bordas, GeoJSON e hierarquia geográfica a partir dos assets do EU4.
- `pdx-tools`: pipeline de assets, leitura de saves, dados de terreno e renderizador de mapas `pdx-map`.
- `Jomini`: referência de parsing Clausewitz/Jomini textual e binário, com diferenças importantes entre gerações de jogos.
- `EU5 Engine Atlas`: catálogo de 21.328 registros de script/GUI/defines do EU5, não um motor de simulação pronto.
- `EU5 Workbench`: ferramenta de edição, busca, guias e lint, não um motor de jogo.

Não presumir que funções, defines, formatos ou regras do EU5 se aplicam ao EU4 ou ao nosso motor. Guardar versão e nível de confiança de toda descoberta.

## Limites atuais

- Dez províncias fictícias; não existe mapa histórico completo.
- Sem raster de províncias, polígonos ou coordenadas.
- Sem áreas, sub-regiões, continentes, mares, estreitos, rios ou portos.
- Sem renderizador gráfico.
- Sem economia completa, população, produção, comércio, diplomacia, tecnologia, guerra ou IA.
- Save é apenas escrita textual; não há load versionado.
- O parser CSV é baseado em linha e ainda não suporta campos multilinha.
- A validação ainda é fail-fast, não um relatório agregado de todos os problemas.

## Próxima sequência obrigatória

### 1. Fechar a integridade do núcleo
- Verificar CI da branch de pesquisa.
- Adicionar testes para IDs duplicados, referências inválidas, adjacências duplicadas e arquivos ausentes.
- Considerar diagnóstico agregado por arquivo/linha.
- Adicionar benchmark de carga com milhares de províncias.

### 2. Construir o modelo geográfico
- IDs estáveis/tipados.
- Hierarquia: área -> sub-região -> região -> continente.
- Conexões tipadas: terrestre, marítima, estreito, rio e bloqueio.
- Costas, portos, mares, lagos, clima e terreno.
- Importador de raster RGB -> ID e metadados.
- Construção de fronteiras e polígonos.
- Pathfinding e distâncias.

### 3. Persistência e determinismo
- Save/load versionado.
- RNG com seed persistente.
- Replay de comandos e hash de estado.

### 4. Simulação
- Economia modular.
- População agregada por província e grupo.
- Produção, bens e mercados.
- Diplomacia, tecnologia, exércitos, guerra e cercos.
- IA com orçamento de CPU por tick.

### 5. Interface
- CLI de depuração.
- Mapa 2D desacoplado do motor.
- Pan/zoom, picking por ID e camadas temáticas.

## Regras para assistentes futuros

- Trabalhar no repositório existente; não criar outro projeto.
- Manter C++17/CMake, salvo justificativa técnica forte.
- Ler este arquivo, README, arquitetura e auditoria antes de alterar a estrutura.
- Fazer mudanças pequenas, compiláveis e testáveis.
- Não adicionar renderer GPU pesado ou dependências grandes antes do modelo geográfico.
- Não simular cada habitante como objeto individual; usar agregação.
- O catálogo completo do Engine Atlas foi processado programaticamente nesta auditoria: 21.332 registros. Isso não deve ser descrito como interpretação manual linha por linha de cada função.
- Não alegar testes, builds, commits ou CI sem observar o resultado real.
- Manter o núcleo de simulação independente da apresentação.
