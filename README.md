# Europa Universalis Lite

Projeto independente de grande estratégia inspirado no gênero Europa Universalis, planejado para rodar em computadores modestos e priorizar a simulação.

## Estado atual

**Versão 0.1.0 - motor inicial e modelo geográfico**

- Calendário mensal iniciado em 11/11/1444.
- Países, regiões e províncias carregados por CSV.
- Fronteiras entre províncias representadas como adjacências bidirecionais.
- Economia mensal provisória baseada em desenvolvimento e modificador tributário.
- Terminal para avançar o tempo, consultar o mundo e salvar.
- Testes automatizados e integração contínua em Windows e Linux.

Esta versão é uma fundação técnica, não um jogo completo. Ainda não existem mapa visual, diplomacia, guerra, comércio ou IA.

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
| `quit` | Encerra |

## Estrutura

- `include/`: interfaces e modelos.
- `src/`: implementação do motor.
- `data/`: países, regiões, províncias e adjacências em CSV.
- `tests/`: testes automatizados.
- `docs/ARCHITECTURE.md`: decisões arquiteturais.
- `HANDOFF.md`: estado técnico e instruções de continuidade.

## Princípios

A simulação não depende da renderização. O projeto não reutiliza código, mapas, imagens, textos ou outros recursos proprietários de jogos comerciais. O conteúdo histórico será criado a partir de fontes permitidas e dados próprios.

Consulte [HANDOFF.md](HANDOFF.md) antes de continuar o desenvolvimento.
