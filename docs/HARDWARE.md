# CrowPanel Game Reference

Frida · Hardware Reference

O que um novo jogo (ex.: Water Pipe) precisa saber sobre esta placa — canvas, toque, áudio, memória e os limites reais medidos no Artilharia.

## 1. Placa e canvas

| Item | Valor |
|---|---|
| MCU | ESP32-S3-WROOM-1, dual core, 240MHz |
| Painel | RGB paralelo 800×480, 16-bit (RGB565) |
| PSRAM | 8MB (quadro lógico do jogo já reserva 768000 bytes nela) |
| Heap interno livre | ~129KB em runtime, com toda a UI principal já construída |
| Toque | GT911, um único ponto de toque (sem multitoque) |

Sem aceleração 2D por hardware — todo desenho é feito em software, pixel a pixel, via `GfxCompat` (`src/core/GfxCompat.h`).

## 2. Como um jogo desenha

Um jogo não cria seu próprio painel — ele recebe um ponteiro global já pronto:

```cpp
extern GfxCompat *gfx;   // src/core/GfxCompat.h
```

Primitivas disponíveis (todas em `gfx->`):

| Função | Nota |
|---|---|
| `fillScreen(color)` | tela inteira |
| `fillRect` / `drawPixel` | base de tudo |
| `fillCircle` / `drawLine` | Bresenham simples |
| `drawRoundRect` / `fillRoundRect` | cantos via helper de círculo |
| `fillTriangle` | scanline clássico |
| `color565(r,g,b)` | conversão de cor, estático |
| `print` / `printf` | stub vazio — texto ainda não implementado, ver §5 |

### Ciclo de frame

O `main.cpp` envolve cada iteração do jogo assim (não precisa mexer nisso, só saber que existe):

```cpp
gfx->beginFrame();
MeuJogo::loop();   // todo o desenho acontece aqui dentro
gfx->endFrame();   // publica no painel real, espera vsync
```

Enquanto o app ativo é "Jogos", a LVGL **não roda** (main.cpp pula `lv_timer_handler()`) — o jogo tem a tela inteira pra ele, sem concorrência de outro sistema de desenho.

> **Padrão de desenho: incremental, não full-clear**
>
> O desenho acontece sempre no **mesmo buffer lógico persistente** — o que foi desenhado num frame continua lá no próximo, exatamente como um framebuffer único de verdade. Isso significa: só repinte o que mudou (posição nova de um objeto, célula que virou), não redesenhe a tela inteira a cada frame. Um `fillScreen()` por frame é caro e desnecessário.

### Custo real medido (Artilharia, 2026-09-02)

| Cenário | Tempo por frame |
|---|---|
| Poucos objetos mudando (mira, bola voando) | 5–14ms — ok |
| Muitas áreas mudando de uma vez (destruição de terreno, várias partículas) | até ~190ms em picos — evitar |

Orçamento de referência: 16ms/frame = 60fps. As primitivas são loops de software (sem otimização de linha ainda), então áreas grandes preenchidas de uma vez custam proporcionalmente — para Water Pipe, evite recolorir tabuleiros inteiros a cada frame; redesenhe só as células/tubos que mudaram de estado.

## 3. Toque

```cpp
TouchDriver::rawTouched()             // bool, ja em cache (nao faz I2C aqui)
TouchDriver::rawX() / rawY()          // coordenadas cruas
TouchDriver::consumeTapInArea(x0,y0,x1,y1)  // true uma vez por toque dentro da area
```

`consumeTapInArea` é o padrão já usado no menu de Jogos e no Artilharia (botão de sair, seletor de arma, tiles do menu) — ideal pra um jogo de grade como Water Pipe: uma área por célula, toque gira/rotaciona a peça.

Um único ponto de toque por vez — sem gestos multitoque.

## 4. Áudio — assíncrono, seguro pra chamar do loop

> **Lição do Artilharia**
>
> Até 2026-09-02, os efeitos sonoros eram **bloqueantes** (`i2s_write` com `portMAX_DELAY`): cada tiro travava o jogo inteiro por ~90ms, cada explosão por ~190ms — desenho, vsync e leitura de toque parados juntos. Corrigido: agora os efeitos rodam numa task FreeRTOS dedicada (core 0); `Audio::tiro()` etc. só enfileiram e retornam na hora.

Pra Water Pipe, siga o mesmo padrão em `src/core/audio.cpp`: adicione um caso no `enum class Efeito`, uma função `tocarXxx()` com a implementação (pode reusar `tocarBeep`/`tocarBeepSweep`), e uma função pública que só chama `enfileirar(Efeito::XXX)`. **Nunca chame `i2s_write` direto de dentro do loop do jogo.**

## 5. Texto — ainda não resolvido

`gfx->print()`/`printf()` existem na assinatura mas são no-op — nenhum texto aparece hoje via desenho cru. Se Water Pipe precisar de placar/rótulos, duas opções antes de escrever um rasterizador de fonte do zero:

1. Portar a rotina de glifo clássica da Adafruit_GFX (~40 linhas, formato GFXfont) — funciona com as fontes que o projeto já usa.
2. Se o texto for só pontual (não a cada frame), considerar desenhar via LVGL sobreposto — mas hoje a LVGL fica desligada enquanto Jogos está ativo, então isso exigiria repensar essa exclusão mútua.

## 6. Onde um novo jogo se encaixa

```text
src/games/water_pipe.h / .cpp     // begin() + loop(), mesmo contrato do Artilharia
src/ui/ui_jogos.cpp               // GameEntry no array games[], troca de Mode
```

`UiJogos` já tem um `enum class Mode` com um valor por jogo — Water Pipe entra como mais um valor, seguindo exatamente o padrão do Artilharia (ícone no menu 3×3, `begin()` ao tocar o tile, `loop()` retornando um booleano de "quero sair").

---

Frida · CrowPanel Advance 7" (ESP32-S3-WROOM-1) · referência viva — atualizar se as primitivas de desenho ou o padrão de áudio mudarem.
