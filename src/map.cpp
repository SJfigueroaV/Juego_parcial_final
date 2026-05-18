#include "../include/map.h"
#include "../include/player.h"
#include <string>

static sf::Texture g_tileset;
static bool        g_tileLoaded = false;
static sf::Texture g_gateTex;
static bool        g_gateLoaded = false;
static sf::Texture g_keyTex;
static bool        g_keyLoaded  = false;

TileDef g_tiles[TILE_PALETTE_SIZE];

static const sf::IntRect R_WALL_TL  ({  0,  0}, {16, 16});
static const sf::IntRect R_WALL_T   ({ 16,  0}, {16, 16});
static const sf::IntRect R_WALL_TR  ({ 80,  0}, {16, 16});
static const sf::IntRect R_WALL_L   ({  0, 16}, {16, 16});
static const sf::IntRect R_WALL_R   ({ 80, 16}, {16, 16});
static const sf::IntRect R_WALL_BL  ({  0, 64}, {16, 16});
static const sf::IntRect R_WALL_B   ({ 16, 64}, {16, 16});
static const sf::IntRect R_WALL_BR  ({ 80, 64}, {16, 16});
static const sf::IntRect R_WALL_FILL({  0, 64}, {16, 16});
static const sf::IntRect R_FLOOR    ({ 16, 16}, {16, 16});
static const sf::IntRect R_DOOR     ({144, 48}, {16, 16});

static const sf::IntRect R_FRAME_TL ({ 16, 16}, {16, 16});
static const sf::IntRect R_FRAME_T  ({ 32, 16}, {16, 16});
static const sf::IntRect R_FRAME_TR ({ 64, 16}, {16, 16});
static const sf::IntRect R_FRAME_L  ({ 16, 32}, {16, 16});
static const sf::IntRect R_FRAME_R  ({ 64, 32}, {16, 16});
static const sf::IntRect R_FRAME_BL ({ 16, 48}, {16, 16});
static const sf::IntRect R_FRAME_B  ({ 32, 48}, {16, 16});
static const sf::IntRect R_FRAME_BR ({ 64, 48}, {16, 16});

static const sf::IntRect R_WALL_IL ({  0, 112}, {16, 16});
static const sf::IntRect R_WALL_IR ({ 16, 112}, {16, 16});

static const sf::IntRect R_GATE_A ({80, 48}, {16, 16});
static const sf::IntRect R_GATE_B ({96, 48}, {16, 16});
static const sf::IntRect R_CHEST  ({80, 64}, {16, 16});
static const sf::IntRect R_SDOOR  ({80, 80}, {16, 16});
static const sf::IntRect R_HOLDER ({32, 32}, {16, 16});

static void inicializarPaletaTiles() {
    for (int i = 0; i < TILE_PALETTE_SIZE; i++) {
        g_tiles[i].solid      = false;
        g_tiles[i].isDoor     = false;
        g_tiles[i].useColor   = true;
        g_tiles[i].useGateTex = false;
        g_tiles[i].color      = sf::Color(0, 0, 0);
        g_tiles[i].rect       = R_FLOOR;
    }

    g_tiles[T_W].solid    = true;
    g_tiles[T_W].useColor = true;
    g_tiles[T_W].color    = sf::Color(80, 80, 80);

    g_tiles[T_F].useColor = true;
    g_tiles[T_F].color    = sf::Color(0x25, 0x13, 0x1A);

    g_tiles[T_D].isDoor   = true;
    g_tiles[T_D].useColor = false;
    g_tiles[T_D].rect     = R_DOOR;

    g_tiles[T_FD].useColor = true;
    g_tiles[T_FD].color    = sf::Color(0x39, 0x33, 0x4A);

    g_tiles[T_WB].solid    = true;
    g_tiles[T_WB].useColor = false;
    g_tiles[T_WB].rect     = R_WALL_B;

    g_tiles[T_WT].solid    = true;
    g_tiles[T_WT].useColor = false;
    g_tiles[T_WT].rect     = R_WALL_T;

    g_tiles[T_WL].solid    = true;
    g_tiles[T_WL].useColor = false;
    g_tiles[T_WL].rect     = R_WALL_L;

    g_tiles[T_WR].solid    = true;
    g_tiles[T_WR].useColor = false;
    g_tiles[T_WR].rect     = R_WALL_R;

    g_tiles[T_WTL].solid    = true;
    g_tiles[T_WTL].useColor = false;
    g_tiles[T_WTL].rect     = R_WALL_TL;

    g_tiles[T_WTR].solid    = true;
    g_tiles[T_WTR].useColor = false;
    g_tiles[T_WTR].rect     = R_WALL_TR;

    g_tiles[T_WBL].solid    = true;
    g_tiles[T_WBL].useColor = false;
    g_tiles[T_WBL].rect     = R_WALL_BL;

    g_tiles[T_WBR].solid    = true;
    g_tiles[T_WBR].useColor = false;
    g_tiles[T_WBR].rect     = R_WALL_BR;

    g_tiles[T_WIL].solid    = true;
    g_tiles[T_WIL].useColor = false;
    g_tiles[T_WIL].rect     = R_WALL_IL;

    g_tiles[T_WIR].solid    = true;
    g_tiles[T_WIR].useColor = false;
    g_tiles[T_WIR].rect     = R_WALL_IR;

    g_tiles[T_LDOOR].solid  = true;
    g_tiles[T_LDOOR].isDoor = true;

    g_tiles[T_DLOCKED].solid    = true;
    g_tiles[T_DLOCKED].isDoor   = true;
    g_tiles[T_DLOCKED].useColor = false;

    g_tiles[T_PIL_L].solid    = true;
    g_tiles[T_PIL_L].useColor = false;
    g_tiles[T_PIL_L].rect     = sf::IntRect({0, 96}, {16, 16});

    g_tiles[T_PIL_R].solid    = true;
    g_tiles[T_PIL_R].useColor = false;
    g_tiles[T_PIL_R].rect     = sf::IntRect({16, 96}, {16, 16});

    g_tiles[T_FLOOR2].solid    = false;
    g_tiles[T_FLOOR2].useColor = false;
    g_tiles[T_FLOOR2].rect     = sf::IntRect({64, 96}, {16, 16});

    g_tiles[T_FLOOR3].solid    = false;
    g_tiles[T_FLOOR3].useColor = true;
    g_tiles[T_FLOOR3].color    = sf::Color(0x39, 0x33, 0x4A);

    g_tiles[T_CHEST].solid      = true;
    g_tiles[T_CHEST].useColor   = false;
    g_tiles[T_CHEST].useGateTex = true;
    g_tiles[T_CHEST].rect       = R_CHEST;

    g_tiles[T_SDOOR].solid    = true;
    g_tiles[T_SDOOR].useColor = false;
    g_tiles[T_SDOOR].rect     = R_SDOOR;

    g_tiles[T_HOLDER].solid      = true;
    g_tiles[T_HOLDER].useColor   = false;
    g_tiles[T_HOLDER].useGateTex = true;
    g_tiles[T_HOLDER].rect       = R_HOLDER;

    g_tiles[T_FTL].useColor = false;
    g_tiles[T_FTL].rect     = R_FRAME_TL;
    g_tiles[T_FT ].useColor = false;
    g_tiles[T_FT ].rect     = R_FRAME_T;
    g_tiles[T_FTR].useColor = false;
    g_tiles[T_FTR].rect     = R_FRAME_TR;
    g_tiles[T_FL ].useColor = false;
    g_tiles[T_FL ].rect     = R_FRAME_L;
    g_tiles[T_FR ].useColor = false;
    g_tiles[T_FR ].rect     = R_FRAME_R;
    g_tiles[T_FBL].useColor = false;
    g_tiles[T_FBL].rect     = R_FRAME_BL;
    g_tiles[T_FB ].useColor = false;
    g_tiles[T_FB ].rect     = R_FRAME_B;
    g_tiles[T_FBR].useColor = false;
    g_tiles[T_FBR].rect     = R_FRAME_BR;
}

void cargarTileset(const std::string &path) {
    g_tileLoaded = g_tileset.loadFromFile(path);
}
void cargarTexturaPuerta(const std::string &path) {
    g_gateLoaded = g_gateTex.loadFromFile(path);
}
void cargarTexturaLlave(const std::string &path) {
    g_keyLoaded = g_keyTex.loadFromFile(path);
}
const sf::Texture *obtenerTileset()     { return g_tileLoaded ? &g_tileset : nullptr; }
const sf::Texture *obtenerTexturaPuerta() { return g_gateLoaded ? &g_gateTex  : nullptr; }
const sf::Texture *obtenerTexturaLlave()  { return g_keyLoaded  ? &g_keyTex   : nullptr; }

static void llenarHabitacionBasica(Habitacion &room) {
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        for (int r = 0; r < MAX_FILAS; r++) {
            room.cuadricula[c][r] = T_F;
        }
    }

    room.cuadricula[0][0]                   = T_WTL;
    room.cuadricula[MAX_COLUMNAS-1][0]          = T_WTR;
    room.cuadricula[0][MAX_FILAS-1]          = T_WBL;
    room.cuadricula[MAX_COLUMNAS-1][MAX_FILAS-1] = T_WBR;

    for (int c = 1; c < MAX_COLUMNAS-1; c++) {
        room.cuadricula[c][0]          = T_WT;
        room.cuadricula[c][MAX_FILAS-1] = T_WB;
    }

    for (int r = 1; r < MAX_FILAS-1; r++) {
        room.cuadricula[0][r]          = T_WL;
        room.cuadricula[MAX_COLUMNAS-1][r] = T_WR;
    }

    room.numPuertas = 0;
}

static void agregarPuerta(Habitacion &room, int fila, int columna, int aHabitacion, bool bloqueada) {
    room.cuadricula[columna][fila]     = T_D;
    room.cuadricula[columna + 1][fila] = T_D;
    room.puertas[room.numPuertas].fila        = fila;
    room.puertas[room.numPuertas].columna     = columna;
    room.puertas[room.numPuertas].aHabitacion = aHabitacion;
    room.puertas[room.numPuertas].bloqueada   = bloqueada;
    room.numPuertas++;
    room.puertas[room.numPuertas].fila        = fila;
    room.puertas[room.numPuertas].columna     = columna + 1;
    room.puertas[room.numPuertas].aHabitacion = aHabitacion;
    room.puertas[room.numPuertas].bloqueada   = bloqueada;
    room.numPuertas++;
}

static void llenarSueloOscuro(Habitacion &room) {
    for (int c = 1; c < MAX_COLUMNAS-1; c++) {
        for (int r = 1; r < MAX_FILAS-1; r++) {
            if (room.cuadricula[c][r] == T_F) {
                room.cuadricula[c][r] = T_FD;
            }
        }
    }
}

static void agregarMarcoDecorativo(Habitacion &room) {
    room.cuadricula[1][1]                   = T_FTL;
    room.cuadricula[MAX_COLUMNAS-2][1]          = T_FTR;
    room.cuadricula[1][MAX_FILAS-2]          = T_FBL;
    room.cuadricula[MAX_COLUMNAS-2][MAX_FILAS-2] = T_FBR;

    for (int c = 2; c < MAX_COLUMNAS-2; c++) {
        room.cuadricula[c][1]          = T_FT;
        room.cuadricula[c][MAX_FILAS-2] = T_FB;
    }
    for (int r = 2; r < MAX_FILAS-2; r++) {
        room.cuadricula[1][r]          = T_FL;
        room.cuadricula[MAX_COLUMNAS-2][r] = T_FR;
    }
}

static void inicializarHabitacionVacia(Habitacion &room) {
    for (int c = 0; c < MAX_COLUMNAS; c++) {
        for (int r = 0; r < MAX_FILAS; r++) {
            room.cuadricula[c][r] = T_F;
        }
    }
    room.numPuertas = 0;
}

static void construirHabitacion0(Habitacion &room) {
    inicializarHabitacionVacia(room);
    agregarPuerta(room, MAX_FILAS-1, 12, 0, false);

    // Row 7 (r=6): top outer wall
    room.cuadricula[6][6]  = T_WTL;
    for (int c = 7; c <= 18; c++) room.cuadricula[c][6] = T_WT;
    room.cuadricula[19][6] = T_WTR;

    // Row 8 (r=7): frame top
    room.cuadricula[6][7]  = T_WTL;
    room.cuadricula[7][7]  = T_FTL;
    for (int c = 8; c <= 17; c++) room.cuadricula[c][7] = T_FT;
    room.cuadricula[18][7] = T_FTR;
    room.cuadricula[19][7] = T_WTR;

    // Rows 9-14 (r=8..13): interior with frame sides
    for (int r = 8; r <= 13; r++) {
        room.cuadricula[6][r]  = T_WTL;
        room.cuadricula[7][r]  = T_FL;
        for (int c = 8; c <= 17; c++) room.cuadricula[c][r] = T_FD;
        room.cuadricula[18][r] = T_FR;
        room.cuadricula[19][r] = T_WTR;
    }
    // Two chests in the interior (row 10)
    room.cuadricula[11][10] = T_CHEST;
    room.cuadricula[14][10] = T_CHEST;

    // Row 15 (r=14): frame bottom with door opening
    room.cuadricula[6][14]  = T_WTL;
    room.cuadricula[7][14]  = T_FBL;
    for (int c = 8; c <= 11; c++) room.cuadricula[c][14] = T_FB;
    room.cuadricula[12][14] = T_FD;
    room.cuadricula[13][14] = T_FD;
    for (int c = 14; c <= 17; c++) room.cuadricula[c][14] = T_FB;
    room.cuadricula[18][14] = T_FBR;
    room.cuadricula[19][14] = T_WTR;

    // Row 16 (r=15): outer bottom wall with corridor opening
    room.cuadricula[6][15]  = T_WBL;
    for (int c = 7; c <= 10; c++) room.cuadricula[c][15] = T_WB;
    room.cuadricula[11][15] = T_PIL_R;
    room.cuadricula[12][15] = T_FD;
    room.cuadricula[13][15] = T_FD;
    room.cuadricula[14][15] = T_PIL_L;
    for (int c = 15; c <= 18; c++) room.cuadricula[c][15] = T_WB;
    room.cuadricula[19][15] = T_WBR;

    // Rows 17-19 (r=16..18): corridor
    for (int r = 16; r <= 18; r++) {
        room.cuadricula[11][r] = T_WTL;
        room.cuadricula[12][r] = T_FD;
        room.cuadricula[13][r] = T_FD;
        room.cuadricula[14][r] = T_WTR;
    }

    // Row 20 (r=19): south door frame corners
    room.cuadricula[11][19] = T_WTL;
    room.cuadricula[14][19] = T_WTR;

}

static void construirHabitacion1(Habitacion &room) {
    inicializarHabitacionVacia(room);

    agregarPuerta(room, 0, 12, 1, true);
    room.cuadricula[12][0] = T_DLOCKED;
    room.cuadricula[13][0] = T_DLOCKED;
    agregarPuerta(room, MAX_FILAS-1, 12, 4, true);
    room.cuadricula[12][MAX_FILAS-1] = T_DLOCKED;
    room.cuadricula[13][MAX_FILAS-1] = T_DLOCKED;

    // Row 1 (r=0)
    room.cuadricula[11][0] = T_WTL;
    room.cuadricula[14][0] = T_WTR;

    // Rows 2-5 (r=1-4): corridor
    for (int r = 1; r <= 4; r++) {
        room.cuadricula[11][r] = T_WTL;
        room.cuadricula[12][r] = T_FLOOR3;
        room.cuadricula[13][r] = T_FLOOR3;
        room.cuadricula[14][r] = T_WTR;
    }

    // Row 6 (r=5)
    room.cuadricula[2][5] = T_WTL;
    for (int c = 3; c <= 9; c++) room.cuadricula[c][5] = T_WT;
    room.cuadricula[10][5] = T_WTR; room.cuadricula[11][5] = T_WTL;
    room.cuadricula[12][5] = T_FLOOR3; room.cuadricula[13][5] = T_FLOOR3;
    room.cuadricula[14][5] = T_WTR; room.cuadricula[15][5] = T_WTL;
    for (int c = 16; c <= 22; c++) room.cuadricula[c][5] = T_WT;
    room.cuadricula[23][5] = T_WTR;

    // Row 7 (r=6)
    room.cuadricula[2][6] = T_WTL; room.cuadricula[3][6] = T_FTL;
    for (int c = 4; c <= 8; c++) room.cuadricula[c][6] = T_FT;
    room.cuadricula[9][6]  = T_FTR; room.cuadricula[10][6] = T_WTR;
    room.cuadricula[11][6] = T_WTL; room.cuadricula[12][6] = T_FLOOR3; room.cuadricula[13][6] = T_FLOOR3;
    room.cuadricula[14][6] = T_WTR; room.cuadricula[15][6] = T_WTL; room.cuadricula[16][6] = T_FTL;
    for (int c = 17; c <= 21; c++) room.cuadricula[c][6] = T_FT;
    room.cuadricula[22][6] = T_FTR; room.cuadricula[23][6] = T_WTR;

    // Row 8 (r=7)
    room.cuadricula[2][7] = T_WTL; room.cuadricula[3][7] = T_FL;
    for (int c = 4; c <= 8; c++) room.cuadricula[c][7] = T_FLOOR3;
    room.cuadricula[9][7]  = T_FR;  room.cuadricula[10][7] = T_WT; room.cuadricula[11][7] = T_WT;
    room.cuadricula[12][7] = T_FLOOR3; room.cuadricula[13][7] = T_FLOOR3;
    room.cuadricula[14][7] = T_WT;  room.cuadricula[15][7] = T_WT; room.cuadricula[16][7] = T_FL;
    for (int c = 17; c <= 21; c++) room.cuadricula[c][7] = T_FLOOR3;
    room.cuadricula[22][7] = T_FR; room.cuadricula[23][7] = T_WTR;

    // Row 9 (r=8)
    room.cuadricula[2][8] = T_WTL; room.cuadricula[3][8] = T_FL;
    for (int c = 4; c <= 9; c++) room.cuadricula[c][8] = T_FLOOR3;
    room.cuadricula[10][8] = T_FT; room.cuadricula[11][8] = T_FT;
    room.cuadricula[12][8] = T_FLOOR3; room.cuadricula[13][8] = T_FLOOR3;
    room.cuadricula[14][8] = T_FT; room.cuadricula[15][8] = T_FT;
    for (int c = 16; c <= 21; c++) room.cuadricula[c][8] = T_FLOOR3;
    room.cuadricula[22][8] = T_FR; room.cuadricula[23][8] = T_WTR;

    // Rows 10-11 (r=9-10)
    for (int r = 9; r <= 10; r++) {
        room.cuadricula[2][r] = T_WTL; room.cuadricula[3][r] = T_FL;
        for (int c = 4; c <= 21; c++) room.cuadricula[c][r] = T_FLOOR3;
        room.cuadricula[22][r] = T_FR; room.cuadricula[23][r] = T_WTR;
    }
    // Test chest for barrel debug (col=6, row=9) — one tile right of player spawn
    room.cuadricula[6][9] = T_CHEST;

    // Row 12 (r=11): pillars
    room.cuadricula[2][11] = T_WTL; room.cuadricula[3][11] = T_FL;
    for (int c = 4; c <= 8; c++) room.cuadricula[c][11] = T_FLOOR3;
    room.cuadricula[9][11]  = T_FR;
    room.cuadricula[10][11] = T_PIL_L; room.cuadricula[11][11] = T_PIL_R;
    room.cuadricula[12][11] = T_FLOOR3; room.cuadricula[13][11] = T_FLOOR3;
    room.cuadricula[14][11] = T_PIL_L;  room.cuadricula[15][11] = T_PIL_R;
    for (int c = 16; c <= 21; c++) room.cuadricula[c][11] = T_FLOOR3;
    room.cuadricula[22][11] = T_FR; room.cuadricula[23][11] = T_WTR;

    // Row 13 (r=12): frame bottom rail with FBL/FBR corners
    room.cuadricula[2][12] = T_WTL; room.cuadricula[3][12] = T_FBL;
    for (int c = 4; c <= 8; c++) room.cuadricula[c][12] = T_FB;
    room.cuadricula[9][12]  = T_FBR;
    room.cuadricula[10][12] = T_WTR; room.cuadricula[11][12] = T_WTL;
    room.cuadricula[12][12] = T_FLOOR3; room.cuadricula[13][12] = T_FLOOR3;
    room.cuadricula[14][12] = T_WTR; room.cuadricula[15][12] = T_WTL;
    for (int c = 16; c <= 21; c++) room.cuadricula[c][12] = T_FB;
    room.cuadricula[22][12] = T_FBR; room.cuadricula[23][12] = T_WTR;

    // Row 14 (r=13): bottom walls
    room.cuadricula[2][13] = T_WBL;
    for (int c = 3; c <= 9; c++) room.cuadricula[c][13] = T_WB;
    room.cuadricula[10][13] = T_WBR; room.cuadricula[11][13] = T_WTL;
    room.cuadricula[12][13] = T_FLOOR3; room.cuadricula[13][13] = T_FLOOR3;
    room.cuadricula[14][13] = T_WTR; room.cuadricula[15][13] = T_WBL;
    for (int c = 16; c <= 22; c++) room.cuadricula[c][13] = T_WB;
    room.cuadricula[23][13] = T_WBR;

    // Rows 15-19 (r=14-18): corridor
    for (int r = 14; r <= 18; r++) {
        room.cuadricula[11][r] = T_WTL;
        room.cuadricula[12][r] = T_FLOOR3;
        room.cuadricula[13][r] = T_FLOOR3;
        room.cuadricula[14][r] = T_WTR;
    }

    // Row 20 (r=19): south door frame
    room.cuadricula[11][19] = T_WTL;
    room.cuadricula[14][19] = T_WTR;

    // Tile 56 decoration at (col=19, row=5)
    room.cuadricula[19][5] = T_SDOOR;
}

static void construirHabitacion2(Habitacion &room) {
    llenarHabitacionBasica(room);

    // Maze walls (user coords are 1-indexed (row,col); mapped to layout[col-1][row-1])
    // row 1
    for (int c = 0; c <= 11; c++) room.cuadricula[c][0] = T_W;
    for (int c = 14; c <= 25; c++) room.cuadricula[c][0] = T_W;
    // row 2
    room.cuadricula[0][1] = T_W; room.cuadricula[17][1] = T_W; room.cuadricula[25][1] = T_W;
    // row 3
    for (int c = 0; c <= 11; c++) room.cuadricula[c][2] = T_W;
    for (int c = 14; c <= 17; c++) room.cuadricula[c][2] = T_W;
    for (int c = 19; c <= 23; c++) room.cuadricula[c][2] = T_W;
    room.cuadricula[25][2] = T_W;
    // row 4
    room.cuadricula[0][3] = T_W;
    for (int c = 4; c <= 8; c++) room.cuadricula[c][3] = T_W;
    room.cuadricula[16][3] = T_W; room.cuadricula[17][3] = T_W;
    room.cuadricula[19][3] = T_W; room.cuadricula[23][3] = T_W; room.cuadricula[25][3] = T_W;
    // row 5
    room.cuadricula[0][4] = T_W; room.cuadricula[2][4] = T_W; room.cuadricula[4][4] = T_W;
    for (int c = 8; c <= 14; c++) room.cuadricula[c][4] = T_W;
    room.cuadricula[16][4] = T_W; room.cuadricula[17][4] = T_W; room.cuadricula[19][4] = T_W;
    room.cuadricula[21][4] = T_W; room.cuadricula[22][4] = T_W; room.cuadricula[23][4] = T_W;
    room.cuadricula[25][4] = T_W;
    // row 6
    room.cuadricula[0][5] = T_W; room.cuadricula[2][5] = T_W; room.cuadricula[6][5] = T_W;
    room.cuadricula[11][5] = T_W; room.cuadricula[19][5] = T_W; room.cuadricula[25][5] = T_W;
    // row 7
    room.cuadricula[0][6] = T_W;
    for (int c = 2; c <= 9; c++) room.cuadricula[c][6] = T_W;
    room.cuadricula[11][6] = T_W; room.cuadricula[13][6] = T_W;
    room.cuadricula[15][6] = T_W; room.cuadricula[16][6] = T_W; room.cuadricula[17][6] = T_W;
    room.cuadricula[19][6] = T_W;
    room.cuadricula[21][6] = T_W; room.cuadricula[22][6] = T_W; room.cuadricula[23][6] = T_W;
    room.cuadricula[25][6] = T_W;
    // row 8
    room.cuadricula[0][7] = T_W; room.cuadricula[2][7] = T_W; room.cuadricula[4][7] = T_W;
    room.cuadricula[11][7] = T_W; room.cuadricula[13][7] = T_W; room.cuadricula[17][7] = T_W;
    room.cuadricula[19][7] = T_W; room.cuadricula[21][7] = T_W; room.cuadricula[25][7] = T_W;
    // row 9
    room.cuadricula[0][8] = T_W; room.cuadricula[2][8] = T_W; room.cuadricula[4][8] = T_W;
    room.cuadricula[6][8] = T_W;
    for (int c = 8; c <= 15; c++) room.cuadricula[c][8] = T_W;
    room.cuadricula[17][8] = T_W; room.cuadricula[19][8] = T_W; room.cuadricula[21][8] = T_W;
    room.cuadricula[23][8] = T_W; room.cuadricula[24][8] = T_W; room.cuadricula[25][8] = T_W;
    // row 10
    room.cuadricula[0][9] = T_W; room.cuadricula[2][9] = T_W; room.cuadricula[6][9] = T_W;
    room.cuadricula[15][9] = T_W; room.cuadricula[17][9] = T_W; room.cuadricula[19][9] = T_W;
    room.cuadricula[21][9] = T_W; room.cuadricula[25][9] = T_W;
    // row 11
    room.cuadricula[0][10] = T_W;
    for (int c = 2; c <= 13; c++) room.cuadricula[c][10] = T_W;
    room.cuadricula[15][10] = T_W; room.cuadricula[17][10] = T_W; room.cuadricula[19][10] = T_W;
    room.cuadricula[21][10] = T_W; room.cuadricula[22][10] = T_W; room.cuadricula[23][10] = T_W;
    room.cuadricula[25][10] = T_W;
    // row 12
    room.cuadricula[0][11] = T_W; room.cuadricula[4][11] = T_W; room.cuadricula[11][11] = T_W;
    room.cuadricula[13][11] = T_W; room.cuadricula[15][11] = T_W;
    room.cuadricula[17][11] = T_W; room.cuadricula[18][11] = T_W; room.cuadricula[19][11] = T_W;
    room.cuadricula[23][11] = T_W; room.cuadricula[25][11] = T_W;
    // row 13
    room.cuadricula[0][12] = T_W; room.cuadricula[1][12] = T_W; room.cuadricula[2][12] = T_W;
    room.cuadricula[4][12] = T_W;
    for (int c = 6; c <= 9; c++) room.cuadricula[c][12] = T_W;
    room.cuadricula[11][12] = T_W; room.cuadricula[19][12] = T_W; room.cuadricula[21][12] = T_W;
    room.cuadricula[23][12] = T_W; room.cuadricula[24][12] = T_W; room.cuadricula[25][12] = T_W;
    // row 14
    room.cuadricula[0][13] = T_W; room.cuadricula[2][13] = T_W; room.cuadricula[9][13] = T_W;
    room.cuadricula[11][13] = T_W; room.cuadricula[12][13] = T_W;
    for (int c = 14; c <= 19; c++) room.cuadricula[c][13] = T_W;
    room.cuadricula[21][13] = T_W; room.cuadricula[25][13] = T_W;
    // row 15
    room.cuadricula[0][14] = T_W;
    for (int c = 2; c <= 7; c++) room.cuadricula[c][14] = T_W;
    room.cuadricula[9][14] = T_W;
    room.cuadricula[12][14] = T_W; room.cuadricula[14][14] = T_W;
    for (int c = 19; c <= 23; c++) room.cuadricula[c][14] = T_W;
    room.cuadricula[25][14] = T_W;
    // row 16
    room.cuadricula[0][15] = T_W; room.cuadricula[9][15] = T_W;
    for (int c = 12; c <= 16; c++) room.cuadricula[c][15] = T_W;
    room.cuadricula[23][15] = T_W; room.cuadricula[25][15] = T_W;
    // row 17
    room.cuadricula[0][16] = T_W;
    for (int c = 2; c <= 9; c++) room.cuadricula[c][16] = T_W;
    room.cuadricula[16][16] = T_W;
    room.cuadricula[18][16] = T_W; room.cuadricula[19][16] = T_W;
    room.cuadricula[21][16] = T_W; room.cuadricula[22][16] = T_W; room.cuadricula[23][16] = T_W;
    room.cuadricula[25][16] = T_W;
    // row 18
    room.cuadricula[0][17] = T_W;
    for (int c = 2; c <= 10; c++) room.cuadricula[c][17] = T_W;
    room.cuadricula[16][17] = T_W; room.cuadricula[19][17] = T_W;
    room.cuadricula[21][17] = T_W; room.cuadricula[22][17] = T_W;
    room.cuadricula[25][17] = T_W;
    // row 19
    room.cuadricula[0][18] = T_W; room.cuadricula[10][18] = T_W;
    room.cuadricula[16][18] = T_W; room.cuadricula[17][18] = T_W;
    room.cuadricula[19][18] = T_W; room.cuadricula[25][18] = T_W;
    // row 20
    for (int c = 0; c <= 11; c++) room.cuadricula[c][19] = T_W;
    for (int c = 14; c <= 25; c++) room.cuadricula[c][19] = T_W;

    agregarPuerta(room, 0, MAX_COLUMNAS/2 - 1, 4, false);
    agregarPuerta(room, MAX_FILAS - 1, MAX_COLUMNAS/2 - 1, 5, false);
    room.cuadricula[10][15] = T_LDOOR;
    room.cuadricula[11][15] = T_LDOOR;

    // Two chests in the open area just inside the north entrance
    room.cuadricula[14][18] = T_CHEST;
    room.cuadricula[15][18] = T_CHEST;
}

static void construirHabitacion3(Habitacion &room) {
    llenarHabitacionBasica(room);
    llenarSueloOscuro(room);
    agregarMarcoDecorativo(room);
}

static void construirHabitacion4(Habitacion &room) {
    llenarHabitacionBasica(room);
    llenarSueloOscuro(room);
    agregarPuerta(room, 0, MAX_COLUMNAS/2-1, 0, false);
    room.cuadricula[MAX_COLUMNAS/2-2][0]          = T_WT;
    room.cuadricula[MAX_COLUMNAS/2+1][0]          = T_WT;
    agregarPuerta(room, MAX_FILAS-1, MAX_COLUMNAS/2-1, 2, false);
    room.cuadricula[MAX_COLUMNAS/2-2][MAX_FILAS-1] = T_WB;
    room.cuadricula[MAX_COLUMNAS/2+1][MAX_FILAS-1] = T_WB;
    agregarMarcoDecorativo(room);
}

static void construirHabitacion5(Habitacion &room) {
    llenarHabitacionBasica(room);
    llenarSueloOscuro(room);
    agregarPuerta(room, 0, MAX_COLUMNAS/2-1, 2, false);
    // Single cup holder at center-bottom
    room.cuadricula[13][17] = T_HOLDER;
}

void inicializarHabitaciones(Habitacion rooms[], int &roomCount) {
    inicializarPaletaTiles();
    roomCount = MAX_HABITACIONES;
    construirHabitacion1(rooms[0]);
    construirHabitacion0(rooms[1]);
    construirHabitacion2(rooms[2]);
    construirHabitacion3(rooms[3]);
    construirHabitacion4(rooms[4]);
    construirHabitacion5(rooms[5]);
}

void dibujarHabitacion(const Habitacion &room, sf::RenderWindow &window,
              const Jugador &player, int roomIndex) {
    const float TSCALE = (float)TAM_TILE / 16.0f;

    sf::Sprite tileSprite(g_tileset);
    tileSprite.setScale(sf::Vector2f(TSCALE, TSCALE));

    sf::Sprite gateSprite(g_gateTex);
    gateSprite.setScale(sf::Vector2f(TSCALE, TSCALE));

    sf::RectangleShape solidRect(sf::Vector2f((float)TAM_TILE, (float)TAM_TILE));

    for (int r = 0; r < MAX_FILAS; r++) {
        for (int c = 0; c < MAX_COLUMNAS; c++) {
            float          px   = (float)(c * TAM_TILE);
            float          py   = (float)(r * TAM_TILE);
            int            id   = room.cuadricula[c][r];
            const TileDef &tile = g_tiles[id];

            if (tile.isDoor && g_gateLoaded) {
                bool isFirst =
                    (r+1 < MAX_FILAS && (room.cuadricula[c][r+1] == T_D || room.cuadricula[c][r+1] == T_LDOOR || room.cuadricula[c][r+1] == T_DLOCKED)) ||
                    (c+1 < MAX_COLUMNAS && (room.cuadricula[c+1][r] == T_D || room.cuadricula[c+1][r] == T_LDOOR || room.cuadricula[c+1][r] == T_DLOCKED));

                gateSprite.setTextureRect(isFirst ? R_GATE_A : R_GATE_B);

                bool isVertical = (c == 0 || c == MAX_COLUMNAS-1);
                if (isVertical) {
                    gateSprite.setOrigin(sf::Vector2f(8.f, 8.f));
                    gateSprite.setRotation(sf::degrees(90.f));
                    gateSprite.setPosition(sf::Vector2f(px + TAM_TILE * 0.5f,
                                                        py + TAM_TILE * 0.5f));
                } else {
                    gateSprite.setOrigin(sf::Vector2f(0.f, 0.f));
                    gateSprite.setRotation(sf::degrees(0.f));
                    gateSprite.setPosition(sf::Vector2f(px, py));
                }
                window.draw(gateSprite);

            } else if (tile.useGateTex && g_gateLoaded) {
                solidRect.setFillColor(g_tiles[T_FD].color);
                solidRect.setPosition(sf::Vector2f(px, py));
                window.draw(solidRect);
                gateSprite.setTextureRect(tile.rect);
                gateSprite.setOrigin(sf::Vector2f(0.f, 0.f));
                gateSprite.setRotation(sf::degrees(0.f));
                gateSprite.setPosition(sf::Vector2f(px, py));
                window.draw(gateSprite);

            } else if (tile.useColor) {
                solidRect.setFillColor(tile.color);
                solidRect.setPosition(sf::Vector2f(px, py));
                window.draw(solidRect);

            } else if (g_tileLoaded) {
                tileSprite.setTextureRect(tile.rect);
                tileSprite.setPosition(sf::Vector2f(px, py));
                window.draw(tileSprite);

            } else {
                solidRect.setFillColor(
                    tile.solid  ? sf::Color( 64,  64,  64) :
                    tile.isDoor ? sf::Color(139,  90,  43) :
                                  sf::Color(200, 180, 140));
                solidRect.setPosition(sf::Vector2f(px, py));
                window.draw(solidRect);
            }

            if (roomIndex == 2) {
                int dr = r - player.fila;
                int dc = c - player.columna;
                if (dr < 0) dr = -dr;
                if (dc < 0) dc = -dc;
                if (dr > 1 || dc > 1) {
                    solidRect.setFillColor(sf::Color(80, 80, 80));
                    solidRect.setPosition(sf::Vector2f(px, py));
                    window.draw(solidRect);
                }
            }
        }
    }
}

bool esPared(const Habitacion &room, int fila, int columna) {
    if (fila < 0 || fila >= MAX_FILAS || columna < 0 || columna >= MAX_COLUMNAS) return true;
    return g_tiles[room.cuadricula[columna][fila]].solid;
}

const Puerta *obtenerPuertaEn(const Habitacion &room, int fila, int columna) {
    for (int i = 0; i < room.numPuertas; i++) {
        if (room.puertas[i].fila == fila && room.puertas[i].columna == columna) {
            return &room.puertas[i];
        }
    }
    return nullptr;
}
