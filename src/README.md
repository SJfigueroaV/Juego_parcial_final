# Dungeon Crawler — Documentación Técnica (Selección)

---

## El sistema de movimiento por turnos

Los enemigos no se mueven cada frame. Tienen un `moveTimer` que **decrece cada frame** y, cuando llega a cero, dan **un paso de grid** (cambia su `row/col`). Entre pasos, su posición en píxeles **se interpola suavemente** hacia el destino:

```cpp
enemies[i].moveTimer--;
if (enemies[i].moveTimer <= 0) {
    enemies[i].moveTimer = effectiveInterval;
    if (enemies[i].type == CHASER) {
        updateChaser(...);
    } else {
        updatePatrol(...);
    }
}

// Interpolación de píxeles hacia el tile destino
float targetX = (float)(col * TILE_SIZE);
float step    = pixelSpeed * dt;
if (dist <= step) { x = targetX; }
else              { x += dx/dist * step; }
```

Esta arquitectura **"turnos + interpolación"** permite IA simple por tiles (decisiones discretas) con animación suave entre tiles. Es un patrón muy común en juegos roguelike modernos.

---

## 8.3. El knockback físico

Cuando un CHASER intenta moverse pero queda bloqueado por un barril, se dispara una **velocidad de knockback** en dirección opuesta a la dirección de movimiento intentado:

```cpp
if (e.row == prevRow && e.col == prevCol) {  // no se movió
    bool blockedByBarrel = ...;
    if (blockedByBarrel) {
        float bLen = (sr != 0 && sc != 0) ? 1.4142f : 1.0f;
        e.knockbackVX = (float)(-sc) / bLen * 280.0f;  // 280 px/s
        e.knockbackVY = (float)(-sr) / bLen * 280.0f;
    }
}
```

Después, en cada frame, mientras la magnitud de la velocidad sea mayor a 4 px/s, el enemigo se mueve por **física pura** ignorando los turnos:

```cpp
if (knockMag > 4.0f) {
    float decay = std::pow(0.004f, dt);  // frenado exponencial
    e.x += e.knockbackVX * dt;
    e.y += e.knockbackVY * dt;
    e.knockbackVX *= decay;
    e.knockbackVY *= decay;
    // resincronizar row/col desde píxel
    e.col = (int)((e.x + TILE_SIZE * 0.5f) / TILE_SIZE);
    e.row = (int)((e.y + TILE_SIZE * 0.5f) / TILE_SIZE);
}
```

> **¿Por qué `pow(0.004, dt)`?** Si quieres que un valor decaiga al 0.4% en 1 segundo (factor 0.004), la fórmula `v *= pow(0.004, dt)` aplicada cada frame da exactamente ese decaimiento independientemente del framerate. Es la fórmula clásica de **fricción exponencial** en tiempo discreto.

### Doble defensa contra entrar al tile del barril

Para que un enemigo jamás se superponga visualmente con un barril:

1. **Defensa de grid (push-off):** al inicio de cada frame, si el enemigo está en la misma celda que un barril (puede haber pasado por knockback), se empuja al tile libre más cercano.
2. **Defensa de píxel:** durante la interpolación, si el siguiente píxel cae dentro del tile del barril, el movimiento se cancela.

---

## Sistema de ítems

### Tipos de ítem

| Tipo | Nombre | Efecto | placeable | singleUse |
|---|---|---|---|---|
| `NONE` | — | Slot vacío | — | — |
| `TRAP` | Trampa | Congela permanentemente al enemigo que la pise | Sí | Sí |
| `SPEEDBOOST` | Velocidad | ×1.5 velocidad por 1.5s, cooldown 5s | No | No |
| `BARREL` | Barril | Tile sólido; bloquea enemigos y causa knockback | Sí | No |
| `KEY` | Llave | Desbloquea puertas (auto-pickup) | No | Sí |
| `COPA` | Copa | Llave maestra para la victoria | No | Sí |

### La estructura Item

```cpp
struct Item {
    ItemType type;          // NONE, TRAP, BARREL, KEY, COPA, SPEEDBOOST
    char     name[32];       // nombre visible
    bool     placeable;     // se puede dejar en el suelo
    bool     singleUse;     // se consume tras un uso
    int      row, col;      // posición si está en el suelo
    int      roomIndex;
    bool     isOnFloor;     // true = en suelo, false = inventario
    bool     active;        // efecto en curso
    float    effectTimer;   // duración del efecto
    float    cooldownTimer; // cooldown post-uso
};
```

### Tres "almacenes" de ítems

El juego maneja ítems en **tres lugares distintos**:

- **`worldItems[MAX_ROOMS]`** — Un ítem fijo por sala (llaves, copa). Aparecen al inicio, se recogen pisando o por proximidad.
- **`inv.slots[MAX_SLOTS]`** — El inventario del jugador. Se llena al recoger ítems o abrir cofres con `E`.
- **`floorItems[MAX_SLOTS]`** — Ítems que el jugador ha *dejado* en el suelo (`P` o `Q`). Pueden volverse a recoger.

> **Decisión de diseño:** originalmente los ítems en el suelo ocupaban un slot del inventario. Esto causaba bugs: el panel mostraba slots "vacíos" pero internamente estaban ocupados. La solución fue separar completamente `floorItems[]` del inventario.

### Pickup de ítems (auto y manual)

```
cada frame
    └─ worldItems[room].type
          ├─ == KEY  → pickup por casilla (row == player.row)
          │              según sala: room 0 → heldKeyId=1
          │                          room 1 → heldKeyId=4
          │                          room 2 → heldKeyId=2
          ├─ == COPA → pickup por distancia (dist² ≤ 2 × TILE²)
          │              heldCopa = true, copa.type = NONE
          └─ otro    → no hacer nada
```

### Efectos de ítems en el suelo

La función `checkItemEffects()` recorre los ítems en el suelo y verifica si algún enemigo está sobre ellos:

```cpp
if (item.type == TRAP) {
    if (item.active) continue;  // ya disparada
    for (int e = 0; e < enemyCount; e++) {
        if (enemies[e].row == item.row && enemies[e].col == item.col) {
            enemies[e].frozenTimer = 1.0e9f;  // permanente
            item.active = true;
        }
    }
}
```

El TRAP queda visualmente en el suelo (no desaparece), pero ya no congelará a otros enemigos.

El BARREL no congela: invierte la dirección del enemigo y le pone un `frozenTimer = 0.3s` para que la inversión sea visible. Su comportamiento principal (bloquear movimiento físicamente) se maneja desde `enemy.cpp`.

### Timer de SPEEDBOOST

```cpp
if (item.type == SPEEDBOOST) {
    if (item.active) {
        item.effectTimer -= dt;
        if (item.effectTimer <= 0.0f) {
            item.active        = false;
            player.speed       = 210.0f;       // volver a velocidad base
            item.cooldownTimer = 5.0f;         // 5s de cooldown
        }
    } else if (item.cooldownTimer > 0.0f) {
        item.cooldownTimer -= dt;
    }
}
```

---

## El inventario

```cpp
struct Inventory {
    Item slots[MAX_SLOTS];  // 5 slots internos
    int  selectedSlot;       // 0..4
    int  itemCount;          // cuántos ítems no-NONE hay
};
```

### Operaciones disponibles

| Función | Qué hace |
|---|---|
| `initInventory(inv)` | Pone todos los slots a NONE. |
| `addItem(inv, type)` | Inserta un ítem en el primer slot libre. `false` si está lleno. |
| `addItemToSlot(inv, slot, type)` | Inserta en slot específico. |
| `removeItem(inv, slot)` | Vacía un slot. |
| `selectSlot(inv, slot)` | Cambia el slot seleccionado. |
| `placeItem(inv, slot, row, col)` | Marca un ítem como en suelo. |
| `drawInventory(inv, window, font)` | Dibuja el panel lateral. |

### Inputs de teclado

| Tecla | Acción |
|---|---|
| `1` `2` `3` | Seleccionar slot 1, 2 o 3 del inventario. |
| `E` | Recoger ítem (suelo o cofre); o activar/colocar el ítem seleccionado. |
| `P` | Colocar el ítem seleccionado en el suelo (si es placeable). |
| `Q` | Descartar el ítem seleccionado al suelo. |

### El renderizado del panel

El panel del inventario es una franja vertical de 100 px en el lado derecho de la ventana (cols 832-932). Solo muestra 3 slots, centrados verticalmente, cada uno de 80×65 px.

```
┌──────────────────┐
│   PANEL          │
│   100 × 640 px   │
│                  │
│  ┌────────────┐  │
│  │ 1  Llave   │  │  ← slot seleccionado (borde amarillo)
│  └────────────┘  │
│  ┌────────────┐  │
│  │ 2  Trampa  │  │
│  └────────────┘  │
│  ┌────────────┐  │
│  │ 3  (vacío) │  │
│  └────────────┘  │
│                  │
└──────────────────┘
```

### Ítem seleccionado a la izquierda del jugador

Como detalle de inmersión, el ítem actualmente seleccionado se dibuja como un mini sprite en la "mano izquierda" del jugador (estilo Minecraft 2D):

```cpp
float ix = player.x - (float)TILE_SIZE * 0.65f;
float iy = player.y + (float)TILE_SIZE * 0.3f;
const float ITEM_SCALE = (float)TILE_SIZE / 16.0f * 0.6f;
```

Se elige el rect del sprite según el tipo: `{0,64}` para TRAP, `{160,16}` para SPEEDBOOST, `{64,64}` para BARREL. La llave usa la textura aparte (`keys_g.png`).

---

## El game loop principal

Esta es la sección **más importante** del proyecto. Toda la lógica del juego converge aquí. Vive en `runGame()` dentro de `src/game.cpp`.

### Estructura general

```cpp
void runGame() {
    // 1. Crear ventana, cargar fuentes, cargar texturas
    ...
    while (window.isOpen() && keepRunning) {
        // 2. Inicializar partida (variables locales)
        Room rooms[MAX_ROOMS]; ...
        while (window.isOpen() && !restartRequested) {
            float dt = clock.restart().asSeconds();
            // 3. Procesar eventos (input)
            ...
            if (state == PLAYING) {
                // 4. Actualizar lógica
                ...
            }
            // 5. Renderizar
            window.clear(...);
            ...
            window.display();
        }
    }
}
```

### Diagrama de flujo — un frame completo

```
┌─────────────────────────────────┐
│         INICIO DE FRAME         │
└────────────────┬────────────────┘
                 │
        dt = clock.restart().asSeconds()
                 │
        window.pollEvent()
        (Closed / Resized / KeyPressed)
                 │
         ┌───────┴────────┐
         │ state==PLAYING? │
         └───────┬────────┘
          NO ────┤          SÍ
          │      │          │
          │   spawnTimer -= dt
          │      │
          │   handleInput(player, room, dt)
          │      │
          │   auto-pickup llave / copa
          │      │
          │   auto-unlock T_SDOOR / T_LDOOR / T_DLOCKED
          │      │
          │   updateEnemies(...)
          │   (turnos + interpolación + knockback)
          │      │
          │   checkItemEffects + updateItemTimers
          │      │
          │   getDoorAt(player.row, player.col)
          │      │
          │   ┌──┴──────────────────┐
          │   │ door && !prevOnDoor  │
          │   │ && canEnter?         │
          │   └──┬──────────────────┘
          │    SÍ│ currentRoom = toRoom
          │      │ spawnTimer = 1.5s
          │      │
          │   checkDefeat → si true: state = DEFEAT
          │      │
          ├──────┘
          │
     window.clear(color)
          │
     drawRoom + drawItems +
     drawEnemies + drawPlayer + HUD
          │
     window.display()
```

### Detalle: las dos lecturas de la puerta (prevOnDoor)

Un detalle sutil del game loop: ¿qué pasa si el jugador está sobre una puerta y la transición ya ocurrió? Sin protección, cada frame volvería a teletransportarse.

```cpp
const Door *door = getDoorAt(rooms[currentRoom], player.row, player.col);
if (door != nullptr && !prevOnDoor) {
    // transición a otra sala
}
prevOnDoor = (door != nullptr);
```

La variable `prevOnDoor` recuerda si en el frame anterior estábamos sobre una puerta. Solo se ejecuta la transición en la **transición** de "no estaba en puerta → ahora sí". Esto evita bucles infinitos de teletransportación.

### Detección de derrota (checkDefeat)

```cpp
bool checkDefeat(const Player &p, const Enemy enemies[], int count, int currentRoom) {
    if (p.immune) return false;
    for (int i = 0; i < count; i++) {
        if (!enemies[i].alive || enemies[i].roomIndex != currentRoom) continue;
        if (enemies[i].frozenTimer > 0.0f) continue;
        float dx = enemies[i].x - p.x;
        float dy = enemies[i].y - p.y;
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if (dx < TILE_SIZE * 0.5f && dy < TILE_SIZE * 0.5f) return true;
    }
    return false;
}
```

Tres condiciones para no morir:
1. `p.immune == true` (durante el spawn de 1.5s)
2. El enemigo no está vivo o está en otra sala
3. El enemigo está congelado (`frozenTimer > 0`)

Si ninguna se cumple y la distancia en **X y Y por separado** es menor que medio tile, hay colisión. Es una colisión por *Manhattan-half-tile*: ambos ejes deben estar dentro del rango.

---

## Estados del juego

```cpp
enum GameState {
    PLAYING,    // jugando normal
    VICTORY,    // llegó a la copa + T_SDOOR
    DEFEAT      // fue atrapado por un enemigo
};
```

### Máquina de estados

```
                    runGame() start
                          │
                          ▼
                    ┌───────────┐
          ┌────────►│  PLAYING  │◄────────┐
          │         └─────┬─────┘         │
          │               │               │
          │   copa+T_SDOOR│   enemigo     │
          │               │  < 0.5 tile   │
          │               ▼               │
          │   ┌─────────────────────┐     │
          │   │  VICTORY  │ DEFEAT  │     │
          │   └─────────────────────┘     │
          │               │               │
          └───────── tecla R ─────────────┘
                   (reset world)
```

### Transiciones

| Desde | Hasta | Disparador |
|---|---|---|
| (inicio) | PLAYING | Al entrar a `runGame()` |
| PLAYING | VICTORY | `heldCopa && tile adyacente == T_SDOOR` |
| PLAYING | DEFEAT | `spawnTimer == 0` + enemigo a menos de 0.5 tile |
| VICTORY / DEFEAT | (restart) | `R` → reset completo de la partida |

### Overlays según estado

Cuando el estado no es PLAYING, se dibuja un overlay semi-transparente sobre toda la pantalla:

- **VICTORY:** texto amarillo "VICTORIA!" centrado, con instrucción `R`/`ESC`.
- **DEFEAT:** pantalla "GAME OVER" con el nivel alcanzado, el récord, y la instrucción "R para reiniciar".

---

## Sistema de renderizado

SFML usa el patrón clásico de **clear → draw → display**:

```cpp
window.clear(sf::Color(10, 10, 10));   // limpia el back buffer
window.draw(...);                       // dibuja todo lo que quieras
window.draw(...);
...
window.display();                       // swap buffers — muestra el frame
```

El concepto detrás: hay dos buffers (*front* y *back*). Mientras dibujas, lo haces sobre el *back*. Al llamar a `display()` se intercambian los buffers, y de golpe el usuario ve el frame completo. Esto evita el *tearing* visual.

### Orden de dibujo (layering)

Como SFML no tiene Z-order automático, el orden importa: lo que se dibuja después, queda **encima**.

```
  Z menor
    ▲
    │  1. window.clear(rgb 10,10,10)
    │  2. drawRoom(rooms[currentRoom], ...)
    │  3. drawWorldItem (llave/copa fija)
    │  4. copa especial sobre pedestal (si sala 5)
    │  5. drawFloorItems (ítems dejados)
    │  6. drawEnemies
    │  7. drawPlayer + ítem en mano
    │  8. HUD: healthBar + statusBar + inventario + overlays
    ▼
  Z mayor
```

### Escalado de la ventana (makeScaledView)

El juego está pensado para 932×640 px, pero la ventana real puede ser más grande. Para arreglar esto, se usa una **View** de SFML que escala el contenido manteniendo proporción:

```cpp
static sf::View makeScaledView(unsigned int w, unsigned int h) {
    float sx = (float)w / (float)WINDOW_W;
    float sy = (float)h / (float)WINDOW_H;
    float s  = sx < sy ? sx : sy;        // el menor de los dos factores
    s = std::floor(s * 2.0f) / 2.0f;       // snap a múltiplos de 0.5
    ...
}
```

La **View** es como una "cámara virtual" que define qué porción del mundo se ve y dónde se proyecta en la ventana. Configurándola con un *viewport* calculado, podemos centrar el juego y añadir letterboxing automáticamente.
