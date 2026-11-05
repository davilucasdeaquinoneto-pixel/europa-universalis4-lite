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
- `include/map/MapPreview.hpp`: interface do exportador de mapa.
- `src/World.cpp`: leitura e validação de CSVs.
- `src/Game.cpp`: avanço mensal, economia provisória, consultas e save.
- `src/MapPreview.cpp`: projeção isométrica e exportação SVG sem biblioteca gráfica.
- `src/main.cpp`: terminal interativo.
- `data/map_layout.csv`: coordenadas de apresentação e elevação visual do mapa demonstrativo.
- `data/`: dados de países, regiões, províncias e adjacências.

## Modelo geográfico

Cada província tem um identificador numérico estável, nome, país proprietário, região, desenvolvimento, terreno e lista de províncias vizinhas. As fronteiras são definidas uma única vez no CSV e carregadas como relações bidirecionais.

A adjacência ainda não diferencia fronteira terrestre, estreito, rio ou rota marítima. Isso será necessário antes de implementar movimentação militar e comércio marítimo.

O layout visual fica fora de `World`: `map_layout.csv` associa cada ID a coordenadas de grade e uma elevação visual. Essa separação evita contaminar o estado da simulação com decisões de câmera ou desenho. O primeiro exportador projeta blocos em uma grade isométrica e escreve SVG. Ele é uma prova visual de conceito, não um mapa geográfico real.

## Apresentação do mapa: decisão técnica

A evolução visual seguirá três estágios:

1. **SVG isométrico de prova:** validar coordenadas, extrusão, paleta política, rótulos e formato de layout sem adicionar dependências.
2. **Viewport 2D interativo:** raster de IDs, fronteiras, pan/zoom, camadas e picking por pixel sem antialiasing.
3. **Terreno 2.5D/3D leve:** malha de terreno derivada de heightmap, câmera orbital limitada e seleção de províncias por ID. O renderer permanece separado do motor.

Não começar com terreno 3D completo, tesselação adaptativa, sombras caras ou backend Vulkan/WebGPU. O hardware-alvo inclui máquinas com memória e GPU limitadas; o primeiro renderer deve priorizar baixa resolução, buffers compactos e poucas dependências.

## Economia provisória

Cada país recebe mensalmente:

`desenvolvimento_total_das_províncias × 0,10 × modificador_de_impostos`

É uma regra temporária para validar o ciclo de simulação, não o modelo econômico final.

## Testes e integração contínua

O CTest verifica calendário, carregamento do cenário, quantidades de entidades, consultas por ID/tag, desenvolvimento agregado, adjacência bidirecional, validação de dados e exportação do SVG de mapa. O GitHub Actions compila e executa os testes em Ubuntu e Windows.

## Próximos módulos

1. Modelo geográfico com áreas, sub-regiões, regiões, continentes, costas e conexões tipadas.
2. Importador de raster RGB -> ID, polígonos e dados de terreno.
3. Viewport 2D com pan/zoom, seleção por ID e camadas temáticas.
4. Save/load versionado e determinismo.
5. Economia modular, população agregada, produção e mercados.
6. Diplomacia, tecnologia, exércitos, guerra e IA.
7. Renderização 2.5D mais completa.
