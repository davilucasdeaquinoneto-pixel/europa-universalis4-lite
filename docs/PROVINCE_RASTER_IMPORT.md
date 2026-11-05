# Importação do raster de províncias

## Objetivo desta etapa

Converter um bitmap de IDs de províncias em:
- matriz de IDs por pixel;
- contagem de pixels por província;
- lista única e determinística de fronteiras entre províncias.

O importador ainda não cria polígonos vetoriais, não lê terreno/clima e não altera o cenário ativo. Ele é a primeira etapa do pipeline geográfico.

## Formatos aceitos

### Raster BMP

- BMP Windows com cabeçalho DIB de pelo menos 40 bytes.
- 24-bit ou 32-bit.
- Sem compressão (BI_RGB).
- Orientação bottom-up ou top-down.
- Padding de linha BMP respeitado.
- RGB comparado exatamente, sem antialiasing ou tolerância de cor.

Formatos comprimidos, paletas indexadas, PNG e JPEG ainda não são suportados.

### Definições de cores

Arquivo delimitado por ponto e vírgula, no formato:

`province;red;green;blue;name;...`

O cabeçalho é opcional. Campos extras são ignorados. IDs e cores RGB precisam ser únicos. O formato aceita BOM UTF-8 e nomes entre aspas com ponto e vírgula escapado por aspas duplicadas.

## Saída

A API `importProvinceRasterBmp` preenche `ProvinceRasterData`:
- `width` e `height`;
- `pixelProvinceIds`, em ordem top-down e linha a linha;
- `provincePixelCounts`;
- `boundaries`, com pares de IDs ordenados e sem duplicatas.

Por padrão, ID 0 é tratado como fundo/oceano e não entra nas contagens nem nas fronteiras. Passe `ignoreProvinceIdZero=false` se ID 0 for uma província válida no dataset.

Cores RGB ausentes nas definições causam erro com a primeira coordenada problemática. Isso detecta paletas alteradas e pixels antialiased em vez de inventar IDs.

## Uso pela CLI

```text
import-map provinces.bmp definition.csv generated-adjacencies.csv [province-map.svg]
```

O comando gera um CSV `province_a,province_b` com as fronteiras detectadas. Se o quarto argumento for informado, também extrai os polígonos e grava um SVG plano com as províncias coloridas. O SVG usa preenchimento `evenodd`, preservando buracos. O comando não substitui automaticamente `data/adjacencies.csv` nem modifica o mundo carregado.

## Complexidade

- Leitura e mapeamento de pixels: O(W × H) esperado.
- Extração de fronteiras: O(W × H) esperado com conjunto hash de pares.
- Memória principal: um uint32 por pixel, mais a linha BMP, contagens e conjunto de fronteiras.

A matriz ocupa aproximadamente 4 bytes por pixel. Um raster de 10 milhões de pixels usa cerca de 40 MB só para IDs, sem contar estruturas auxiliares. A extração vetorial adiciona memória proporcional ao comprimento total das fronteiras, não cria um vértice para cada pixel interno.

## Contornos vetoriais implementados

`extractProvinceContours` transforma a matriz de IDs em anéis vetoriais por província:
- Fronteiras seguem as arestas exatas dos pixels; nenhuma interpolação altera a forma.
- Componentes desconectados são mantidos como anéis separados, permitindo províncias com ilhas.
- Buracos internos são identificados pela orientação do anel (`isHole`).
- Vértices colineares são removidos sem alterar a geometria ortogonal.
- A saída é ordenada por ID de província para facilitar testes e serialização determinística.
- A função valida dimensões e preserva a saída anterior em caso de falha.

Os pontos usam coordenadas do raster, com origem no canto superior esquerdo. O ponto de fechamento não é repetido no vetor.

## Próximos passos

1. Adicionar validação topológica e associação de buracos ao contorno externo correto.
2. Importar terrain/climate/positions e relacionar metadados aos IDs.
3. Criar índices espaciais e viewport 2D com seleção por pixel.
4. Integrar o raster ao renderizador sem misturar dados gráficos com o estado da simulação.
