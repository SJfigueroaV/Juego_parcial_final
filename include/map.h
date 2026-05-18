#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <string>

struct Jugador;

const int MAX_FILAS         = 20;
const int MAX_COLUMNAS      = 26;
const int TAM_TILE          = 32;
const int MAX_HABITACIONES  = 6;
const int VISION_RADIUS     = 4;

const int T_W  = 0;
const int T_F  = 1;
const int T_D  = 2;
const int T_FD = 3;
const int T_WB  = 4;
const int T_WT  = 5;
const int T_WL  = 6;
const int T_WR  = 7;
const int T_WTL = 8;
const int T_WTR = 9;
const int T_WBL = 18;
const int T_WBR = 19;
const int T_WIL   = 20;
const int T_WIR   = 21;
const int T_LDOOR  = 22;
const int T_DLOCKED = 23;
const int T_PIL_L   = 24;
const int T_PIL_R   = 25;
const int T_FLOOR2  = 26;
const int T_FLOOR3  = 27;
const int T_CHEST   = 28;
const int T_SDOOR   = 29;
const int T_HOLDER  = 30;

const int T_FTL = 10;
const int T_FT  = 11;
const int T_FTR = 12;
const int T_FL  = 13;
const int T_FR  = 14;
const int T_FBL = 15;
const int T_FB  = 16;
const int T_FBR = 17;

const int TILE_PALETTE_SIZE = 32;

struct TileDef {
    bool        solid;
    bool        isDoor;
    bool        useColor;
    bool        useGateTex;
    sf::Color   color;
    sf::IntRect rect;
};

extern TileDef g_tiles[TILE_PALETTE_SIZE];

struct Puerta {
    int  aHabitacion;
    int  fila, columna;
    bool bloqueada;
};

struct Habitacion {
    int  cuadricula[MAX_COLUMNAS][MAX_FILAS];
    Puerta puertas[8];
    int  numPuertas;
};

void               inicializarHabitaciones(Habitacion rooms[], int &roomCount);
void               cargarTileset(const std::string &path);
void               cargarTexturaPuerta(const std::string &path);
void               cargarTexturaLlave(const std::string &path);
const sf::Texture *obtenerTileset();
const sf::Texture *obtenerTexturaPuerta();
const sf::Texture *obtenerTexturaLlave();
void               dibujarHabitacion(const Habitacion &room, sf::RenderWindow &window,
                            const Jugador &player, int roomIndex);
bool               esPared(const Habitacion &room, int fila, int columna);
const Puerta        *obtenerPuertaEn(const Habitacion &room, int fila, int columna);

#endif
