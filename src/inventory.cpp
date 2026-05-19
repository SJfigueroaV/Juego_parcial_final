#include "../include/inventory.h"
#include <cstring>
#include <cstdio>

static void inicializarObjetoSegunTipo(Objeto &item, TipoObjeto type) {
    item.tipo          = type;
    item.enSuelo     = false;
    item.active        = false;
    item.effectTimer   = 0.0f;
    item.cooldownTimer = 0.0f;
    item.fila           = 0;
    item.columna           = 0;
    item.habitacion     = 0;

    switch (type) {
    case TRAP:
        std::strncpy(item.nombre, "Trampa",    sizeof(item.nombre));
        item.placeable = true;
        item.singleUse = false;
        break;
    case SPEEDBOOST:
        std::strncpy(item.nombre, "Velocidad", sizeof(item.nombre));
        item.placeable = false;
        item.singleUse = false;
        break;
    case BARREL:
        std::strncpy(item.nombre, "Barril",    sizeof(item.nombre));
        item.placeable = true;
        item.singleUse = false;
        break;
    case LLAVE:
        std::strncpy(item.nombre, "Llave",     sizeof(item.nombre));
        item.placeable = false;
        item.singleUse = true;
        break;
    case COPA:
        std::strncpy(item.nombre, "Copa",      sizeof(item.nombre));
        item.placeable = false;
        item.singleUse = true;
        break;
    default:
        std::strncpy(item.nombre, "",          sizeof(item.nombre));
        item.placeable = false;
        item.singleUse = false;
        break;
    }
}

void inicializarInventario(Inventario &inv) {
    for (int i = 0; i < MAX_SLOTS; i++) {
        inicializarObjetoSegunTipo(inv.espacios[i], NINGUNO);
    }
    inv.espacioSeleccionado = 0;
    inv.numObjetos    = 0;
}

bool agregarObjeto(Inventario &inv, TipoObjeto type) {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (inv.espacios[i].tipo == NINGUNO) {
            inicializarObjetoSegunTipo(inv.espacios[i], type);
            inv.numObjetos++;
            return true;
        }
    }
    return false;
}

void agregarObjetoASlot(Inventario &inv, int slot, TipoObjeto type) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    bool wasEmpty = (inv.espacios[slot].tipo == NINGUNO);
    inicializarObjetoSegunTipo(inv.espacios[slot], type);
    if (wasEmpty && type != NINGUNO) inv.numObjetos++;
}

void quitarObjeto(Inventario &inv, int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) {
        return;
    }
    if (inv.espacios[slot].tipo != NINGUNO) {
        inicializarObjetoSegunTipo(inv.espacios[slot], NINGUNO);
        inv.numObjetos--;
        if (inv.numObjetos < 0) {
            inv.numObjetos = 0;
        }
    }
}

void seleccionarSlot(Inventario &inv, int slot) {
    if (slot >= 0 && slot < MAX_SLOTS) {
        inv.espacioSeleccionado = slot;
    }
}

void colocarObjeto(Inventario &inv, int slot, int fila, int columna) {
    if (slot < 0 || slot >= MAX_SLOTS) {
        return;
    }
    if (inv.espacios[slot].tipo == NINGUNO || !inv.espacios[slot].placeable) {
        return;
    }
    inv.espacios[slot].enSuelo = true;
    inv.espacios[slot].fila    = fila;
    inv.espacios[slot].columna = columna;
}

void dibujarInventario(const Inventario &inv, sf::RenderWindow &window, const sf::Font &font) {
    const int   DISPLAY_SLOTS = 3;
    const float SLOT_W        = 80.0f;
    const float SLOT_H        = 65.0f;
    const float SLOT_PAD      = 10.0f;
    const float PANEL_X       = 832.0f;
    const float PANEL_W       = 100.0f;
    const float WINDOW_H_F    = 640.0f;

    const float TOTAL_H = (float)DISPLAY_SLOTS * SLOT_H + (float)(DISPLAY_SLOTS - 1) * SLOT_PAD;
    const float START_X = PANEL_X + (PANEL_W - SLOT_W) / 2.0f;
    const float START_Y = (WINDOW_H_F - TOTAL_H) / 2.0f;

    sf::RectangleShape panel(sf::Vector2f{PANEL_W, WINDOW_H_F});
    panel.setPosition(sf::Vector2f{PANEL_X, 0.0f});
    panel.setFillColor(sf::Color(15, 15, 15));
    window.draw(panel);

    for (int i = 0; i < DISPLAY_SLOTS; i++) {
        float y = START_Y + (float)i * (SLOT_H + SLOT_PAD);

        sf::RectangleShape bg(sf::Vector2f{SLOT_W, SLOT_H});
        bg.setPosition(sf::Vector2f{START_X, y});
        bg.setFillColor(sf::Color(30, 30, 30, 220));

        const Objeto &slot = inv.espacios[i];
        bool onFloor  = slot.enSuelo && slot.tipo != NINGUNO;
        bool onCooldown = (slot.tipo == SPEEDBOOST) && (slot.cooldownTimer > 0.0f) && !onFloor;

        bool showItem = (slot.tipo != NINGUNO) && !onFloor;

        if (i == inv.espacioSeleccionado) {
            bg.setOutlineColor(sf::Color::Yellow);
            bg.setOutlineThickness(2.0f);
        } else {
            bg.setOutlineColor(sf::Color(120, 120, 120));
            bg.setOutlineThickness(1.0f);
        }
        window.draw(bg);

        char numBuf[4];
        std::snprintf(numBuf, sizeof(numBuf), "%d", i + 1);
        sf::Text numText(font, numBuf, 10);
        numText.setFillColor(sf::Color(160, 160, 160));
        numText.setPosition(sf::Vector2f{START_X + 3.0f, y + 2.0f});
        window.draw(numText);

        if (showItem) {
            sf::Text label(font, slot.nombre, 11);
            label.setFillColor(onCooldown ? sf::Color(140, 140, 140) : sf::Color::White);
            sf::FloatRect lb = label.getLocalBounds();
            label.setPosition(sf::Vector2f{
                START_X + (SLOT_W - lb.size.x) / 2.0f,
                y + (SLOT_H - lb.size.y) / 2.0f - 6.0f
            });
            window.draw(label);

            if (onCooldown) {
                char cdBuf[16];
                std::snprintf(cdBuf, sizeof(cdBuf), "CD: %.1fs", slot.cooldownTimer);
                sf::Text cdText(font, cdBuf, 9);
                cdText.setFillColor(sf::Color(255, 160, 40));
                sf::FloatRect cb = cdText.getLocalBounds();
                cdText.setPosition(sf::Vector2f{
                    START_X + (SLOT_W - cb.size.x) / 2.0f,
                    y + (SLOT_H - lb.size.y) / 2.0f + 8.0f
                });
                window.draw(cdText);
            }
        }
    }
}
