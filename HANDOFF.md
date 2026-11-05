# HANDOFF - Europa Universalis Lite

## Estado do projeto

Repositório: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite
Branch principal: `main`
Versão de trabalho: 0.1.0
Estado: fundação inicial criada; compilação ainda não verificada em ambiente local.

O repositório originalmente continha somente um README. A base do projeto foi iniciada do zero.

## Objetivo

Jogo de grande estratégia inspirado em Europa Universalis, com simulação profunda e desempenho adequado a notebook de baixo custo. O alvo informado é Windows em C++17. A simulação deve permanecer independente da interface.

Não copiar código, mapas, imagens, textos ou outros recursos proprietários de Europa Universalis.

## Arquivos criados

- `CMakeLists.txt`: build CMake, C++17, executável `europa-lite`.
- `.gitignore`: ignora build e arquivos locais.
- `include/core/Date.hpp`: data e avanço mensal.
- `include/world/World.hpp`: modelos de província, país e mundo.
- `include/simulation/Game.hpp`: API da partida.
- `src/World.cpp`: parser CSV e validação básica.
- `src/Game.cpp`: ciclo mensal, economia provisória, status, províncias e save.
- `src/main.cpp`: interface de terminal e comandos.
- `data/countries.csv`: quatro países ficticiamente simplificados para teste inicial.
- `data/provinces.csv`: dez províncias de teste.
- `docs/ARCHITECTURE.md`: princípios e arquitetura.

## Como compilar

Requer CMake 3.20+ e compilador C++17.

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/europa-lite
```

No Windows, o executável ficará normalmente em `build/europa-lite.exe`. Execute a partir da raiz do projeto para que a pasta `data` seja encontrada, ou passe o caminho da pasta de dados como primeiro argumento.

## Comandos disponíveis

- `help`
- `status`
- `provinces`
- `tick [meses]`
- `save [arquivo]`
- `quit`

## Regras provisórias

- Data inicial: 11/11/1444.
- O tempo avança em meses.
- Países recebem imposto mensal calculado a partir do desenvolvimento total das províncias.
- O save grava data, tesouro, efetivo de mão de obra e proprietário de cada província.

A economia é deliberadamente mínima e serve apenas para validar o ciclo de simulação.

## Limitações conhecidas / ainda não implementado

- Não há carregamento de save.
- O formato CSV atual não aceita campos contendo vírgulas ou aspas.
- Não há testes automatizados.
- Não há adjacências, mapa visual, regiões, comércio, produção, manutenção, diplomacia, guerra ou IA.
- A gravação de save é um formato textual inicial, ainda sem versionamento robusto nem recuperação de erro avançada.
- A compilação precisa ser executada em ambiente real antes de considerar esta etapa validada.

## Próxima etapa recomendada

Adicionar testes automatizados para calendário, validação de dados e economia; depois modelar adjacências entre províncias e separar a economia em um módulo próprio. Não iniciar a interface gráfica antes de consolidar o núcleo e os testes.
