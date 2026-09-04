# Water Pipe — Project Handoff / Contexto Completo

> Documento de continuidade do projeto para abrir outro chat sem perder as decisões já tomadas.
> Data: 2026-09-04

## 1. Resumo

Water Pipe é um jogo de puzzle hidráulico para o CrowPanel Advance 7", baseado em ESP32-S3.

O jogador constrói uma rede de canos em uma grade enquanto a água começa a circular. Ele precisa raciocinar sobre conexões, volume, perdas, pressão, altura, temperatura e recursos para cumprir o objetivo específico de cada fase.

Loop principal:

**Pensar → Construir → Água flui → Observar → Reagir → Corrigir**

A inspiração de inventário é parcialmente Tetris: o jogador recebe uma sequência de peças, vê as próximas e pode usar HOLD.

## 2. Hardware alvo

CrowPanel Advance 7":

- ESP32-S3-WROOM-1
- Dual core, 240 MHz
- Display 800×480 RGB565
- 8 MB PSRAM
- Heap interno livre em runtime ~129 KB
- Touch GT911, single touch
- Sem aceleração 2D
- Desenho via GfxCompat
- Framebuffer lógico já disponível
- LVGL não roda durante o modo Jogos

Consequência: formas geométricas, alto contraste, áreas grandes, poucos elementos por frame, desenho incremental e nenhuma dependência de GPU.

## 3. Layout aprovado

A direção visual atualmente aprovada usa:

- 800×480
- 10 colunas × 7 linhas
- células de aproximadamente 64×64 px
- tabuleiro ocupando a maior parte da tela
- HUD compacto à direita
- fundo externo escuro
- tabuleiro claro
- tubos escuros/metálicos com água claramente visível

HUD:

- Restart e Exit no topo
- dinheiro simples, como $ 1900
- Cu/PVC
- peça atual / NEXT PIPE
- duas próximas peças
- HOLD
- indicador simples de pressão verde → amarelo → vermelho

Não voltar para uma HUD cheia de números ou excessivamente trabalhada. O tabuleiro é o protagonista.

## 4. Controle por toque

- Tap em célula vazia: coloca o cano atual.
- Tap curto em cano existente: gira 90° horário.
- Pressionar e segurar cerca de 450 ms em cano existente: remove.
- O hold de remoção não deve também disparar rotação.
- Peças aparecem imediatamente; não existe animação de arrastar.
- Não existe Undo.

Remoção:

- só peça seca pode ser removida;
- peça com água não pode ser removida;
- peça quebrada não pode ser reparada;
- dinheiro gasto não é devolvido;
- para reformar uma instalação viva, pode-se usar válvula e esperar uma seção esvaziar.

## 5. Inventário

### NEXT PIPE

Modo principal:

- sequência finita;
- peça atual;
- duas próximas peças;
- um slot HOLD.

O jogador toca no quadrado para colocar a peça.

### HOLD

Lógica inspirada em Tetris:

- guarda a peça atual;
- troca pela peça guardada;
- depois de usar HOLD, precisa colocar uma peça antes de usar HOLD novamente.

### FREE PICK

Algumas fases podem permitir escolher qualquer peça do repositório permitido, com quantidade e dinheiro limitados.

## 6. Peças

Peças principais:

- Straight
- Curve
- T
- Cross
- Cap
- Splitter 2-way
- Splitter 3-way
- Mixer
- Valve
- Pump 1
- Pump 2
- Pump 3
- Pump 4
- Reservoir

Materiais:

- PVC
- Copper / Cu

Possíveis peças futuras, não necessárias inicialmente:

- relief valve
- one-way pipe
- reducer
- heater
- cooler
- leak pipe
- steel

## 7. T, Cross e Splitters

T é um T hidráulico normal e passivo. A água pode entrar e sair conforme a dinâmica.

Quando existem caminhos equivalentes:

- 2 saídas → 50% / 50%
- 3 saídas → aproximadamente 33% / 33% / 33%

Splitter 2-way e Splitter 3-way são peças explicitamente destinadas a dividir fluxo.

A física pode ser simplificada; não é necessário simular hidráulica científica.

## 8. Cap

Cap representa uma tampa real:

- fecha uma ponta;
- impede água de escapar;
- permite conter água;
- ajuda em reformas e controle de fluxo.

## 9. Água

Água é um recurso real do puzzle.

Possui:

- volume;
- temperatura;
- fluxo;
- pressão associada ao sistema.

Deve ser visível dentro dos tubos.

Visual desejado:

- tubo vazio;
- tubo enchendo;
- tubo cheio.

O jogador deve entender o estado sem depender de números.

## 10. Capacidade

Valores iniciais:

- tubo normal: 10 unidades
- reservatório: 100 unidades

A água preenche progressivamente.

Quando um segmento está cheio, a água adicional segue para caminhos disponíveis. Se não houver para onde ir, ocorre pressão/overflow conforme a situação.

## 11. Fluxo e conectividade

Uma conexão é válida apenas quando existe reciprocidade.

Exemplo: um cano aberto para a direita só conecta com vizinho aberto para a esquerda.

Máscaras:

- UP = 1
- RIGHT = 2
- DOWN = 4
- LEFT = 8

O tabuleiro é pequeno, então BFS é suficiente.

Com 10×7 existem no máximo 70 células.

## 12. Encontro de águas

Correntes podem chegar ao mesmo sistema.

Para temperatura, águas de temperaturas diferentes permanecem separadas até entrarem em um Mixer.

## 13. Temperatura

Temperatura é propriedade da água e entra em fases posteriores.

Exemplo de fontes:

- 90 °C
- 10 °C

A água mantém sua temperatura inicialmente e não há perda de calor no começo.

Mixer:

Tmix = soma(volume × temperatura) / volume total

Exemplo:

10 L a 90 °C + 10 L a 10 °C = 20 L a 50 °C.

Outro exemplo:

10 L a 90 °C + 2 L a 10 °C continua próximo de 90 °C.

Objetivos podem exigir uma faixa, por exemplo 40–60 °C.

## 14. Pressão

Pressão é uma mecânica central, mas será uma física fake convincente.

Aumenta quando água tenta avançar e não consegue:

- válvula fechada;
- cano cheio recebendo água;
- caminho bloqueado;
- subida sem bomba;
- restrição da fase.

Diminui quando a restrição é removida.

Deve mudar gradualmente para dar tempo de reação.

Estados conceituais:

- normal;
- médio;
- alto;
- crítico;
- ruptura.

## 15. Ruptura

Quando pressão ultrapassa o limite:

1. cano quebra;
2. cria abertura;
3. água escapa;
4. perda é contabilizada;
5. não há reparo;
6. o cano precisa ser removido depois de seco e substituído.

Valores iniciais:

| Material | Custo | Limite |
|---|---:|---:|
| PVC | 10 | 700 |
| Copper | 25 | 1000 |

Valores são de balanceamento e podem mudar após testes.

## 16. Válvula

Válvula é uma peça colocável.

Aberta: água passa.

Fechada: água fica retida e pressão pode aumentar.

Usos:

- controlar fluxo;
- isolar setores;
- reformar instalações;
- criar puzzles de pressão;
- segurar água temporariamente.

## 17. Reservatório

Peça com capacidade muito maior que tubo normal.

Pode:

- receber;
- armazenar;
- liberar;
- transbordar.

Overflow conta como perda.

Pode ser objetivo de fase.

## 18. Gravidade e vistas

Existem dois tipos de fase.

### Vista superior

Instalação plana. Altura é ignorada.

### Vista lateral

Existe elevação.

A água:

- desce naturalmente;
- busca níveis inferiores;
- não sobe indefinidamente;
- precisa de pressão/bomba para subir.

A regra deve ser ensinada visualmente, sem depender de texto.

Exemplo de identidade de fase:

LEVEL — PIPELINE: vista superior.

LEVEL — WATER TOWER: vista lateral.

## 19. Bombas

Bombas são peças colocáveis.

Força:

- Pump 1 → +1 nível
- Pump 2 → +2
- Pump 3 → +3
- Pump 4 → +4

Aproximadamente 3–4 bombas diferentes.

Visualmente, bomba forte deve parecer mais complexa, permitindo reconhecer força sem ler texto.

Bombas podem ser encadeadas.

## 20. Objetivos

Não existe uma condição universal.

Cada fase pode ter objetivo próprio:

- conectar A → B;
- conter toda a água;
- entregar determinado volume;
- encher reservatório;
- manter perda abaixo do limite;
- manter pressão abaixo do limite;
- levar água a determinada altura;
- produzir temperatura-alvo;
- combinar vários requisitos.

Exemplo importante:

Em fase de contenção, qualquer perda pode significar derrota.

Em fase de água morna, perder água quente demais pode tornar o objetivo impossível.

## 21. Timing

Algumas fases começam imediatamente.

Outras possuem atraso.

Exemplo:

- fase inicia;
- fonte espera;
- depois de 10 segundos começa a água.

Deve existir indicação visual/sonora.

O jogador continua construindo enquanto a água corre.

Não existe botão de acelerar.

## 22. Erros e recuperação

Não existe Undo.

O jogador pode colocar uma peça errada e precisar:

- esperar secar;
- usar válvula;
- remover;
- reconstruir;
- perder dinheiro.

O erro deve ser parte da experiência.

## 23. Economia

Cada fase possui orçamento próprio.

Dinheiro é gasto ao colocar peças.

Remover não devolve dinheiro.

Decisões:

- PVC barato ou Copper caro?
- solução com mais peças ou solução eficiente?
- usar material forte ou economizar?

HUD mostra somente algo simples como $ 1900.

## 24. Estrelas

Até 3 estrelas por fase.

Base:

- 1 estrela: completar;
- 2 estrelas: boa eficiência de água;
- 3 estrelas: boa eficiência geral.

Métricas possíveis:

- água perdida;
- dinheiro gasto;
- peças usadas;
- tempo;
- eventos de pressão.

Estrelas não bloqueiam progressão.

## 25. Campanha

Estrutura:

World / Chapter → Phases

Sem Easy / Normal / Hard separados.

Progressão proposta:

### World 1 — Basic Water
Grade, straight, curve, T, cross, source, target, fluxo, perda, cap.

### World 2 — Control & Pressure
Valve, pressão, PVC, Copper, ruptura.

### World 3 — Storage & Distribution
Reservoir, splitters, múltiplos caminhos, volume.

### World 4 — Height
Vista lateral, gravidade, bombas.

### World 5 — Temperature
Água quente/fria, Mixer, temperatura.

Depois: combinações.

## 26. Exemplos de fases iniciais

São conceitos, não lista definitiva.

1. Connect
2. Don't Spill
3. Junction
4. First Valve
5. Pressure
6. Storage
7. Split
8. Height
9. More Height
10. Hot + Cold

## 27. Menu principal

Menu próprio do Water Pipe:

- CONTINUE
- PHASES
- SCORES
- EXIT

### CONTINUE

Retoma a fase incompleta mais recente.

Restaura:

- tabuleiro;
- peças;
- água;
- fila;
- HOLD;
- dinheiro;
- simulação;
- tempo.

### PHASES

Mostra seleção de fases:

- disponíveis;
- estrelas;
- bloqueadas.

Fases concluídas continuam rejogáveis.

### SCORES

Top 5 global.

### EXIT

Volta ao menu Games do host.

## 28. Durante a fase

Controles persistentes:

- RESTART
- EXIT

RESTART:

- reinicia a fase;
- não apaga estrelas já conquistadas.

EXIT:

- salva;
- volta ao menu.

## 29. Save

Não existe botão Save.

Salvar automaticamente:

- ao sair;
- ao terminar fase;
- periodicamente.

Persistência deve sobreviver a reinicialização/desligamento.

Intenção: NVS / Preferences no ESP32.

Save deve possuir versão.

Ao completar fase:

- salvar progresso;
- limpar resume;
- registrar estrelas;
- registrar score.

## 30. Pontuação

Modelo inicial:

- 1000 pontos iniciais;
- -3 por segundo;
- -20 por unidade de água perdida;
- mínimo 100.

Resultado entra no Top 5.

Pode futuramente ganhar bônus por economia, eficiência e segurança.

## 31. High scores

Top 5 persistente global.

Estrelas são armazenadas individualmente por fase.

## 32. Visual aprovado

Direção visual atual:

- tabuleiro claro;
- grid simples;
- 10×7;
- canos escuros/metálicos;
- água azul/teal visível dentro do cano;
- fonte e destino fáceis de reconhecer;
- fundo externo escuro;
- HUD escuro;
- controles simples;
- pressure bar verde/amarelo/vermelho.

Não adicionar sombras, gradientes ou efeitos caros sem necessidade.

## 33. Performance

Referência do hardware:

- orçamento aproximado de 16 ms/frame para 60 FPS;
- poucos objetos mudando: ~5–14 ms;
- muitas áreas mudando: até ~190 ms.

Portanto:

- evitar fillScreen por frame;
- usar framebuffer persistente;
- redraw incremental;
- dirty cells;
- buffers fixos;
- evitar heap no loop.

## 34. Áudio

Áudio deve ser assíncrono.

Nunca usar escrita I2S bloqueante dentro do game loop.

Eventos possíveis:

- colocar;
- girar;
- remover;
- água iniciar;
- pressão;
- ruptura;
- válvula;
- mixer;
- vitória;
- derrota.

## 35. Arquitetura

Separação atual/conceitual:

- water_pipe.cpp — loop principal
- wp_board — estado do tabuleiro
- wp_simulation — água/conectividade
- wp_pieces — tipos/rotações
- wp_levels — fases
- wp_inventory — NEXT/HOLD
- wp_input — touch
- wp_render — desenho
- wp_layout — geometria
- wp_time — tempo
- menu/progress/save/scores — campanha

A simulação não deve depender do renderer.

## 36. Estado atual do código

O repositório já possui uma base organizada para:

- board;
- pieces;
- simulation;
- levels;
- inventory;
- input;
- renderer;
- layout;
- timing;
- testes.

Existe ambiente de testes/demo para lógica.

O próximo objetivo é validar e ajustar o jogo no CrowPanel real.

## 37. MVP

O MVP precisa provar:

- tabuleiro;
- placement;
- rotação;
- remoção;
- conexão;
- fonte;
- destino;
- água;
- preenchimento;
- perda;
- vitória;
- derrota;
- restart;
- primeira fase.

Não implementar tudo de uma vez.

Temperatura, bombas, vista lateral, Mixer, campanha completa e física avançada entram depois que o loop básico estiver divertido no hardware.

## 38. Ordem recomendada

1. Confirmar visual no CrowPanel.
2. Confirmar touch.
3. Confirmar placement.
4. Confirmar rotação.
5. Confirmar hold-to-remove.
6. Confirmar água enchendo.
7. Confirmar fluxo.
8. Confirmar perda.
9. Confirmar vitória.
10. Confirmar derrota.
11. Ajustar velocidade e feeling.
12. Criar primeiras fases.
13. Implementar menu.
14. Implementar save/resume.
15. Implementar scores.
16. Pressão + válvula.
17. PVC/Copper.
18. Reservoir.
19. Splitters.
20. Vista lateral.
21. Bombas.
22. Temperatura.
23. Mixer.
24. Campanha completa.

## 39. Princípios

1. O jogador deve entender pela imagem.
2. Realismo simplificado.
3. A água reage enquanto o jogador constrói.
4. Erros são recuperáveis, mas não apagados por Undo.
5. Recursos importam.
6. Cada fase pode fazer uma pergunta diferente.
7. Hardware manda no visual.

## 40. Pontos ainda abertos

Ajustar por testes:

- velocidade da água;
- valores de pressão;
- custos;
- capacidade;
- thresholds de estrelas;
- quantidade de fases;
- quantidade de fases por mundo;
- balanceamento de bombas;
- aparência final dos tubos;
- sons;
- regras avançadas de fluxo;
- tela final de seleção de fases;
- tela de resultado.

## 41. Repositório

Projeto:

gcbraga6-afk/water-pipe

Documentação:

- docs/GAMEPLAY.md
- docs/WATER_SYSTEM.md
- docs/PIECES.md
- docs/LEVEL_DESIGN.md
- docs/TECHNICAL_DESIGN.md
- docs/PROJECT_HANDOFF.md

Este documento é o ponto de partida para continuar o projeto em outro chat.

Ao continuar:

1. consultar este documento;
2. consultar o código atual do repositório;
3. não reabrir decisões já fechadas;
4. tratar os pontos abertos como assuntos de teste/balanceamento;
5. manter o foco no hardware CrowPanel.
