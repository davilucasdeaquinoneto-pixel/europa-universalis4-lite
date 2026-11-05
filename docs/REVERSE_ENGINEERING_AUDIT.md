# Auditoria técnica de subsistemas e engenharia reversa

**Data da auditoria:** 2026-11-05  
**Projeto-alvo:** Europa Universalis Lite (C++17)  
**Branch de trabalho:** `research/subsystem-audit-and-world-hardening`

## 1. Objetivo e limite desta auditoria

O objetivo é transformar os projetos de referência em decisões técnicas para o nosso próprio motor: quais dados extrair, quais arquiteturas adotar, quais algoritmos reimplementar e quais componentes não fazem sentido integrar.

O arquivo completo `catalogue.json` foi obtido pelo objeto Git (12,4 MB) e os 21.332 registros foram processados programaticamente: contagem por tipo, estado de confiança, famílias, escopos, fontes de tipagem e buscas temáticas. Também foram examinados CURATED/VERIFIED/CATALOGUE/DEFINES_STATUS/BACKLOG, os oito guias de `task_map.json` e arquivos centrais do parser, extrator de assets e renderizador. Isso cobre o inventário e a análise estrutural de todos os registros, mas não equivale a uma interpretação manual linha por linha de cada comportamento ou função GUI.

## 2. Fontes examinadas

- [EU5 Engine Atlas](https://github.com/mattebin/eu5-engine-atlas)
- [EU5 Workbench](https://github.com/mattebin/eu5-workbench)
- [PDX Tools](https://github.com/pdx-tools/pdx-tools)
- [eu4-parser](https://github.com/matthewbauer/eu4-parser)
- [Jomini](https://github.com/rakaly/jomini)
- Nosso [repositório](https://github.com/davilucasdeaquinoneto-pixel/europa-universalis4-lite)

## 3. EU5 Engine Atlas: o que são os 21.328 registros

O Atlas não é um motor de jogo pronto. É um inventário de funcionalidades registradas no executável do EU5, complementado por documentação, extração estática e testes dentro do jogo. Os números efetivamente presentes no `catalogue.json` processado são:

| Registro | Quantidade | O que representa |
|---|---:|---|
| Effects | 555 | Operações que scripts podem executar |
| Triggers | 1.272 | Condições consultáveis por scripts |
| Scope links | 283 | Transições entre contextos de país, localização, guerra etc. |
| List-type bases | 485 | Tipos/famílias de listas expostos ao sistema de scripts |
| GUI functions | 15.762 | Funções de dados acessíveis à interface |
| Defines | 2.975 | Parâmetros configuráveis registrados pelo motor |
| **Total** | **21.332** | Registros catalogados |

A documentação explica que as 15.762 funções GUI combinam 10.828 extraídas do binário e outras 4.934 obtidas do dump de tipos do próprio jogo. Existem ainda mais de 19 mil associações tipadas. Portanto, não se deve somar essas associações ao total de registros.

**Divergência encontrada:** o texto explicativo de `CATALOGUE.md` ainda informa 21.328 itens, 553 effects e 1.270 triggers, mas o JSON publicado contém `meta.items_total = 21.332` e as contagens por estado somam 555 effects e 1.272 triggers. Para esta auditoria, os dados efetivamente processados do JSON prevalecem sobre os números desatualizados da prosa.

### 3.0 Resultado do processamento integral

| Tipo | Total | Vanilla | Confirmados | Reais, não totalmente provados | Aceitos | Registrados | Inferidos | Mortos |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Effects | 555 | 516 | 9 | 30 | 0 | 0 | 0 | 0 |
| Triggers | 1.272 | 1.133 | 10 | 129 | 0 | 0 | 0 | 0 |
| Scope links | 283 | 231 | 6 | 0 | 45 | 1 | 0 | 0 |
| List types | 485 | 253 | 4 | 0 | 4 | 3 | 221 | 0 |
| GUI functions | 15.762 | 6.523 | 578 | 0 | 0 | 8.661 | 0 | 0 |
| Defines | 2.975 | 2.888 | 0 | 0 | 22 | 64 | 0 | 1 |
| **Total** | **21.332** | **11.544** | **607** | **159** | **71** | **8.729** | **221** | **1** |

O conjunto contém 12.310 itens em estados `vanilla`, `confirmed_working` ou `verified_real`; os demais ainda exigem cautela, teste ou confirmação. Isso não significa que os 12.310 sejam código portável: muitos são nomes de funções de script ou funções GUI específicas do EU5.

O campo `undocumented` marca itens ausentes dos arquivos vanilla, não funcionalidades secretas automaticamente úteis. O total por tipo foi: 34 effects, 135 triggers, 52 scope links, 232 list types, 4.305 GUI functions e 87 defines.

As funções GUI têm 27.274 associações de tipo registradas: 24.912 vindas do dump de tipos, 2.181 inferidas por adjacência com precisão indicada de aproximadamente 80% e 181 observadas em arquivos GUI vanilla. Essas associações não são funções adicionais.

### 3.0.1 Busca temática sobre o inventário

As buscas abaixo são filtros por nomes/identificadores, não categorias oficiais exclusivas. Um item pode aparecer em mais de uma busca.

| Tema | Itens effect/trigger/scope/list encontrados | Em estado de alta confiança* |
|---|---:|---:|
| Economia | 247 | 228 |
| População e sociedade | 172 | 152 |
| Geografia e controle | 310 | 280 |
| Diplomacia | 147 | 133 |
| Militar | 180 | 161 |
| IA | 53 | 49 |
| Variáveis, eventos e lógica de script | 118 | 114 |

*Alta confiança aqui significa `vanilla`, `verified_real` ou `confirmed_working`. A classificação por palavra-chave pode produzir falsos positivos e não substitui a leitura do item.

Os blocos de defines mais numerosos incluem `NAI` (751), `NDiplomacy` (338), `NMapColors` (231), `NCountry` (120), `NEconomy` (104), `NGameIcons` (101), `NGraphics` (93) e `NCombat` (79). Isso mostra a extensão da superfície de configuração, mas não prova que cada define esteja ativo.

Os escopos mais frequentes entre atributos indexados incluem country (389 ocorrências), location (162), character (75), religion (42), unit (40), province (39), market (19), pop (16), culture (15) e area (11). As contagens representam ocorrências de atributos, não quantidade de entidades no jogo.

### 3.1 Confiança das descobertas

O Atlas separa existência de comportamento. Seus níveis publicados são:

| Estado | Usabilidade | Interpretação |
|---|---:|---|
| confirmed_working | 100 | Comportamento demonstrado com leitura de volta, sentinel ou dado vivo |
| vanilla | 90 | Usado pelos arquivos oficiais do jogo |
| verified_real | 80 | O executável reconhece o item, mas a semântica completa pode não estar provada |
| accepted | 60 | Aceito em teste, sem prova de efeito útil |
| registered | 25 | Encontrado no registro, sem teste comportamental |
| inferred | 20 | Inferência de baixa confiança |
| dead | 0 | Testado e comprovadamente inerte |

A confiança de atributos individuais também é separada: uma função pode existir e funcionar, mas o tipo inferido para seu argumento pode continuar incerto. Nosso projeto deve adotar essa disciplina para qualquer dado obtido por engenharia reversa: registrar fonte, versão, método, confiança e teste de confirmação.

### 3.2 Subsistemas representados no Atlas

Os registros cobrem superfícies de script relacionadas a:

- Economia: produção de alimentos, lucro de construções, custos de contratação, capacidade de empréstimo, pagamentos e recursos anuais.
- População e sociedade: população por área, tipo de população, cultura, idioma, religião e emprego.
- Geografia e controle: distância até áreas, capacidade portuária, controle médio, prosperidade e quantidade de localidades.
- Diplomacia: opinião, confiança, antagonismo, relações, anexação, capacidade diplomática e tipos de súdito.
- Militar: força de unidades, exércitos e marinhas, cercos, brechas, pontuação de guerra e levantes.
- IA: preferência de conquista, utilidade de alianças, mercados, organizações e ações genéricas.
- Eventos e lógica: efeitos, gatilhos, variáveis, listas, mapas e execução condicional.
- Interface: funções de dados, contextos, tooltips, seleção e atualização de apresentação.
- Configuração: defines de calendário, economia, combate, movimentação e IA.

Esses registros mostram quais conceitos e consultas existem no EU5, mas não fornecem por si só os algoritmos internos de economia, população, combate ou IA. Para nosso motor, servem como mapa de requisitos e vocabulário de sistemas, não como implementação pronta.

### 3.3 O que os oito guias de tarefas acrescentam

O `task_map.json` organiza referências em oito tarefas práticas: criar modos de mapa, ajustar defines, alterar comportamento da IA, criar eventos e on_actions, trabalhar com GUI, configurar modificadores, conteúdo militar e depuração. A ideia de guias orientados a tarefas é útil para a documentação do nosso projeto: cada funcionalidade deverá listar módulos envolvidos, dados necessários, testes e riscos.

O Atlas também registra armadilhas importantes de engenharia reversa: erros silenciosos, arquivos inteiros abortados por erro de sintaxe, logs que rodam ou são truncados e diferenças entre um símbolo registrado e um comportamento realmente ativo. Devemos usar testes positivos, controles negativos e leitura de resultados, nunca concluir que algo funciona apenas porque não apareceu erro.

### 3.4 O que não importar do Atlas

- Não importar as 21.328 entradas como se fossem código de simulação.
- Não tratar nomes de funções do EU5 como APIs do nosso C++.
- Não copiar defines sem descobrir quais regras realmente os consomem.
- Não presumir que o EU5 e o EU4 compartilham as mesmas regras ou estruturas.
- Não incorporar ferramentas de extração de executável ao jogo final; elas pertencem a ferramentas de pesquisa/modding.

O Atlas é especialmente útil como checklist de cobertura funcional e como exemplo de processo de verificação. Seu snapshot é associado à versão EU5 1.3.11 e precisa ser tratado como versionado, não como verdade eterna.

## 4. PDX Tools: dados, mapas e análise de saves

PDX Tools é um analisador de saves e visualizador, não um motor de simulação. Sua arquitetura separa extração de assets, leitura de estado, renderização e interface.

### 4.1 Pipeline geográfico do EU4

A implementação de assets mostra uma cadeia de dados que devemos reproduzir em um importador próprio:

1. `definition.csv` associa IDs de província a cores RGB.
2. `provinces.bmp` informa qual província ocupa cada pixel.
3. `default.map` fornece informações como mares iniciais e lagos.
4. `terrain.bmp` e `terrain.txt` permitem obter o terreno visual e as substituições explícitas.
5. `climate.txt` marca províncias impassáveis.
6. Arquivos de áreas, regiões, super-regiões, continentes e posições completam a hierarquia geográfica.
7. Rios, costas, estreitos e adjacências complementam a topologia.

A rasterização por cor é uma boa base para identificar províncias sem depender de polígonos complexos em cada consulta. A tabela RGB deve ser convertida uma vez para uma tabela de ID, e a imagem deve permanecer sem suavização de pixels para não criar cores intermediárias inexistentes.

### 4.2 Cálculo de terreno

A documentação técnica de PDX Tools descreve um algoritmo para províncias sem substituição explícita: contar os tipos de terreno presentes nos pixels da província, combinar índices gráficos que representam o mesmo terreno, aplicar a camada de árvores, ignorar oceano e oceano interior e escolher o terreno de maior peso. Florestas, selvas e bosques recebem peso maior. Empates seguem uma regra estável. As substituições explícitas mudam de precedência conforme a versão do EU4.

Conclusão: não basta ler um único campo de terreno. Nosso importador deve distinguir terreno explícito, terreno inferido e terreno desconhecido, guardar a proveniência e a versão das regras.

### 4.3 Arquitetura do renderizador

A biblioteca `pdx-map` separa:

- Mundo/topologia e índices espaciais.
- Arrays compactos de localidades e cores.
- Viewport, zoom e pan.
- Camadas de renderização.
- Renderizador GPU.
- Controlador de interação.
- Leitura de cor/ID para descobrir a localidade sob o cursor.

O renderizador usa WebGPU/wgpu e separa texturas em hemisférios para respeitar limites de dimensão de textura em GPUs comuns. O módulo também suporta bordas de província e país, cores primária/secundária, destaque, wrap horizontal e captura de imagem.

O padrão mais valioso para nós é a separação entre dados do mundo e apresentação. O núcleo C++ não deve conhecer SDL, OpenGL ou qualquer janela. O mapa recebe um snapshot compacto de cores e fronteiras, renderiza e devolve eventos de seleção por ID.

### 4.4 Identificação de províncias pelo cursor

O PDX Tools cria uma imagem de IDs codificados em cores, desativa suavização e encontra a cor do pixel sob o cursor. A cor é procurada em uma lista ordenada e mapeada para o ID da província. Isso permite seleção rápida sem testar ponto-em-polígono para cada clique.

Para nosso hardware, a primeira versão pode usar uma imagem raster de IDs e um lookup RGB->ProvinceId. A renderização pode começar em 2D simples e resolução limitada; não precisamos de um renderizador 3D com GPU complexa para validar o mapa.

### 4.5 O que aproveitar

- Pipeline de importação de mapa baseado em bitmap + tabela de cores.
- Separação entre topologia, viewport, interação e renderização.
- Picking por ID de pixel.
- Camadas de mapa desacopladas do motor.
- Índices espaciais e arrays compactos.
- Testes de terreno e de conversão de pixels.

Não faz sentido portar diretamente a biblioteca Rust/wgpu para dentro do nosso executável C++ nesta fase. Isso adicionaria uma segunda stack, toolchain e dependências gráficas grandes antes de termos um modelo de mundo estável. Vamos reimplementar a arquitetura necessária em C++ e manter a possibilidade de trocar o backend gráfico.

## 5. eu4-parser: reconstrução de geografia

O `eu4-parser` mostra como derivar dados geográficos de arquivos do jogo:

- Abrir o bitmap de províncias e a tabela de definições.
- Encontrar as bordas de cada cor/província.
- Converter bordas em polígonos e GeoJSON.
- Ler áreas, clima, países, posições, províncias, regiões, super-regiões, continentes e ventos.
- Projetar as formas em um mapa navegável.

O projeto original é escrito em Haskell, mas o algoritmo de extração é independente da linguagem. Vamos implementar um importador C++ que gere um pacote de dados intermediário versionado. O jogo não deve precisar analisar os arquivos-fonte do EU4 a cada inicialização.

Para a primeira versão visual, o raster original de IDs é mais simples e rápido. Polígonos são úteis depois para fronteiras vetoriais, hit-testing alternativo, exportação e ferramentas de edição.

## 6. Jomini: formatos Clausewitz e saves

Jomini é uma biblioteca Rust para parsing de sintaxe textual e binária usada por jogos Paradox. A documentação mostra:

- Leitura de texto com codificações como Windows-1252.
- Tokenização em uma representação intermediária (tape) para percorrer campos sem materializar toda a árvore.
- Desserialização para estruturas tipadas.
- Parsing binário com resolução de tokens de 16 bits e regras de codificação específicas por jogo.
- Escrita e normalização de texto.
- Tratamento de envelopes de saves de jogos mais novos.

Uma distinção essencial: EU4 e HOI4 não usam o mesmo envelope de save dos jogos modernos baseados no formato Jomini. Mesmo que partes da sintaxe e do parsing binário sejam reutilizáveis, o cabeçalho, compressão, token map e detalhes de versão precisam ser específicos.

Decisão: manter o núcleo C++17. Vamos usar Jomini como referência de arquitetura, corpus de casos e benchmark conceitual. Se futuramente um parser Rust externo for considerado, ele ficará atrás de uma interface isolada e só será adotado se o custo de integração compensar. Não será uma dependência obrigatória do motor.

## 7. EU5 Workbench

O Workbench combina editor, árvore de arquivos, catálogo de funções, autocomplete, guias por tarefa e lint em tempo real. Não é o motor de jogo nem um renderizador.

A lição aproveitável é construir ferramentas de desenvolvimento próprias para os dados do nosso jogo: validador de cenários, diagnóstico por arquivo/linha, editor de definições e documentação pesquisável. Isso pode vir depois que o formato de dados estabilizar.

## 8. Auditoria dos subsistemas do nosso motor

| Subsistema | Estado atual | Referência útil | Decisão |
|---|---|---|---|
| Calendário/scheduler | Data inicial e avanço mensal | Atlas: defines e pulsos; Jomini: datas | Criar fases mensais determinísticas e calendário testável |
| Países | Tag, nome, tesouro, manpower e imposto | Atlas: país, relações, utilidades | Separar identidade, governo, recursos e estado político |
| Províncias | ID, nome, dono, região, desenvolvimento e terreno | eu4-parser + pdx-assets | Expandir para localização, clima, cultura, religião, população e edifícios |
| Hierarquia geográfica | Apenas região | eu4-parser | Adicionar área, sub-região, região e continente |
| Grafo | Adjacência bidirecional sem tipo | pdx-assets / pdx-map | Tipar fronteira terrestre, mar, estreito, rio e bloqueio |
| Geometria | Ausente | eu4-parser / pdx-map | Importar raster de IDs primeiro; polígonos depois |
| Renderização | Ausente | pdx-map | Camada independente, 2D e econômica em recursos |
| Economia | Fórmula temporária de imposto | Atlas + PDX Tools | Separar produção, impostos, custos, estoques e mercados |
| População | Ausente | Atlas: queries de população; EU5 | População agregada por província e grupos, sem simular cada pessoa |
| Comércio | Ausente | Atlas: mercados/rotas; PDX Tools | Grafo de mercados e fluxo de bens em atualização periódica |
| Diplomacia | Ausente | Atlas: opinião, confiança, relações, sujeitos | Relações bilaterais e tratados com estado explícito |
| Tecnologia/instituições | Ausente | Atlas: defines e consultas; dados EU4 | Progresso, desbloqueios e difusão como sistemas próprios |
| Exércitos/marinha | Ausente | Atlas: unidades, força e levies | Entidades compactas, composição e localização |
| Guerra/combate/cercos | Ausente | Atlas: força, cercos, war score | Simulação por fases e resultados determinísticos |
| Eventos/scripts | Ausente | Atlas: effects, triggers, scopes | Começar com eventos declarativos limitados, não clonar Clausewitz inteiro |
| IA | Ausente | Atlas: generic actions e utilidades | Objetivos por país, avaliação de ações e orçamento por tick |
| Saves | Escrita textual inicial, sem load | Jomini / PDX Tools | Formato próprio versionado, load transacional e RNG persistente |
| Ferramentas de dados | CSV básico | EU4 parser / PDX assets / Workbench | Importador, validador e diagnóstico de dados |
| Testes/performance | CTest e CI Windows/Linux | Jomini benchmarks / Atlas probes | Testes determinísticos, benchmarks e cenários grandes |

## 9. Arquitetura-alvo proposta

O motor deve ser dividido em camadas:

1. **Core:** data, IDs tipados, RNG com seed, eventos, calendário, serialização e logs.
2. **World:** países, províncias, áreas, regiões, continentes, geografia e grafo de conexões.
3. **Simulation:** economia, população, produção, comércio, diplomacia, tecnologia, guerra e eventos.
4. **AI:** objetivos, avaliação de ameaças/oportunidades e seleção de ações com limite de CPU.
5. **Presentation:** CLI de depuração e renderizador 2D independente.
6. **Tools/Data:** importador de assets, validador, conversor de formatos e gerador de cenários.

### Regras de desempenho

- IDs inteiros estáveis e arrays contíguos para dados acessados frequentemente.
- Índices auxiliares para buscas por ID/tag.
- Grafo de adjacência em estrutura compacta (CSR ou equivalente) quando o mapa crescer.
- Atualizações por sistema e por frequência; não recalcular tudo em todo frame.
- População agregada em grupos, não um objeto C++ por habitante.
- Pathfinding sob demanda com cache invalidado quando a topologia mudar.
- Renderizador recebe snapshots; não participa do tick econômico.
- Cenários de teste com milhares de províncias desde cedo.

O alvo de hardware é modesto (Celeron N4020, 4 GB de RAM e GPU integrada). A prioridade é reduzir alocação, cópias e atualizações desnecessárias. Uma interface 2D simples é preferível a introduzir uma stack 3D pesada agora.

## 10. Plano de integração em ordem

### Fase A — integridade do núcleo
- [x] Parser CSV com campos citados e aspas escapadas.
- [x] Validação de cabeçalhos e números finitos.
- [x] Carregamento transacional de mundo (nesta branch).
- [x] BOM UTF-8 nos cabeçalhos CSV (nesta branch).
- [x] Índice local de IDs para montar adjacências em O(P + E), em vez de varrer todas as províncias para cada conexão (nesta branch).
- [ ] Rodar CI da branch e revisar os resultados.
- [ ] Diagnóstico agregado e testes de referências/IDs duplicados.

### Fase B — modelo geográfico
- [ ] IDs tipados e índices por ID/tag.
- [ ] Áreas, sub-regiões, regiões e continentes.
- [ ] Tipos de conexão, portos, costas, rios, estreitos e impassáveis.
- [ ] Importador de raster RGB->ID e metadados.
- [ ] Construção de fronteiras e polígonos.
- [ ] Pathfinding e distâncias.

### Fase C — persistência e determinismo
- [ ] Save/load versionado.
- [ ] RNG com seed persistente.
- [ ] Replay de comandos e hash de estado.
- [ ] Testes de determinismo entre execuções.

### Fase D — simulação
- [ ] Economia modular.
- [ ] População e mão de obra.
- [ ] Produção, bens e mercados.
- [ ] Diplomacia e tecnologia.
- [ ] Exércitos, guerra e cercos.
- [ ] IA estratégica limitada por orçamento.

### Fase E — apresentação
- [ ] Mapa 2D com pan/zoom.
- [ ] Picking de província por ID.
- [ ] Camadas políticas, terreno, religião, cultura, economia e guerra.
- [ ] Painéis e ferramentas de depuração.

## 11. Alterações de código desta branch

Além desta auditoria, a branch faz três correções concretas no carregador:

1. **Carga transacional:** `World::load` carrega em um objeto candidato e só troca os vetores do mundo atual quando todos os arquivos e referências são aceitos. Uma carga inválida não destrói o cenário que já estava na memória.
2. **Montagem de adjacências:** cria um índice temporário `province ID -> posição no vetor`. Cada aresta atualiza diretamente as duas províncias, eliminando a varredura completa do vetor para cada conexão.
3. **BOM UTF-8:** remove o BOM apenas no cabeçalho de cada CSV, evitando que o primeiro nome de coluna fique contaminado.

Foram adicionados testes para BOM e preservação do mundo após falha de carga. A CI desta branch ainda precisa executar antes de declarar essas alterações aprovadas.

## 12. Conclusão técnica

O material mais valioso não é um único motor pronto para encaixar. É a combinação de três ideias:

- **EU4 parser + PDX assets:** como transformar arquivos de mapa em um mundo de províncias real e consultável.
- **PDX map:** como separar renderização, topologia, viewport e seleção de localidades.
- **Jomini + Engine Atlas:** como tratar formatos de dados, parsing, introspecção e evidência de comportamento sem confundir registro com funcionalidade real.

Nosso próximo grande marco deve ser um **importador geográfico que gere um mundo completo e um grafo eficiente**, antes de iniciar a economia profunda. Sem essa base, diplomacia, movimentação, comércio e IA vão acabar dependendo de um mapa fictício e de estruturas que precisarão ser reescritas.
