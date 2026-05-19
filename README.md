# Dungeon Crawler — Parcial Final

Juego top-down en C++17 con SFML 3.x. El objetivo es atravesar 6 habitaciones conectadas, esquivar enemigos, recoger una llave y llegar a la sala final.

**Autores:** Santiago Figueroa, Juan Betancurt

---

## Controles

| Tecla | Acción |
|-------|--------|
| `W` `A` `S` `D` / `↑` `↓` `←` `→` | Mover al jugador |
| `E` | Recoger objeto del suelo / interactuar |
| `P` | Colocar el item del slot seleccionado en el suelo |
| `Q` | Descartar el item del slot seleccionado |
| `1` `2` `3` | Seleccionar slot del inventario |
| `R` | Reiniciar (cuando aparece game over) |
| `Esc` | Salir |

---

## Cómo pasarse el juego

El mapa tiene 6 salas conectadas así:

```
[Sala 0] ──este──► [Sala 1] ──sur──► [Sala 2] ──este (BLOQUEADA)──► [Sala 3: FINAL]
                                          └──sur──► [Sala 4] ──sur──► [Sala 5: copa]
```

**Pasos para la victoria:**

1. Comienzas en la **sala 0**. Avanza por el corredor norte-sur esquivando los dos enemigos patrulleros naranjas (rebotan en las paredes).
2. Cruza al **este** para entrar a la **sala 1** — esta tiene **niebla de guerra**, sólo verás 5 tiles alrededor tuyo.
3. Baja al **sur** a la **sala 2**, que tiene un laberinto. Recoge la **llave** dorada (`E` al pisar sobre ella).
4. La puerta **este** de la sala 2 conecta a la **sala 3** pero está **bloqueada**. Con la llave en el inventario, se desbloquea automáticamente.
5. **Sala 3 = VICTORIA.**

**Cómo morir (a evitar):**
- Quedarte a menos de medio tile de cualquier enemigo activo.
- Las salas 4 y 5 tienen **chasers rojos** rápidos que persiguen en diagonal. La sala 4 sirve de "trampa" si bajas por error desde la 2.

**Items que ayudan:**
- **Trampa (morado)** — la colocas con `P`. El primer enemigo que la pise queda congelado permanentemente.
- **Barril (marrón)** — bloqueo sólido. Los chasers rebotan con knockback físico al chocar.
- **Velocidad (verde)** — activa un boost de ×1.5 durante 3s. Tiene cooldown de 5s.

---

## Innovaciones del equipo

### 1. Knockback físico al chocar chaser contra barril
**Qué hace:** cuando un chaser rojo intenta moverse hacia un tile con barril, se le aplica una velocidad de 280 px/s en dirección opuesta que decae exponencialmente (`pow(0.004, dt)`) hasta detenerse en ~0.5s.

**Por qué lo agregamos:** la mecánica original de "el chaser se queda quieto al chocar" se veía artificial. El knockback físico continuo lo hace sentir como un objeto rígido empujando otro, y le da al jugador una herramienta táctica real (no sólo bloqueo estático).

### 2. Niebla de guerra en sala 1
**Qué hace:** sólo se renderizan tiles dentro de un cuadrado de 5×5 alrededor del jugador; el resto queda en negro.

**Por qué:** una sola sala con visión limitada agrega tensión sin frustrar al jugador en todo el mapa. Funciona como "puente psicológico" entre la sala de aprendizaje (sala 0) y el laberinto (sala 2).

### 3. Floor items desacoplados del inventario
**Qué hace:** los items colocados en el suelo con `P` no ocupan slot del inventario — se guardan en un array independiente. Esto permite colocar una trampa y seguir recogiendo cofres sin perder espacio visible.

**Por qué:** la versión inicial mezclaba ambos (un item en el suelo seguía ocupando un slot del panel), lo que confundía al jugador porque cofres posteriores parecían "no funcionar" (entregaban items a slots ocultos).

### 4. Pixel art procedural para enemigos
**Qué hace:** los chasers y patrulleros se dibujan con una matriz 7×7 hardcoded y una paleta de 4 colores, no con un sprite externo.

**Por qué:** evita la dependencia de assets adicionales y nos da control fino sobre el "look" del enemigo. También facilita agregar más tipos sin tener que conseguir/editar imágenes.

### 5. Cooldown visible en el SPEEDBOOST
**Qué hace:** después de activar la velocidad, el slot muestra "CD: 4.2s" en naranja hasta que termina el cooldown.

**Por qué:** sin el indicador, el jugador no sabía si la velocidad ya estaba lista o si la había usado por equivocación. El texto en el slot da feedback inmediato.

---

## Compilación e instalación

### Dependencias

- Compilador C++17 (g++ ≥ 9, clang++ ≥ 10)
- CMake ≥ 3.16
- **SFML 3.x** con componente Graphics

> ⚠️ Importante: este proyecto usa la API de **SFML 3** (constructores con `sf::Vector2u`, `sf::Event` como variant con `is<T>()`, etc.). **NO compila con SFML 2.x**.

### Instalar SFML 3 según el sistema operativo

**Arch Linux / Manjaro** (recomendado, viene con SFML 3 por defecto):
```bash
sudo pacman -S sfml cmake
```

**Ubuntu 24.04+** (verificar versión):
```bash
sudo apt install libsfml-dev cmake build-essential
apt show libsfml-dev   # confirmar que es >= 3.0
```

**Ubuntu 22.04 o anterior** (los repos tienen SFML 2.x — compilar SFML 3 desde código):
```bash
sudo apt install cmake build-essential libgl1-mesa-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev libflac-dev libvorbis-dev libopenal-dev
git clone --branch 3.0.0 https://github.com/SFML/SFML.git
cd SFML && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build . --parallel
sudo cmake --install .
sudo ldconfig
```

**Fedora**:
```bash
sudo dnf install SFML-devel cmake gcc-c++
```

**macOS (Homebrew)**:
```bash
brew install sfml cmake
```

### Compilar el proyecto

Desde la raíz del repo:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

Si SFML quedó instalado en una ruta no estándar (caso de compilarlo manualmente), indica la ruta con `SFML_DIR`:

```bash
cmake -DSFML_DIR=/usr/local/lib/cmake/SFML ..
```

### Ejecutar

```bash
cd build
./dungeon
```

> ⚠️ **Importante**: el ejecutable debe correrse desde dentro del directorio `build/` porque las rutas de los sprites son relativas (`../include/sprites/...`).

### Notas adicionales para el profesor

- Si la HUD se ve sin texto (sólo cuadros), el sistema no tiene ninguna de las fuentes que busca el juego (`NotoSans-Regular.ttf`, `DejaVuSans.ttf`). El juego sigue siendo jugable — sólo no se muestra el HP numérico ni el contador de nivel. Para activar el texto: `sudo apt install fonts-dejavu` (Ubuntu) o equivalente.
- Si al ejecutar aparece "no such file or directory" para los sprites: verificar que se está corriendo desde `build/`, no desde la raíz.
- La ventana se escala automáticamente al tamaño del escritorio manteniendo proporción 4:3.

---

## Restricciones de diseño cumplidas

- ✅ Lenguaje: C++17 puro
- ✅ Mínimo 6 habitaciones conectadas (las tenemos exactas)
- ✅ Colisiones funcionales (AABB con sliding por ejes)
- ✅ 2 tipos de enemigos con persecución (`PERSEGUIDOR`, `PATRULLERO`)
- ✅ Render visual del entorno, jugador y enemigos
- ✅ Sistema de inventario con recoger/soltar
- ✅ Condiciones claras de victoria y derrota
- ✅ Código en múltiples archivos `.h` / `.cpp`
- ✅ Uso verificable de punteros (ej. `const Puerta *obtenerPuertaEn(...)`, `const sf::Texture *obtenerTileset()`)
- ✅ Build automatizado con CMake
- ✅ Sólo arreglos estáticos — cero `new` / `delete` en todo el código

---

## Estructura del proyecto

```
dungeon/
├── CMakeLists.txt           # build config
├── README.md                # este archivo
├── include/                 # headers
│   ├── game.h               # estados, loop, victoria/derrota
│   ├── map.h                # Habitacion, Puerta, constantes
│   ├── player.h             # Jugador, controles
│   ├── enemy.h              # Enemigo, PERSEGUIDOR/PATRULLERO
│   ├── inventory.h          # Inventario, Objeto, slots
│   ├── items.h              # efectos de items, recolección
│   └── sprites/             # PNGs (tileset, jugador, props, llave)
└── src/                     # implementaciones (mismos nombres)
```
