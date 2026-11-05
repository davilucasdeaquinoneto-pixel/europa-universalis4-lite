# HANDOFF - Europa Universalis Lite

## Estado do projeto

Repositório: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite
Branch: `main`
Versão de trabalho: 0.1.1-dev
Estado: motor inicial e modelo geográfico compilam e passam nos testes em Linux e Windows via GitHub Actions.

O repositório originalmente continha somente um README. A base foi iniciada do zero.

## Objetivo

Jogo independente de grande estratégia inspirado no gênero Europa Universalis, com simulação profunda e desempenho adequado a notebook de baixo custo. Alvo informado: Windows, C++17. A simulação deve permanecer independente da interface.

Não copiar código, mapas, imagens, textos ou outros recursos proprietários de Europa Universalis.

## Arquivos importantes

- `CMakeLists.txt`: build CMake, C++17, executável `europa-lite`, CTest.
- `.gitignore`: ignora build e arquivos locais.
- `include/core/Date.hpp`: calendário e avanço mensal.
- `include/world/World.hpp`: países, regiões, províncias e adjacências.
- `include/simulation/Game.hpp`: API da partida.
- `src/World.cpp`: parser CSV e validação do cenário.
- `src/Game.cpp`: ciclo mensal, economia provisória, status, províncias e save.
- `src/main.cpp`: interface de terminal e comandos.
- `data/countries.csv`: quatro países de teste.
- `data/regions.csv`: três regiões de teste.
- `data/provinces.csv`: dez províncias de teste.
- `data/adjacencies.csv`: grafo inicial de fronteiras.
- `tests/TestCore.cpp`: testes de calendário, entidades, desenvolvimento e adjacência.\n- `tests/TestWorldValidation.cpp`: testes de regressão do parser CSV e dados inválidos.
- `.github/workflows/ci.yml`: compilação e testes em Linux e Windows.
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

No Windows, execute `build/europa-lite.exe`. Rode a partir da raiz do repositório para localizar a pasta `data`.

## Comandos

- `help`
- `status`
- `provinces`
- `tick [meses]`
- `save [arquivo]`
- `quit`

## Sistemas implementados

- Data inicial 11/11/1444 e avanço mensal.
- Países com tesouro, efetivo de mão de obra e modificador tributário.
- Regiões geográficas.
- Províncias com dono, região, desenvolvimento e terreno.
- Adjacência bidirecional entre províncias, carregada de CSV.
- Economia mensal temporária: desenvolvimento total × 0,10 × modificador tributário.
- Consulta de status e listagem geográfica no terminal.
- Save textual inicial.
- Testes automatizados e CI para Ubuntu/Windows.

## Trabalho atual: fortalecimento do carregador CSV (branch `hardening/data-loader-validation`)\n\n- Parser CSV com suporte a campos entre aspas, vírgulas internas e aspas escapadas.\n- Rejeição de aspas não fechadas e conteúdo inesperado após fechamento de aspas.\n- Validação explícita dos cabeçalhos dos quatro CSVs.\n- Rejeição de `NaN` e infinito em campos `double`.\n- Diagnósticos com arquivo e número da linha.\n- Novo executável de testes `europa-lite-validation-tests`, registrado no CTest.\n- Casos de regressão: campos citados, vírgulas, aspas escapadas, `NaN`, infinito, aspas malformadas, cabeçalho errado e desenvolvimento infinito.\n\n**Atenção:** essas alterações foram gravadas na branch de trabalho, mas ainda precisam ser compiladas e executadas pelo GitHub Actions. Não marcar como aprovadas até a CI terminar.\n\n## Validação executada

GitHub Actions: execução mais recente do primeiro ciclo geográfico passou nos dois sistemas:
https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite/actions/runs/42029385919

Passaram as etapas Configure, Build e Test em `ubuntu-latest` e `windows-latest`.

Testes cobrem:
- Data inicial e virada de ano.
- Carregamento de países, regiões e províncias.
- Busca de país, região e província.
- Quantidade de províncias por país.
- Desenvolvimento total de país.
- Adjacência simétrica e ausência de ligação não cadastrada.

## Limitações conhecidas

- Ainda não há mapa visual nem interface SDL.
- O parser CSV agora suporta campos entre aspas, vírgulas internas e aspas escapadas (`""`).
- Campos vazios finais precisam de tratamento específico.
- O grafo não diferencia fronteiras terrestres, estreitos, rios ou rotas marítimas.\n- A carga ainda não é transacional: se um arquivo falhar depois de outros terem sido lidos, o objeto World pode ficar parcialmente preenchido.
- Não há carregamento de save.
- A economia é provisória; não há produção, comércio, despesas ou manutenção.
- Não há diplomacia, guerra, rebeliões, tecnologia, eventos ou IA.
- O cenário contém somente dez províncias de demonstração, não um mapa histórico europeu.

## Próxima etapa recomendada

1. Melhorar o parser CSV, validar valores finitos e criar testes negativos para dados inválidos.
2. Adicionar tipo de fronteira e estrutura de áreas/sub-regiões.
3. Criar módulo de economia separado, com receitas, despesas e manutenção.
4. Expandir o cenário por regiões, mantendo os dados separados do código.
5. Planejar a interface gráfica somente depois que o núcleo de simulação estiver mais completo.

## Prompt de continuidade para outro assistente

Continue o projeto existente em https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite. Leia primeiro README.md, HANDOFF.md e docs/ARCHITECTURE.md. Não crie outro projeto nem substitua a arquitetura sem justificar.

Projeto C++17/CMake. A simulação deve permanecer independente da interface gráfica e ser eficiente em hardware modesto. Já existem calendário mensal, países, regiões, províncias, adjacências bidirecionais, carregamento CSV, economia mensal provisória, CLI, save textual, testes e CI Windows/Linux.

Antes de implementar:
- confira o branch main;
- confira o resultado mais recente do GitHub Actions;
- compile e execute CTest;
- corrija falhas reais antes de adicionar sistemas.

Próxima tarefa: confirmar CI da branch `hardening/data-loader-validation`; depois tornar o carregamento transacional e ampliar os testes negativos para IDs e referências. Só então adicionar tipos de fronteira e áreas/sub-regiões. Em seguida, separar a economia em um módulo próprio. Atualize HANDOFF.md a cada etapa. Não afirme que compilou ou testou sem executar os comandos e observar o resultado. Preserve as alterações existentes e faça commits pequenos e descritivos.
