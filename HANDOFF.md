# HANDOFF - Europa Universalis Lite

## Estado do projeto

Repositório: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite
Branch principal: `main`
Versão de trabalho: 0.1.0
Estado: fundação do motor e testes iniciais adicionados; compilação ainda aguarda execução do GitHub Actions ou teste local.

O repositório originalmente continha somente um README. A base foi iniciada do zero.

## Objetivo

Jogo independente de grande estratégia inspirado no gênero Europa Universalis, com simulação profunda e desempenho adequado a notebook de baixo custo. Alvo informado: Windows, C++17. A simulação deve permanecer independente da interface.

Não copiar código, mapas, imagens, textos ou outros recursos proprietários de Europa Universalis.

## Arquivos importantes

- `CMakeLists.txt`: build CMake, C++17, executável `europa-lite`, CTest.
- `.gitignore`: ignora build e arquivos locais.
- `include/core/Date.hpp`: calendário e avanço mensal.
- `include/world/World.hpp`: modelos de país, província e consultas do mundo.
- `include/simulation/Game.hpp`: API da partida.
- `src/World.cpp`: parser CSV e validação básica.
- `src/Game.cpp`: ciclo mensal, economia provisória, status, províncias e save.
- `src/main.cpp`: interface de terminal e comandos.
- `data/countries.csv`: quatro países de cenário de teste.
- `data/provinces.csv`: dez províncias de cenário de teste.
- `tests/TestCore.cpp`: testes iniciais do calendário e do carregamento do mundo.
- `.github/workflows/ci.yml`: workflow para compilar e testar em Linux e Windows.
- `docs/ARCHITECTURE.md`: decisões arquiteturais.
- `README.md`: apresentação e instruções.
 
## Como compilar

Requer CMake 3.20+ e compilador C++17. Ninja é recomendado, mas não obrigatório.

```bash
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/europa-lite
```

No Windows, o executável ficará normalmente em `build/europa-lite.exe`. Execute a partir da raiz do projeto para que a pasta `data` seja encontrada, ou passe o caminho da pasta de dados como primeiro argumento.

O GitHub Actions também tentará compilar e executar os testes em Windows e Ubuntu. Até o workflow terminar, não considerar a compilação validada.

## Comandos

- `help`
- `status`
- `provinces`
- `tick [meses]`
- `save [arquivo]`
- `quit`

## Regras provisórias

- Data inicial: 11/11/1444.
- O tempo avança em meses.
- Países recebem mensalmente: desenvolvimento total das províncias × 0,10 × modificador tributário.
- O save grava data, tesouro, efetivo de mão de obra e proprietário de cada província.

A economia é deliberadamente mínima e serve apenas para validar o ciclo de simulação.

## Testes existentes

- Data inicial.
- Avanço de novembro para dezembro.
- Virada de dezembro para janeiro.
- Carregamento dos CSVs.
- Quantidade de países e províncias.
- Busca de país por tag.
- Contagem de províncias e soma de desenvolvimento.

## Limitações conhecidas / ainda não implementado

- Não há carregamento de save.
- O parser CSV é simples e não suporta campos com vírgulas ou aspas; linhas com campos vazios no fim também precisam de tratamento específico.
- Os testes ainda não foram executados neste ambiente.
- Não há adjacências, mapa visual, regiões, comércio, produção, manutenção, diplomacia, guerra ou IA.
- A gravação de save é um formato textual inicial, sem versionamento robusto nem recuperação avançada.
- O cenário é pequeno e serve somente como teste de funcionamento, não como mapa histórico.

## Próxima etapa recomendada

1. Confirmar o resultado do workflow do GitHub Actions e corrigir qualquer falha de compilação.
2. Melhorar o parser e validar os dados com testes negativos (IDs duplicados, país inexistente, valores inválidos).
3. Criar um modelo de adjacência entre províncias e regiões.
4. Separar a economia em um módulo próprio, com receitas, despesas, manutenção e balanço.
5. Só depois ampliar o cenário europeu e iniciar a interface gráfica.

## Prompt de continuidade para outro assistente

Continue o projeto existente no repositório https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite. Leia primeiro README.md, HANDOFF.md e docs/ARCHITECTURE.md. Não crie outro projeto nem substitua a arquitetura sem justificar.

O projeto é C++17 com CMake. A simulação deve ficar independente da interface gráfica e ser eficiente em hardware fraco. A base atual contém calendário mensal, modelos de país/província, carregamento CSV, economia mensal provisória, CLI, save textual, testes iniciais e CI Windows/Linux.

Antes de implementar:
- confira o estado real do branch main;
- veja o resultado do GitHub Actions;
- compile e rode CTest;
- corrija erros reais antes de adicionar sistemas.

Próxima tarefa: endurecer o carregamento de dados e seus testes; em seguida modelar adjacências e regiões. Atualize HANDOFF.md ao concluir cada etapa. Não afirme que compilou/testou sem executar os comandos e observar o resultado. Preserve as alterações existentes e faça commits pequenos e descritivos.
