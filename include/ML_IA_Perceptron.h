#define MACHINE_LEARNING

#include "activation_functions/activation_function.h"

struct Accion {
    bool mover_izq = false;
    bool mover_der = false;
    bool disparar  = false;
};


typedef enum { // Game State Features
    // Example: s_enemigoYRel
   s_N_EstadosJuego     // Total number of game states
} SEstadoJuego;

extern double vEstadoJuego[s_N_EstadosJuego];