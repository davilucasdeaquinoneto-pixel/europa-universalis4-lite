# Europa Universalis Lite

Projeto independente de grande estratégia inspirado no gênero de Europa Universalis, planejado para rodar em computadores modestos e priorizar a simulação.

## Estado atual

**Versão 0.1.0 - fundação do motor**

- Calendário mensal iniciado em 11/11/1444.
- Leitura e validação de países e províncias por CSV.
- Economia mensal provisória baseada em desenvolvimento e modificador tributário.
- Interface de terminal para avançar o tempo, consultar o mundo e salvar.
- Núcleo separado da futura interface gráfica.

Esta versão é uma fundação técnica, não um jogo completo. Ainda não existem mapa visual, diplomacia, guerra, comércio ou IA.

## Tecnologias

- C++17
- CMake 3.20+
- Ninja (recomendado)

## Compilar

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/europa-lite
```

No Windows, execute `build/europa-lite.exe`. Rode o programa a partir da raiz do repositório para localizar a pasta `data`.

## Comandos

| Comando | Ação |
|---|---|
| `help` | Lista os comandos |
| `status` | Mostra países e economia |
| `provinces` | Lista províncias |
| `tick 12` | Avança 12 meses |
| `save campanha.eu4lite` | Salva o estado atual |
| `quit` | Encerra |

## Estrutura

- `include/`: interfaces e modelos.
- `src/`: implementação do motor.
- `data/`: cenário de teste em CSV.
- `docs/ARCHITECTURE.md`: decisões arquiteturais.
- `HANDOFF.md`: estado técnico e instruções de continuidade.

## Princípios

A simulação não deve depender da renderização. O projeto não reutiliza código, mapas, imagens, textos ou outros recursos proprietários de jogos comerciais. O conteúdo histórico será criado a partir de fontes permitidas e dados próprios.

Consulte [HANDOFF.md](HANDOFF.md) antes de continuar o desenvolvimento.
