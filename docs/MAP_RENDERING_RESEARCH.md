# Pesquisa complementar: renderização e ferramentas de mapa

**Data:** 2026-11-05  
**Objetivo:** extrair decisões úteis para o mapa do Europa Universalis Lite sem importar engines grandes ou misturar apresentação com simulação.

## 1. FIFE — Flexible Isometric Free Engine

Repositório/site: https://fifengine.net/

FIFE é uma engine C++ multiplataforma voltada a jogos isométricos. A documentação pública lista renderização 2D acelerada por GPU, atlases de imagem, renderizadores SDL/OpenGL, visão isométrica e top-down, ordenação automática por profundidade, mapas em camadas, múltiplas câmeras e carregamento tardio.

**O que aproveitar:**
- Separar câmera, camada do mapa e conteúdo visual.
- Ordenar elementos por profundidade de forma previsível.
- Manter visão top-down e isométrica como modos de apresentação do mesmo mundo.
- Carregar texturas e dados visuais sob demanda.
- Não transformar cada província em um objeto gráfico pesado; usar buffers e lotes.

**O que não fazer agora:** integrar FIFE diretamente. Isso adicionaria uma engine completa, seu ciclo de vida e suas dependências ao projeto antes de termos o modelo geográfico real. A licença LGPL também exige uma avaliação própria antes de qualquer incorporação de código. Por enquanto, FIFE é referência arquitetural.

## 2. EU Toolkit — ferramenta de mapa e edição de mods EU4

Repositório: https://github.com/lnxnb/eu-toolkit

É uma ferramenta desktop para visualizar e editar mods do EU4. O projeto documenta mapas políticos e temáticos, seleção de província, edição de dados, fila de alterações pendentes, undo/redo, validação e composição de mods sobre o jogo-base. Também preserva chaves de dados que a ferramenta ainda não modela, em vez de descartá-las.

A implementação usa Tauri 2, Rust e SvelteKit. O próprio projeto recomenda 8 GB de RAM para executar o mapa, então sua arquitetura de produto é mais útil como referência do que sua stack para nosso alvo de hardware.

**O que aproveitar:**
- Camadas temáticas separadas: política, terreno, cultura, religião, comércio e desenvolvimento.
- Fila de comandos/alterações para permitir desfazer e refazer.
- Validação antes de gravar dados.
- Preservar campos desconhecidos ao importar/exportar arquivos.
- Resolver sobreposição de mods como camadas, sem destruir a fonte original.

**Aplicação no nosso projeto:** o futuro editor de mapas pode usar uma fila de comandos reversíveis. O importador deve preservar metadados não reconhecidos quando isso for possível, e os modos de mapa devem ser camadas de renderização, não cópias do mundo.

## 3. OpenGL Terrain Demo — heightmap e relevo

Referência: https://github.com/MadEqua/opengl-terrain-demo

O exemplo mostra terreno gerado por heightmap e normal map, texturização por altitude/inclinação, neblina e técnicas de otimização como frustum culling. O projeto exige OpenGL 4.5 e usa tesselação adaptativa.

**O que aproveitar mais tarde:**
- Heightmap para elevação contínua.
- Normal map para iluminação sem aumentar demais a geometria.
- Mistura de materiais por altitude e inclinação.
- Neblina de distância para profundidade visual.

**O que adiar:** tesselação adaptativa, sombras avançadas e OpenGL 4.5. Não são requisitos para o primeiro mapa e podem excluir GPUs antigas.

## 4. Decisões consolidadas para o Europa Universalis Lite

1. O motor geográfico é a fonte de verdade. A câmera nunca altera coordenadas ou propriedade de províncias.
2. O mapa 2D interativo virá antes do terreno 2.5D real.
3. A seleção será baseada em IDs de província, não em nomes ou cores políticas.
4. O raster de IDs não pode usar antialiasing; cores intermediárias quebrariam a consulta.
5. Camadas temáticas serão calculadas a partir de dados do mundo e trocadas sem reconstruir a simulação.
6. A primeira etapa 2.5D usará malha simples derivada de heightmap, relevo moderado e câmera orbital limitada.
7. Renderizador e motor continuarão desacoplados. O primeiro protótipo SVG existe para validar layout e projeção sem dependências.
8. Dependências gráficas só entram depois de um protótipo funcional e medição de desempenho no hardware-alvo.

## 5. Próximos passos técnicos

- Fechar o importador de IDs RGB e dimensões do raster.
- Criar hierarquia geográfica e conexões tipadas.
- Gerar fronteiras e polígonos a partir do raster.
- Criar viewport 2D com pan/zoom, seleção e camadas.
- Adicionar altura por província/terreno e construir malha 2.5D.
- Medir memória, tempo de carga e FPS antes de aumentar detalhe.
