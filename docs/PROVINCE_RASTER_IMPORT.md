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
import-map provinces.bmp definition.csv generated-adjacencies.csv
```

O comando gera um CSV `province_a,province_b` com as fronteiras detectadas. Ele não substitui automaticamente `data/adjacencies.csv` nem modifica o mundo carregado.

## Complexidade

- Leitura e mapeamento de pixels: O(W × H) esperado.
- Extração de fronteiras: O(W × H) esperado com conjunto hash de pares.
- Memória principal: um uint32 por pixel, mais a linha BMP, contagens e conjunto de fronteiras.

A matriz ocupa aproximadamente 4 bytes por pixel. Um raster de 10 milhões de pixels usa cerca de 40 MB só para IDs, sem contar estruturas auxiliares.

## Próximos passos

1. Gerar máscaras e contornos por província.
2. Converter contornos em polígonos simplificados e validar topologia.
3. Importar terrain/climate/positions e relacionar metadados aos IDs.
4. Criar índices espaciais e viewport 2D com seleção por pixel.
5. Integrar o raster ao renderizador sem misturar dados gráficos com o estado da simulação.
