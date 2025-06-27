#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL/SDL.h>
#include "ale_interface.hpp"
#include "common/Constants.h"

#include "../include/neural_network.h"
#include "../include/activation_functions/step.h"
#include "../include/weights_initialization/kaiming.h"

constexpr unsigned s_kinputs = 5;
constexpr unsigned s_numActions = 5;

const unsigned vEstadoJuego[s_kinputs] = {1, 2, 3, 4, 5};

NeuralNetwork crearRedDesdePesos() {
    std::shared_ptr<ActivationFunction> activation = std::make_shared<StepFunction>();
    auto initializer = std::make_unique<Kaiming>();

    std::vector<std::vector<std::vector<float>>> pesos_por_capa = {
        {
            {0.1, 0.2, 0.3, 0.4, 0.5, 0.6},
            {-0.2, 0.1, -0.1, 0.4, -0.5, 0.2},
            {0.05, 0.3, 0.2, -0.1, 0.6, 0.1}
        },
        {
            {0.2, 0.1, 0.3, -0.2},
            {0.0, -0.3, 0.2, 0.4f}
        },
        {
            {0.1, 0.5, -0.4},
            {-0.2, 0.3, 0.1},
            {0.0, -0.5, 0.2},
            {0.15, 0.1, 0.1},
            {-0.1, -0.2, 0.6}
        }
    };

    std::vector<Layer> capas;

    for (size_t i = 0; i < pesos_por_capa.size(); ++i) {
        size_t input_size = pesos_por_capa[i][0].size() - 1;
        size_t output_size = pesos_por_capa[i].size();

        Layer capa(input_size, output_size, activation->clone(), initializer->clone());
        for (size_t j = 0; j < output_size; ++j)
            capa.getNeurons()[j].getWeights() = pesos_por_capa[i][j];

        capas.push_back(std::move(capa));
    }

    return NeuralNetwork(capas);
}

Action Inteligencia(const std::vector<float>& estado, NeuralNetwork& nn) {
    std::vector<float> salida = nn.feedForward(estado);

    std::vector<int> activaciones(s_numActions);
    for (unsigned i = 0; i < s_numActions; ++i)
        activaciones[i] = (salida[i] >= 0.5f) ? 1 : 0;

    if (activaciones[3] || (activaciones[1] && activaciones[2])) return PLAYER_A_RIGHTFIRE;
    if (activaciones[4] || (activaciones[0] && activaciones[2])) return PLAYER_A_LEFTFIRE;
    if (activaciones[0]) return PLAYER_A_LEFT;
    if (activaciones[1]) return PLAYER_A_RIGHT;
    if (activaciones[2]) return PLAYER_A_FIRE;

    return PLAYER_A_NOOP;
}

reward_t agentStep(ALEInterface& alei, NeuralNetwork& nn) {
    static int32_t lives = alei.lives();
    reward_t reward = 0;

    if (alei.lives() < lives) {
        lives = alei.lives();
        alei.act(PLAYER_A_FIRE);
    }

    const ALERAM& ram = alei.getRAM();
    std::vector<float> estado;
    estado.reserve(s_kinputs);
    for (unsigned i = 0; i < s_kinputs; ++i) {
        estado.push_back(static_cast<float>(ram.get(vEstadoJuego[i])));
    }

    Action act = Inteligencia(estado, nn);
    reward = alei.act(act);

    return reward;
}

void usage(const char* pname) {
    std::cerr << "USAGE:\n  " << pname << " <romfile>\n";
    std::exit(1);
}

int main(int argc, char** argv) {
    if (argc != 2) usage(argv[0]);

    ALEInterface alei;
    alei.setInt("random_seed", 0);
    alei.setFloat("repeat_action_probability", 0);
    alei.setBool("display_screen", true);
    alei.setBool("sound", true);
    alei.loadROM(argv[1]);

    NeuralNetwork nn = crearRedDesdePesos();
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    alei.act(PLAYER_A_FIRE);

    reward_t totalReward = 0;
    while (!alei.game_over()) {
        totalReward += agentStep(alei, nn);
    }

    std::cout << "Total Reward: " << totalReward << "\n";
    return 0;
}
