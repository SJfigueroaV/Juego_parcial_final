#include "../include/inventory.h"
#include <cstring>

static void inicializarObjetoSegunTipo(Objeto &obj, TipoObjeto tipo) {
    obj.tipo       = tipo;
    obj.enSuelo    = false;
    obj.fila       = 0;
    obj.columna    = 0;
    obj.habitacion = 0;
    if (tipo == LLAVE) {
        std::strncpy(obj.nombre, "Llave", sizeof(obj.nombre));
    } else {
        std::strncpy(obj.nombre, "", sizeof(obj.nombre));
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
