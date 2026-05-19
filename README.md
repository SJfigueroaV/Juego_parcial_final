# Dungeons — Parcial Final

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

1. **Uso de un set de assets para la mejora gráfica.** Pack utilizado: [Dungeon Asset Puck](https://pixel-poem.itch.io/dungeon-assetpuck?download#google_vignette).
2. **Implementación de 3 items nuevos** (barril, velocidad, trampa) con los que el jugador puede interactuar para que sea más divertido el juego.
3. **Implementación de una barra de inventario de 3 slots** para que el jugador sepa qué tiene equipado.
4. **Implementación de un sistema progresivo por salas** que están bloqueadas por sus correspondientes llaves.
5. **Implementación de la mecánica de niebla en la sala 3** para completar el laberinto.
6. **Implementación de cofres** en los que el jugador interactúa presionando la tecla `E` para obtener cierto item.
7. **Implementación de movimiento fluido del personaje** con coordenadas flotantes, y así mismo con los enemigos.

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

### Notas adicionales

- Si la HUD se ve sin texto (sólo cuadros), el sistema no tiene ninguna de las fuentes que busca el juego (`NotoSans-Regular.ttf`, `DejaVuSans.ttf`). El juego sigue siendo jugable — sólo no se muestra el HP numérico ni el contador de nivel. Para activar el texto: `sudo apt install fonts-dejavu` (Ubuntu) o equivalente.
- Si al ejecutar aparece "no such file or directory" para los sprites: verificar que se está corriendo desde `build/`, no desde la raíz.
- La ventana se escala automáticamente al tamaño del escritorio manteniendo proporción 4:3.

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
