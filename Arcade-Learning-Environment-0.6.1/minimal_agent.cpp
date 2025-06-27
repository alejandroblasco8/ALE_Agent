#include <iostream>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <SDL/SDL.h>
#include "src/ale_interface.hpp"
#include "src/common/Constants.h"
#include <cstdio>

// Constants
constexpr uint32_t maxSteps = 20000;

// File where RAM states and actions will be stored
std::ofstream g_csvFile;

// Global Variables
size_t g_frameCount = 0;
std::vector<uint8_t> g_prevRam(128, 0);
bool g_firstFrame = true;

///////////////////////////////////////////////////////////////////////////////
/// Get info from RAM
///////////////////////////////////////////////////////////////////////////////
int32_t getPlayerX(ALEInterface& alei) {
   return alei.getRAM().get(72) + ((rand() % 3) - 1);
}

int32_t getBallX(ALEInterface& alei) {
   return alei.getRAM().get(99) + ((rand() % 3) - 1);
}

///////////////////////////////////////////////////////////////////////////////
/// Print RAM and action
///////////////////////////////////////////////////////////////////////////////

void printRam(ALEInterface& alei, Action act){
   std::system("clear");

   const ALERAM &ram = alei.getRAM();

   // Show RAM
   std::printf("    ");
   for (int col = 0; col < 16; ++col)
      std::printf(" %3d", col);
   std::printf("\n");

   for (size_t i = 0; i < ram.size(); ++i) {
      if (i % 16 == 0)
         std::printf("%3zu:", i);
      std::printf(" %3u", ram.get(i));
      if ((i + 1) % 16 == 0)
         std::printf("\n");
   }

   // Show RAM(t) - RAM(t-1)
   if (!g_firstFrame) {
      std::printf("\n== Δ RAM (actual - anterior) ==\n    ");
      for (int col = 0; col < 16; ++col)
         std::printf(" %4d", col);
      std::printf("\n");

      for (size_t i = 0; i < ram.size(); ++i) {
         if (i % 16 == 0)
            std::printf("%3zu:", i);
         int delta = static_cast<int>(ram.get(i)) - static_cast<int>(g_prevRam[i]);
         std::printf(" %+4d", delta);
         if ((i + 1) % 16 == 0)
            std::printf("\n");
      }
   }

   // Mostrar ram.get(47) para ver los valores en binario, es candidata a contener la "i" utilizada en el calculo
   // de las posiciones del jugador y enemigos, explicada en la memoria
   /*std::printf("RAM[70] = %3u => Binario: ", ram.get(70));
   for (int bit = 7; bit >= 0; --bit)
      std::printf("%d", (ram.get(70) >> bit) & 1);
   std::printf("\n");*/
   


   // Save prev RAM
   for (size_t i = 0; i < ram.size(); ++i)
      g_prevRam[i] = ram.get(i);

   g_firstFrame = false;

   // Show action and frame
   std::cout << "\nAction: " << action_to_string(act) << std::endl;
   std::cout << "Frame: " << g_frameCount << std::endl;
   ++g_frameCount;
}

///////////////////////////////////////////////////////////////////////////////
/// Save frame state and action
///////////////////////////////////////////////////////////////////////////////
void saveFrame(ALEInterface& alei, Action act) {
   const ALERAM &ram = alei.getRAM();
   if (g_csvFile.is_open()) {
      for (size_t i = 0; i < ram.size(); ++i) {
         g_csvFile << static_cast<int>(ram.get(i));
         if (i + 1 < ram.size())
            g_csvFile << ',';
      }
      g_csvFile << ',' << action_to_string(act) << '\n';
   }
}

///////////////////////////////////////////////////////////////////////////////
/// Do Next Agent Step
///////////////////////////////////////////////////////////////////////////////
reward_t agentStep(ALEInterface& alei) {
   static int32_t lives { alei.lives() };
   reward_t reward{0};

   // When we lose a life, we need to press FIRE to start again
   if (alei.lives() < lives) {
      lives = alei.lives();
      alei.act(PLAYER_A_FIRE);
   }

   // Read keyboard state
   //SDL_PumpEvents();
   Uint8 *state = SDL_GetKeyState(nullptr); //const
   bool left  = state[SDLK_LEFT];
   bool right = state[SDLK_RIGHT];
   bool fire  = state[SDLK_SPACE];

   Action act = PLAYER_A_NOOP;
   if (left && fire)       act = PLAYER_A_LEFTFIRE;
   else if (right && fire) act = PLAYER_A_RIGHTFIRE;
   else if (left)          act = PLAYER_A_LEFT;
   else if (right)         act = PLAYER_A_RIGHT;
   else if (fire)          act = PLAYER_A_FIRE;


   reward = alei.act(act);
   
   printRam(alei, act);
   saveFrame(alei, act);

   return reward;


}

///////////////////////////////////////////////////////////////////////////////
/// Print usage and exit
///////////////////////////////////////////////////////////////////////////////
void usage(char const* pname) {
   std::cerr
      << "\nUSAGE:\n" 
      << "   " << pname << " <romfile>\n";
   exit(-1);
}

///////////////////////////////////////////////////////////////////////////////
/// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
   reward_t totalReward{};
   ALEInterface alei{};

   // Check input parameter
   if (argc != 2)
      usage(argv[0]);

   // Configure alei object.
   alei.setInt  ("random_seed", 0);
   alei.setFloat("repeat_action_probability", 0); //Si no se pone a 0 por defecto es 0.25. Es la probabilidad de que repita un movimiento sin importar lo que hayamos hecho
   alei.setBool ("display_screen", true);
   alei.setInt("frame_skip", 1);
   alei.setBool ("sound", false);
   alei.loadROM (argv[1]); //alei.loadROM (argv[1]);

   // Checks if CSV file exists before writing header
   bool writeHeader = true;
   std::ifstream infile("ram_log.csv");
   if (infile.good() && infile.peek() != std::ifstream::traits_type::eof()) {
      writeHeader = false;
   }
   infile.close();

   // Open CSV file and write header
   g_csvFile.open("ram_log.csv", std::ios::out | std::ios::app);

   // Init
   std::srand(static_cast<uint32_t>(std::time(0)));

   // Main loop
   {
      alei.act(PLAYER_A_FIRE);
      uint32_t step{};
      while ( !alei.game_over() && step < maxSteps ) {
         totalReward += agentStep(alei);
         ++step;
      }

      std::cout << "Steps: " << step << std::endl;
      std::cout << "Reward: " << totalReward << std::endl;
   }

   if (g_csvFile.is_open())
      g_csvFile.close();

   return 0;
}
