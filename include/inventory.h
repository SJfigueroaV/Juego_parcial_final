#ifndef INVENTORY_H
#define INVENTORY_H

#include <SFML/Graphics.hpp>

const int MAX_SLOTS = 5;

enum TipoObjeto {
    NINGUNO,
    LLAVE
};

struct Objeto {
    TipoObjeto tipo;
    char       nombre[32];
    int        fila;
    int        columna;
    int        habitacion;
    bool       enSuelo;
};

struct Inventario {
    Objeto espacios[MAX_SLOTS];
    int    espacioSeleccionado;
    int    numObjetos;
};

void inicializarInventario(Inventario &inv);
bool agregarObjeto(Inventario &inv, TipoObjeto tipo);
void quitarObjeto(Inventario &inv, int slot);
void seleccionarSlot(Inventario &inv, int slot);

#endif
