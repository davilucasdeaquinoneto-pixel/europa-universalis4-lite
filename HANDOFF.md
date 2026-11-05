# HANDOFF - Europa Universalis Lite

## Estado atual

- Repositório: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite
- Branch principal: `main`
- Branch desta etapa: `hardening/data-loader-validation`
- Versão de trabalho: `0.1.1-dev`
- Pull request: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite/pull/1
- CI desta etapa: aprovada em Windows e Ubuntu (Configure, Build e Test).

O projeto começou do zero porque o repositório original continha apenas um README. Esta é uma fundação técnica, ainda não um jogo completo.

## Objetivo e restrições

Criar um jogo independente de grande estratégia inspirado no gênero Europa Universalis, priorizando simulação profunda, IA estratégica e desempenho em hardware modesto. Alvo principal: Windows, C++17. O núcleo da simulação deve permanecer independente da interface gráfica.

Não copiar código, mapas, imagens, textos ou outros recursos proprietários de jogos comerciais.

## Arquivos principais

- `CMakeLists.txt`: build CMake, executável `europa-lite` e configuração CTest.
- `include/core/Date.hpp`: calendário mensal.
- `include/world/World.hpp`: países, regiões, províncias e adjacências.
- `include/simulation/Game.hpp`: API da partida.
- `src/World.cpp`: parser CSV e validação dos dados do cenário.
- `src/Game.cpp`: avanço mensal, economia provisória, consultas e save textual.
- `src/main.cpp`: interface de terminal.
- `data/countries.csv`: quatro países de demonstração.
- `data/regions.csv`: três regiões de demonstração.
- `data/provinces.csv`: dez províncias de demonstração.
- `data/adjacencies.csv`: grafo inicial de fronteiras.
- `tests/TestCore.cpp`: testes do calendário e modelo geográfico.
- `tests/TestWorldValidation.cpp`: testes de regressão do parser e entradas inválidas.
- `.github/workflows/ci.yml`: compilação e testes em Windows e Linux.
- `docs/ARCHITECTURE.md`: decisões arquiteturais.
- `README.md`: apresentação e instruções.
- `HANDOFF.md`: este documento.

## Sistemas já implementados

- Data inicial 11/11/1444 e avanço mensal.
- Países com tesouro, mão de obra e modificador tributário.
- Regiões geográficas.
- Províncias com proprietário, região, desenvolvimento e terreno.
- Adjacência bidirecional carregada de CSV.
- Economia temporária: desenvolvimento total × 0,10 × modificador tributário.
- Interface de terminal: `help`, `status`, `provinces`, `tick`, `save`, `quit`.
- Save textual inicial, ainda sem carregamento.
- Testes automatizados e CI para Windows/Linux.

## Etapa 1 em andamento: fortalecer o carregador de dados

Na branch `hardening/data-loader-validation`:

- Parser CSV suporta campos entre aspas, vírgulas internas e aspas escapadas (`""`).
- Rejeita aspas não fechadas e caracteres inesperados após o fechamento de aspas.
- Valida os cabeçalhos dos quatro arquivos CSV.
- Rejeita valores `NaN` e infinitos nos campos `double`.
- Mensagens de erro identificam arquivo e linha.
- Novo alvo CTest `europa-lite-validation-tests`.
- Testes cobrem campo citado com vírgula e aspas, NaN, infinito, aspas não fechadas, cabeçalho incorreto e desenvolvimento infinito.

### Validação observada

GitHub Actions run: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite/actions/runs/42033054808

- `build (windows-latest)`: Configure, Build e Test aprovados.
- `build (ubuntu-latest)`: Configure, Build e Test aprovados.

## Limitações conhecidas

- O cenário tem apenas dez províncias demonstrativas, não um mapa europeu histórico.
- Ainda não há mapa visual nem interface SDL.
- O parser é um leitor CSV de uma linha por registro; campos multilinha não são suportados.
- O carregamento de `World` ainda não é transacional: se um arquivo falhar após os anteriores terem sido lidos, o objeto pode ficar parcialmente preenchido.
- Ainda faltam testes adicionais para IDs duplicados, referências inexistentes e adjacências duplicadas.
- O grafo não diferencia fronteiras terrestres, estreitos, rios ou rotas marítimas.
- Não há carregamento de save.
- A economia é provisória; não há produção, comércio, despesas ou manutenção.
- Não há diplomacia, guerra, rebeliões, tecnologia, eventos, colonização ou IA.

## Próximos passos, nesta ordem

1. Integrar esta etapa após CI verde.
2. Tornar o carregamento de `World` transacional: ler e validar em estruturas temporárias e só publicar o novo mundo após sucesso total.
3. Acrescentar testes negativos para IDs duplicados, referências inexistentes, adjacências duplicadas e arquivos ausentes.
4. Separar o parser CSV em módulo reutilizável, se isso reduzir duplicação e facilitar testes.
5. Depois, adicionar tipos de fronteira e áreas/sub-regiões.
6. Só então iniciar a economia modular com receitas, despesas, produção e manutenção.

Não iniciar mapa gráfico, diplomacia ou guerra antes de estabilizar o motor e o carregamento dos dados.

## Como compilar e testar

Requer CMake 3.20+ e compilador C++17. Ninja é recomendado.

```bash
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/europa-lite
```

No Windows, executar `build/europa-lite.exe`. Rodar o programa a partir da raiz do repositório para localizar a pasta `data`.

## Prompt de continuidade para outro assistente

Continue o projeto existente: https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite

Leia primeiro `README.md`, `HANDOFF.md` e `docs/ARCHITECTURE.md`. Não crie outro repositório nem substitua a arquitetura sem justificativa técnica. O projeto é C++17/CMake e deve priorizar simulação profunda com baixo consumo de memória e CPU. O núcleo deve continuar independente da interface gráfica.

Estado atual: calendário mensal, países, regiões, províncias, adjacências, economia provisória, CLI e save textual inicial. O carregador CSV foi fortalecido para campos citados, vírgulas internas, aspas escapadas, cabeçalhos validados e rejeição de NaN/infinito. Foram adicionados testes de regressão. A CI da etapa passou em Windows e Ubuntu.

Próxima tarefa obrigatória: tornar `World::load` transacional. Carregue países, regiões, províncias e adjacências em estruturas temporárias; em qualquer erro, preserve integralmente o mundo anterior; só substitua o estado atual após todas as validações passarem. Adicione testes demonstrando que uma carga inválida não destrói um mundo já carregado.

Depois acrescente testes para IDs duplicados, referências inexistentes, adjacências duplicadas e arquivos ausentes. Mantenha mensagens de erro com arquivo e linha. Atualize README, arquitetura e HANDOFF. Execute compilação e CTest e aguarde CI Windows/Linux. Não declare testes aprovados sem observar os resultados reais. Faça mudanças pequenas e commits descritivos. Não avance para economia, mapa ou diplomacia enquanto esta etapa de robustez não estiver concluída.
