# HANDOFF — Europa Universalis Lite

## Repositório e estado

- Repositório: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite
- Branch principal: `main`
- Branch atual de trabalho: `feature/first-isometric-map-preview`
- Stack: C++17 + CMake.
- Objetivo: motor independente de grande estratégia, com simulação desacoplada da apresentação e desempenho em hardware modesto.

Leia também:
- `README.md`
- `docs/ARCHITECTURE.md`
- `docs/REVERSE_ENGINEERING_AUDIT.md`

## Estado do motor

- Calendário mensal iniciado em 11/11/1444.
- Quatro países, três regiões e dez províncias fictícias de demonstração.
- Adjacências bidirecionais simples.
- Economia provisória: desenvolvimento total × 0,10 × modificador tributário.
- CLI com status, lista de províncias, avanço de meses e gravação de save textual.
- Parser CSV com aspas, vírgulas internas e aspas escapadas.
- Validação de cabeçalhos, referências básicas, números finitos e BOM UTF-8.
- Carga transacional do mundo: falhas não destroem o estado anteriormente carregado.
- Índice ID -> índice de província para montagem eficiente das adjacências.
- Testes CTest e CI Windows/Linux.

## Primeiro mapa isométrico

Branch: `feature/first-isometric-map-preview`

Arquivos adicionados:
- `include/map/MapPreview.hpp`
- `src/MapPreview.cpp`
- `data/map_layout.csv`
- `tests/TestMapPreview.cpp`

Alterações:
1. Exportador SVG isométrico em C++17 puro, sem SDL/OpenGL ou dependência adicional.
2. Faces laterais extrudadas para dar aparência de mapa 2.5D antigo.
3. Cor política por proprietário, rótulos de província e legenda de países.
4. Layout visual separado do modelo de simulação.
5. Novo comando CLI: `map [arquivo.svg]`, que gera `mapa-demo.svg` por padrão.
6. Novo teste automatizado que verifica a geração do SVG.
7. CMake atualizado para compilar o exportador e executar o teste.

**Limitação importante:** o mapa é uma composição esquemática das dez províncias fictícias do cenário de teste. Não representa a geografia real da Europa, não tem polígonos históricos e não é um renderer 3D interativo. É uma primeira prova visual para validar a direção 2.5D.

## Referências técnicas examinadas

- `eu4-parser`: extração de províncias, bordas, GeoJSON e hierarquia geográfica a partir dos assets do EU4.
- `pdx-tools`: pipeline de assets, leitura de saves, dados de terreno e renderizador de mapas `pdx-map`.
- `Jomini`: referência de parsing Clausewitz/Jomini textual e binário.
- `EU5 Engine Atlas`: inventário processado de 21.332 registros de script/GUI/defines do EU5, não um motor de simulação pronto.
- `EU5 Workbench`: ferramenta de edição, busca, guias e lint, não um motor de jogo.
- `FIFE`: referência de engine isométrica C++ multiplataforma; sua arquitetura de camadas e ordenação visual é interessante, mas não foi adicionada como dependência.
- `opengl-terrain-demo`: referência de heightmap, normais, iluminação e texturização por altitude/inclinação; requer OpenGL 4.5 e é avançado demais para adotar diretamente agora.

Não presumir que funções, defines, formatos ou regras do EU5 se aplicam ao EU4 ou ao nosso motor. Guardar versão e nível de confiança de toda descoberta.

## Limites atuais

- Dez províncias fictícias; não existe mapa histórico completo.
- Sem raster real de províncias, polígonos ou coordenadas geográficas.
- Sem áreas, sub-regiões, continentes, mares, estreitos, rios ou portos.
- O mapa atual é SVG esquemático; sem pan/zoom, interação ou câmera 3D.
- Sem economia completa, população, produção, comércio, diplomacia, tecnologia, guerra ou IA.
- Save é apenas escrita textual; não há load versionado.
- O parser CSV é baseado em linha e ainda não suporta campos multilinha.
- A validação ainda é fail-fast, não um relatório agregado de todos os problemas.

## Próxima sequência obrigatória

1. Verificar CI da branch do mapa e corrigir qualquer falha.
2. Fechar a integridade do núcleo com testes para adjacências duplicadas, referências inválidas e arquivos ausentes.
3. Construir modelo geográfico: IDs estáveis, hierarquia área -> sub-região -> região -> continente e conexões terrestres/marítimas/estreitos/rios.
4. Importador nativo C++ para raster de províncias, definições RGB, terreno, clima e posições fornecidos pelo usuário a partir de uma instalação legítima do jogo.
5. Viewport 2D interativo com pan/zoom, camadas e seleção por ID.
6. Terreno 2.5D com heightmap e câmera orbital leve.
7. Save/load versionado, RNG com seed e replay.
8. Economia, população agregada, bens, mercados, diplomacia, tecnologia, exércitos, guerra e IA.

## Regras para assistentes futuros

- Trabalhar no repositório existente; não criar outro projeto.
- Manter C++17/CMake, salvo justificativa técnica forte.
- Ler este arquivo, README, arquitetura e auditoria antes de alterar a estrutura.
- Fazer mudanças pequenas, compiláveis e testáveis.
- Não adicionar renderer GPU pesado ou dependências grandes antes do modelo geográfico.
- Não simular cada habitante como objeto individual; usar agregação.
- O catálogo completo do Engine Atlas foi processado programaticamente: 21.332 registros. Isso não deve ser descrito como interpretação manual linha por linha de cada função.
- Não alegar testes, builds, commits ou CI sem observar o resultado real.
- Manter o núcleo de simulação independente da apresentação.
