#define MACHINE_LEARNING

#include "activation_functions/activation_function.h"
#include "../src/common/Constants.h"
#include "../src/ale_interface.hpp"


const int s_N_EstadosJuego = 5; //Number of features
double vEstadoJuego[s_N_EstadosJuego] = {1, 2, 3, 4, 5}; //Vector of Ram indexes