#include "../include/inventory.h"
#include <cstring>

static void inicializarObjetoSegunTipo(Objeto &obj, TipoObjeto tipo) {
    obj.tipo          = tipo;
    obj.enSuelo       = false;
    obj.active        = false;
    obj.effectTimer   = 0.0f;
    obj.cooldownTimer = 0.0f;
    obj.fila          = 0;
    obj.columna       = 0;
    obj.habitacion    = 0;

    switch (tipo) {
    case TRAP:
        std::strncpy(obj.nombre, "Trampa", sizeof(obj.nombre));
        obj.placeable = true;
        obj.singleUse = false;
        break;
    case SPEEDBOOST:
        std::strncpy(obj.nombre, "Velocidad", sizeof(obj.nombre));
        obj.placeable = false;
        obj.singleUse = false;
        break;
    case BARREL:
        std::strncpy(obj.nombre, "Barril", sizeof(obj.nombre));
        obj.placeable = true;
        obj.singleUse = false;
        break;
    case LLAVE:
        std::strncpy(obj.nombre, "Llave", sizeof(obj.nombre));
        obj.placeable = false;
        obj.singleUse = true;
        break;
    default:
        std::strncpy(obj.nombre, "", sizeof(obj.nombre));
        obj.placeable = false;
        obj.singleUse = false;
        break;
    }
}

void inicializarInventario(Inventario &inv) {
    for (int i = 0; i < MAX_SLOTS; i++) {
        inicializarObjetoSegunTipo(inv.espacios[i], NINGUNO);
    }
    inv.espacioSeleccionado = 0;
    inv.numObjetos          = 0;
}

bool agregarObjeto(Inventario &inv, TipoObjeto tipo) {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (inv.espacios[i].tipo == NINGUNO) {
            inicializarObjetoSegunTipo(inv.espacios[i], tipo);
            inv.numObjetos++;
            return true;
        }
    }
    return false;
}

void quitarObjeto(Inventario &inv, int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    if (inv.espacios[slot].tipo != NINGUNO) {
        inicializarObjetoSegunTipo(inv.espacios[slot], NINGUNO);
        inv.numObjetos--;
        if (inv.numObjetos < 0) inv.numObjetos = 0;
    }
}

void seleccionarSlot(Inventario &inv, int slot) {
    if (slot >= 0 && slot < MAX_SLOTS) {
        inv.espacioSeleccionado = slot;
    }
}

void colocarObjeto(Inventario &inv, int slot, int fila, int columna) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    if (inv.espacios[slot].tipo == NINGUNO || !inv.espacios[slot].placeable) return;
    inv.espacios[slot].enSuelo = true;
    inv.espacios[slot].fila    = fila;
    inv.espacios[slot].columna = columna;
}
