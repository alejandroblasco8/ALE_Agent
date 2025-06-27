#include "../include/ML_IA_Perceptron.h"
#include "../include/activation_functions/step.h"

const unsigned s_kinputs = s_N_EstadosJuego + 1; // Perceptron inputs
const unsigned s_numActions = 5; //0 Mov izq, 1 Mov der, 2 Disparar, 3 mov der y disparar, 4 mov izq y disparar
double s_w[s_numActions][s_kinputs] = { //Example weights
    {42.3, -15.8, 0.351429, 3.6, 5.11984, -9.7741},
    {63.7, -19.2, -0.845003, 2.9, 8.00310, -10.6582},
    {51.0, -22.1, 0.114992, 5.4, 6.54729, -12.9015},
    {59.8, -17.4, -0.270011, 4.7, 9.11076, -11.3048},
    {47.2, -21.9, 0.000127, 3.2, 7.89342, -13.7760}
};

std::vector<float> h(unsigned p_id, std::vector<float>& st) {
    // p_id: Perceptron ID (0 a 4)
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

    if (accionConfianza[3] == 1.0 || (accionConfianza[1] == 1.0 && accionConfianza[2] == 1.0)) {
        return PLAYER_A_RIGHTFIRE;
    }

    if (accionConfianza[4] == 1.0 || (accionConfianza[0] == 1.0 && accionConfianza[2] == 1.0)) {
        return PLAYER_A_LEFTFIRE;
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

