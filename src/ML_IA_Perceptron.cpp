#include "ML_IA_Perceptron.h"
#include "../include/activation_functions/step.h"

const unsigned s_kinputs = s_N_EstadosJuego-1; // Perceptron inputs
const unsigned s_numActions = 5; //0 Mov izq, 1 Mov der, 2 Disparar, 3 mov der y disparar, 4 mov izq y disparar
double s_w[s_numActions][s_kinputs];

std::vector<float> h(unsigned p_id, const double* st) {
    // p_id: Perceptron ID (0 a 4)
    // st: Game state vector
    float hval = 0;

    for(unsigned i=1; i < s_kinputs; i++)
      hval += st[i-1] * s_w[p_id][i];

    hval += s_w[p_id][0];
    std::vector<float>tmp = {hval};

    return tmp;
}

Accion
Inteligencia (int jugador) {
    Accion accion_resultado;
    std::unique_ptr<ActivationFunction> afunc = std::make_unique<StepFunction>();
    double accionConfianza[s_numActions];
    for(unsigned i = 0; i < s_numActions; i++) {
        std::vector<float> tmp = h(jugador, vEstadoJuego);
        afunc->activate(tmp);
        accionConfianza[i] = tmp[0];
    }

    if (accionConfianza[3] == 1.0) {
        accion_resultado.mover_der = true;
        accion_resultado.disparar = true;
    }

    if (accionConfianza[4] == 1.0) {
        accion_resultado.mover_izq = true;
        accion_resultado.disparar = true;
    }

    if (accionConfianza[0] == 1.0) accion_resultado.mover_izq = true;
    if (accionConfianza[1] == 1.0) accion_resultado.mover_der = true;
    if (accionConfianza[2] == 1.0) accion_resultado.disparar = true;

    return accion_resultado;
}