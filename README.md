# Europa Universalis Lite

Projeto independente de grande estratégia inspirado no gênero Europa Universalis, planejado para rodar em computadores modestos e priorizar a simulação.

## Estado atual

**Versão 0.2.0-dev - primeiro protótipo de mapa isométrico**

- Calendário mensal iniciado em 11/11/1444.
- Países, regiões e províncias carregados por CSV.
- Fronteiras entre províncias representadas como adjacências bidirecionais.
- Economia mensal provisória baseada em desenvolvimento e modificador tributário.
- Terminal para avançar o tempo, consultar o mundo e salvar.
- Parser CSV com campos entre aspas, vírgulas internas e aspas escapadas.
- Validação de cabeçalhos e rejeição de valores numéricos não finitos.
- Testes de regressão para entradas CSV inválidas e preservação do mundo em falha de carga.
- **Primeiro mapa isométrico 2.5D exportado para SVG**, sem dependência gráfica externa.
- Layout visual separado dos dados de simulação em `data/map_layout.csv`.

O mapa atual usa dez províncias fictícias de demonstração. Ele valida projeção isométrica, extrusão visual, cores por país, rótulos e exportação. **Não é o mapa histórico da Europa e ainda não é um renderizador 3D interativo.**

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
| `quit` | Encerra |

O SVG pode ser aberto em um navegador. O layout aceita as colunas `province_id,grid_x,grid_y,elevation`; coordenadas são posições de apresentação em uma grade isométrica, não latitude/longitude.

## Estrutura

- `include/`: interfaces e modelos.
- `src/`: implementação do motor e exportador de mapa.
- `data/`: países, regiões, províncias, adjacências e layout visual.
- `tests/`: testes automatizados.
- `docs/ARCHITECTURE.md`: decisões arquiteturais.
- `docs/REVERSE_ENGINEERING_AUDIT.md`: auditoria de referências técnicas.
- `HANDOFF.md`: estado técnico e instruções de continuidade.

## Próximas etapas

1. Fechar o modelo geográfico e importar dados de mapa fornecidos legalmente pelo usuário.
2. Trocar o layout demonstrativo por raster de províncias, hierarquia geográfica e conexões tipadas.
3. Criar viewport interativo 2D com pan, zoom, camadas e seleção por ID.
4. Evoluir a apresentação para terreno 2.5D com câmera, iluminação e relevo real.
5. Só depois avaliar um renderizador 3D mais completo.

## Princípios

A simulação não depende da renderização. O projeto não reutiliza código, mapas, imagens, textos ou outros recursos proprietários de jogos comerciais. O conteúdo histórico será criado a partir de fontes permitidas e dados próprios.

Consulte [HANDOFF.md](HANDOFF.md) antes de continuar o desenvolvimento.

A auditoria dos subsistemas e o plano de engenharia reversa estão em [docs/REVERSE_ENGINEERING_AUDIT.md](docs/REVERSE_ENGINEERING_AUDIT.md).
