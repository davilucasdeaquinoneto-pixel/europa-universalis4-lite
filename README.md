# Europa Universalis Lite

Projeto independente de grande estratégia inspirado no gênero Europa Universalis, planejado para rodar em computadores modestos e priorizar a simulação.

## Estado atual

**Versão 0.4.0-dev - importador inicial de raster de províncias**

- Calendário mensal iniciado em 11/11/1444.
- Países, continentes, regiões, sub-regiões, áreas e províncias carregados por CSV.
- Ligações entre províncias tipadas como terrestres, marítimas, estreitos, rios ou intransponíveis, com suporte a sentido único.
- Relação de cada província com sua área e hierarquia geográfica validada na carga.
- Economia mensal provisória baseada em desenvolvimento e modificador tributário.
- Terminal para avançar o tempo, consultar o mundo e salvar.
- Parser CSV com campos entre aspas, vírgulas internas e aspas escapadas.
- Validação de cabeçalhos e rejeição de valores numéricos não finitos.
- Testes de regressão para entradas CSV inválidas e preservação do mundo em falha de carga.
- **Primeiro mapa isométrico 2.5D exportado para SVG**, sem dependência gráfica externa.
- Layout visual separado dos dados de simulação em `data/map_layout.csv`.
- Importador C++17 de raster BMP 24/32-bit com definição RGB exata.
- Extração de fronteiras únicas e contagem de pixels por província.
- Testes de raster bottom-up/top-down, padding BMP, cores ausentes e preservação do resultado em falha.

O cenário geográfico atual ainda usa dez províncias fictícias de demonstração. Ele valida projeção isométrica, extrusão visual, cores por país, rótulos e exportação. **Não é o mapa histórico da Europa e ainda não é um renderizador 3D interativo.**

## Tecnologias

- C++17
- CMake 3.20+
- Ninja (recomendado)

## Compilar

```bash
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/europa-lite
```

No Windows, execute `build/europa-lite.exe`. Rode o programa a partir da raiz do repositório para localizar a pasta `data`.

## Comandos

| Comando | Ação |
|---|---|
| `help` | Lista os comandos |
| `status` | Mostra países e economia |
| `provinces` | Lista províncias, regiões e vizinhos |
| `tick 12` | Avança 12 meses |
| `save campanha.eu4lite` | Salva o estado atual |
| `map` | Gera `mapa-demo.svg` na pasta atual |
| `map europa.svg` | Gera o mapa com outro nome |
| `import-map provinces.bmp definition.csv borders.csv` | Extrai fronteiras de um raster e grava CSV |
| `quit` | Encerra |

O SVG pode ser aberto em um navegador. O layout aceita as colunas `province_id,grid_x,grid_y,elevation`; coordenadas são posições de apresentação em uma grade isométrica, não latitude/longitude.

## Estrutura

- `include/`: interfaces e modelos.
- `src/`: implementação do motor e exportador de mapa.
- `data/`: países, hierarquia geográfica, províncias, áreas, conexões tipadas, adjacências e layout visual.
- `tests/`: testes automatizados.
- `docs/ARCHITECTURE.md`: decisões arquiteturais.
- `docs/GEOGRAPHY_DATA_FORMAT.md`: formatos CSV da hierarquia e conexões geográficas.
- `docs/PROVINCE_RASTER_IMPORT.md`: formatos BMP/definition.csv e pipeline de importação.
- `docs/REVERSE_ENGINEERING_AUDIT.md`: auditoria de referências técnicas.
- `docs/MAP_RENDERING_RESEARCH.md`: pesquisa complementar de engines isométricas e ferramentas de mapa.
- `HANDOFF.md`: estado técnico e instruções de continuidade.

## Próximas etapas

1. Gerar máscaras e polígonos simplificados a partir das fronteiras extraídas.
2. Importar terreno, clima e posições e relacioná-los aos IDs.
3. Trocar o cenário fictício por dados geográficos próprios ou fornecidos legalmente pelo usuário.
4. Criar viewport interativo 2D com pan, zoom, camadas e seleção por ID.
5. Evoluir a apresentação para terreno 2.5D com câmera, iluminação e relevo real.
6. Só depois avaliar um renderizador 3D mais completo.

## Princípios

A simulação não depende da renderização. O projeto não reutiliza código, mapas, imagens, textos ou outros recursos proprietários de jogos comerciais. O conteúdo histórico será criado a partir de fontes permitidas e dados próprios.

Consulte [HANDOFF.md](HANDOFF.md) antes de continuar o desenvolvimento.

A auditoria dos subsistemas e o plano de engenharia reversa estão em [docs/REVERSE_ENGINEERING_AUDIT.md](docs/REVERSE_ENGINEERING_AUDIT.md).
