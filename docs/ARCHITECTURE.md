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
- `include/world/World.hpp`: países, hierarquia geográfica, províncias e conexões tipadas.
- `include/simulation/Game.hpp`: coordenação da partida.
- `include/map/MapPreview.hpp`: interface do exportador de mapa.
- `include/map/ProvinceRaster.hpp`: interface do importador de raster BMP e fronteiras.
- `include/map/ProvinceContours.hpp`: polígonos vetoriais extraídos dos limites de pixels.
- `src/World.cpp`: leitura e validação de CSVs.
- `src/Game.cpp`: avanço mensal, economia provisória, consultas e save.
- `src/MapPreview.cpp`: projeção isométrica e exportação SVG sem biblioteca gráfica.
- `src/ProvinceRaster.cpp`: leitura BMP 24/32-bit, associação RGB -> ID e extração de fronteiras.
- `src/ProvinceContours.cpp`: extração de anéis de contorno, ilhas e buracos, com remoção de vértices colineares.
- `src/main.cpp`: terminal interativo.
- `data/map_layout.csv`: coordenadas de apresentação e elevação visual do mapa demonstrativo.
- `data/`: países, continentes, regiões, sub-regiões, áreas, províncias, adjacências e conexões tipadas.

## Modelo geográfico

A hierarquia implementada é:

`continente -> região -> sub-região -> área -> província`

- `regions.csv` continua definindo IDs e nomes das regiões.
- `geography.csv` associa regiões a continentes e declara continentes, sub-regiões e áreas.
- `province_areas.csv` associa cada província a uma área.
- `adjacencies.csv` mantém compatibilidade com o formato antigo e representa ligações terrestres bidirecionais.
- `connections.csv` adiciona ligações terrestres, marítimas, estreitos, rios e barreiras intransponíveis, com opção de sentido único.

A carga valida IDs duplicados, referências, pais hierárquicos, compatibilidade entre a região da província e a região da área, e exige que toda província tenha uma área quando a hierarquia é habilitada. O carregamento continua transacional: uma falha não substitui o mundo já carregado.

`areAdjacent` responde se existe uma ligação física cadastrada. `hasPassableConnection` considera o tipo e o sentido, ignorando ligações intransponíveis. A lista legada `adjacentProvinceIds` é preenchida para consultas rápidas de vizinhança.

O cenário atual é apenas uma amostra fictícia de dez províncias. A estrutura está pronta para crescer, mas ainda não contém o raster, os polígonos nem as fronteiras históricas do mapa europeu.

## Apresentação do mapa: decisão técnica

A evolução visual seguirá três estágios:

1. **SVG isométrico de prova:** validar coordenadas, extrusão, paleta política, rótulos e formato de layout sem adicionar dependências.
2. **Viewport 2D interativo:** raster de IDs, fronteiras, pan/zoom, camadas e picking por pixel sem antialiasing.
3. **Terreno 2.5D/3D leve:** malha de terreno derivada de heightmap, câmera orbital limitada e seleção de províncias por ID. O renderer permanece separado do motor.

Não começar com terreno 3D completo, tesselação adaptativa, sombras caras ou backend Vulkan/WebGPU. O hardware-alvo inclui máquinas com memória e GPU limitadas; o primeiro renderer deve priorizar baixa resolução, buffers compactos e poucas dependências.

## Importador de raster

`importProvinceRasterBmp` lê BMP Windows sem compressão (24 ou 32 bits), aplica definições RGB de um arquivo delimitado por ponto e vírgula e produz IDs por pixel, contagem por província e pares de fronteira. A comparação RGB é exata; cores desconhecidas causam erro, o que ajuda a detectar antialiasing. O importador não altera o mundo ativo. `extractProvinceContours` converte os IDs por pixel em anéis vetoriais, preservando ilhas e buracos e removendo apenas pontos colineares. Consulte `docs/PROVINCE_RASTER_IMPORT.md` para os limites e formatos.

## Economia provisória

Cada país recebe mensalmente:

`desenvolvimento_total_das_províncias × 0,10 × modificador_de_impostos`

É uma regra temporária para validar o ciclo de simulação, não o modelo econômico final.

## Testes e integração contínua

O CTest verifica calendário, carregamento do cenário, quantidades de entidades, consultas por ID/tag, desenvolvimento agregado, adjacência, hierarquia geográfica, conexões direcionais, importação de raster, extração de fronteiras e polígonos (incluindo buracos e ilhas), validação de dados, preservação transacional e exportação do SVG de mapa. O GitHub Actions compila e executa os testes em Ubuntu e Windows.

## Próximos módulos

1. Validar topologia dos polígonos e exportar geometria para inspeção visual.
2. Importar terrain/climate/positions e associar metadados às províncias.
3. Criar viewport 2D com pan/zoom, seleção por ID e camadas temáticas.
4. Adicionar costas, rios, portos, estreitos e regras de navegação.
5. Implementar save/load versionado e determinismo.
6. Construir economia modular, população agregada, produção, mercados, diplomacia, tecnologia, exércitos, guerra e IA.
7. Evoluir o renderer para terreno 2.5D.
