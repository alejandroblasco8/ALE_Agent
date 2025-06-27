#include "../include/ML_IA_Perceptron.h"
#include "../include/activation_functions/step.h"

const unsigned s_kinputs = s_N_EstadosJuego + 1; // Perceptron inputs
const unsigned s_numActions = 3; //0 Mov izq, 1 Mov der, 2 Disparar, 3 mov der y disparar, 4 mov izq y disparar
double s_w[s_numActions][s_kinputs] = { //Example weights
    {0, -0.1, 0, 0, -2.5, 0, 3.2, 0, 9.2, 21.9, 21, 12.8, -12.4, -25.6, 29.6, -4.6,
  23, 14.5, 15.3, 36.4, 6.7, -0.900002, -22.1, 8.2, 25.4, 0.2, 0, -1.3, 0.2, -6.4,
  0.8, 0.8, 1.2, 0, 0.8, -1.8, -1.4, -2.6, 1, 15.2, 4, 0, 0, 0, 0, -6.8, -12.8, -3.4,
  17.7, -11.4, -2.5, -9.9, -2.6, -0.3, 0.1, 0, -16.8, -19.2, -0.0999999, -6.4, -2.2,
  0, 3.3, -0.1, 0, 0.8, 0, -11.2, 0, -5, -4, -5.1, -5.1, -7.4, -20.8, -8, -17.6, 0,
  -0.799999, 0, 0, 3.9, 0, 0, 0, 0, 8, -1.4, 1.4, 0.2, -3.6, 0.5, -7.6, -24, 0, 0, 0,
  0, 18, 0, 7, 0, -8, 0, 0, 0, 0, 0, 12.8, 0, 0, 0, -15.2, -0.1, 0, -0.3, 0, 0, 0, 0,
  0, 0, 0, -6.5, -2.9, 0, 0, 0, 0 },
    {0, 0.2, 0, 0, 0.8, 0, -2.1, 0, 17.6, 41.2, 35.4, -9.53674e-07, 3.2, 1.6, 26.8, 30.2,
  5.1, 27.6, 12.5, 15.9, 18.8, -19.1, 38.7, 14.7, 50.7, -0.4, 0, 0.2, 0.7, 25.6,
  -0.3, -0.7, -0.3, 0, -0.3, -0.7, -0.3, 2.7, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.0999991,
  -15.2, -3.6, 18.3, 2.7, -0.2, -0.1, 0, 0, 0, 1.5, 0, -2, 0, -24, 0.2, 0, 19.2, 0,
  -2.4, 0, -4.9, -3.7, -5.6, -4.9, 1.4, -1.6, -17.6, -11.2, 0, 1.6, 0, 0, 7.5, 0, 0,
  0, 0, -9, 2, -2, 0.4, 12.6, -2.98023e-08, 5, 17.6, 0, 0, 0, 0, 0, 0, -2, 0, -1, 0,
  0, 0, 0, 0, -16, 0.4, 0, 0.1, 2.4, 0.2, 0, -0.1, 0, 0, 0, 0, 0, 0, 0, 6.5, 2.9, 0,
  0, 0, 0},
    {0, 0.8, 0, 0, 2.3, 0, -14.9, 0, 23.8, 17.1, 0.0999985, -3.2, 6.4, -12.8, -13.6,
  13.7, 15.5, 12.4, 12.5, -3.3, -3.8, 39, 2.1, -25.8, 25.4, -1.6, 0, -1.2, 0.7, 44.8,
  4, 2.6, 3.7, 0, 4, 2.6, 3.7, 2.6, -2.7, -12.9, 10.3, -5, -0.8, 0, 0, 0, 0, 0, 38.7,
  -2.8, 0.800001, 11.4, 2.6, 0.4, -2.6, 24, 24.8, 23.2, 0.8, 6.4, -0.8, 0, -13.4,
  0.8, 0, 42.4, 0, 29.6, 0, 1.4, -4.8, -2.8, 1.5, 4.4, 30.4, 22.4, 9.6, 0, 6.4, 0,
  -50, 25.7, 0, 0, 0, 0, 23.5, 14, -14, 1.7, 12.8, -1.8, 12.8, 24, 0, 0, 0, 0, -9, 0,
  -23, 0, -19, 0, 0, 0, 0, 0, -12.8, 0.4, 0, 0.3, 24.3, 0.8, 0, -0.4, 0, 0, 0, 0, 0,
  0, 0, 6.5, 2.9, 0, 0, 2.6, 0}
};

std::vector<float> h(unsigned p_id, std::vector<float>& st) {
    // p_id: Perceptron ID (0 a 2)
    // st: Game state vector
    float hval = 0;

    for(unsigned i=1; i < s_kinputs; i++)
      hval += st[i-1] * s_w[p_id][i];

    hval += s_w[p_id][0];
    std::vector<float>tmp = {hval};

    return tmp;
}

Action
Inteligencia (std::vector<float>& estado) {
    std::unique_ptr<ActivationFunction> afunc = std::make_unique<StepFunction>();
    double accionConfianza[s_numActions];
    for(unsigned i = 0; i < s_numActions; i++) {
        std::vector<float> tmp = h(i, estado);
        afunc->activate(tmp);
        accionConfianza[i] = tmp[0];
    }

    if(s_numActions > 3){

        if (accionConfianza[3] == 1.0 || (accionConfianza[1] == 1.0 && accionConfianza[2] == 1.0)) {
            return PLAYER_A_RIGHTFIRE;
        }

        if (accionConfianza[4] == 1.0 || (accionConfianza[0] == 1.0 && accionConfianza[2] == 1.0)) {
            return PLAYER_A_LEFTFIRE;
        }
    }

    if (accionConfianza[0] == 1.0) return PLAYER_A_LEFT;
    if (accionConfianza[1] == 1.0) return PLAYER_A_RIGHT;
    if (accionConfianza[2] == 1.0) return PLAYER_A_FIRE;

    return PLAYER_A_NOOP;
}

void usage(const char* pname) {
    std::cerr << "USAGE:\n  " << pname << " <romfile>\n";
    std::exit(1);
}

reward_t agentStep(ALEInterface& alei) {
    static int32_t lives { alei.lives() };
    reward_t reward{0};

    if (alei.lives() < lives) {
        lives = alei.lives();
        alei.act(PLAYER_A_FIRE);
    }

    const ALERAM& ram = alei.getRAM();
    std::vector<float> estado_ia;
    estado_ia.reserve(s_kinputs);

    for (unsigned i = 1; i < s_kinputs; ++i) {
        uint8_t val = ram.get(vEstadoJuego[i-1]);
        estado_ia.push_back(static_cast<float>(val));
    }

    Action act = Inteligencia(estado_ia);
    reward = alei.act(act);

    return reward;
}

int main(int argc, char** argv) {
    if (argc != 2)
        usage(argv[0]);

    ALEInterface alei;
    alei.setInt("random_seed", 0);
    alei.setFloat("repeat_action_probability", 0);
    alei.setBool("display_screen", true);
    alei.setBool("sound", true);
    alei.loadROM(argv[1]);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    reward_t totalReward = 0;

    alei.act(PLAYER_A_FIRE);

    while (!alei.game_over()) {
        totalReward += agentStep(alei);
    }

    std::cout << "Total Reward: " << totalReward << "\n";

    return 0;
}

