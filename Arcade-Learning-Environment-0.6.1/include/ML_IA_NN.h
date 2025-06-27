#define MACHINE_LEARNING

#include "activation_functions/activation_function.h"
#include "../src/common/Constants.h"
#include "../src/ale_interface.hpp"


const int s_N_EstadosJuego = 15; //Number of features
const unsigned vEstadoJuego[s_N_EstadosJuego] = {16, 17, 18, 19, 21, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46};