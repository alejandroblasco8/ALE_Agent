#include "IA_ML.h"
#include "TPartido.h"

///////////////////////////////////////////////////////////
// Inteligencia:                                         //
// * Vectores de Pesos de los perceptrones para la IA    //
///////////////////////////////////////////////////////////
const unsigned s_kinputs = s_N_EstadosJuego-1;
const unsigned s_numIAs = 6;
double s_w[s_numIAs][2][s_kinputs] = {
    // IA 0: W_up, W_down
      {  {  -128,   644.2, -166.2510 ,  12.00850, -27.32860,  99.5423,  -74.42380, -1198.33000}
      ,  {  -1  ,    -2.6,   51.8678 ,   1.00819, -30.62510,  85.5748,   -9.01556,     2.74653}
      } 
    // IA 1: W_up, W_down
   ,  {  {    0 ,   199.8,    0      ,   0      ,  -8.74410, -43.5157, -135.70100,  -451.17500}
      ,  {  -68 ,  -123.6,    0      ,   0      ,  -5.82508,  19.2405, -143.82000,   119.56300}
      }
    // IA 2: W_up, W_down
   ,  {  {    0 ,   199.8,  -31.20720, -17.66770,  -8.74410, -43.5157, -135.70100,  -451.17500}
      ,  {  -68 ,  -123.6,    1.29211,  38.03280,  -5.82508,  19.2405, -143.82000,   119.56300}
      }
    // IA 3: W_up, W_down
   ,  {  { -248 , -2319.062,   58.0342 ,  82.41930,  -4.17163, -13.4985, -119.84500,   345.77600}
      ,  { -390 ,  2216.8,  -48.9395 , -50.18210,  -5.03074,  18.7431, -489.70500,  -414.06100}
      }
    // IA 4: W_up, W_down
   ,  {  {  185 ,   -69.4,    0      ,   0      , -55.59740, -36.7335,  -95.71310,  -176.88200}
      ,  {  174 ,   -92.2,    0      ,   0      , -62.34150,  18.6742,  -59.36490,   317.80500}
      }
    // IA 5: W_up, W_down
   ,  {  {  114 ,   566.0,    2.55401,  13.34970, -31.50070, -59.0554,   13.81800,  -255.62300}
      ,  {  251 ,  -785.2,  109.14500, 148.47100, -33.06440, -45.5554, -243.01100,  1010.17000}
      }
};

///////////////////////////////////////////////////////////
// Funcion Hipótesis (H):                                //
// * Calcula la hipótesis (1/0) según estado y pesos     //
///////////////////////////////////////////////////////////
unsigned h(unsigned wn, unsigned dir, const double* st) {
   double hval = 0;

   // Sumar wx de 1 a n
   for(unsigned i=1; i < s_kinputs; i++)
      hval += st[i-1] * s_w[wn][dir][i];

   // Añadir w0x0 (1*x0), calcular el signo y devolver
   return (hval + s_w[wn][dir][0] > 0) ? 1 : 0;
}

///////////////////////////////////////////////////////////
// INTELIGENCIA DE LAS PALAS                             //
// * Versión controlada por Machine Learning             //
///////////////////////////////////////////////////////////
#ifdef MACHINE_LEARNING
double
TPartido::Inteligencia (int jugador) {
   double aum {0.0};
   unsigned bup, bdown;

   if (jugador == 1) {
      bup   = h(IA_Izq, 0, vEstadoJuego);
      bdown = h(IA_Izq, 1, vEstadoJuego);
   } else {
      double vEstadoJ_2[s_N_EstadosJuego];

      vEstadoJ_2[s_jugadorVY]   = campo.PaletaDchaVel();
      vEstadoJ_2[s_enemigoYRel] = campo.PaletaIzqY() - campo.PaletaDchaY();
      vEstadoJ_2[s_enemigoVY]   = campo.PaletaIzqVel();
      vEstadoJ_2[s_pelotaXRel]  = campo.PaletaDchaX() - campo.PelotaX();
      vEstadoJ_2[s_pelotaYRel]  = campo.PelotaY() - campo.PaletaDchaY();
      vEstadoJ_2[s_pelotaVX]    = -campo.PelotaVX();
      vEstadoJ_2[s_pelotaVY]    = campo.PelotaVY();

      bup   = h(IA_Der, 0, vEstadoJ_2);
      bdown = h(IA_Der, 1, vEstadoJ_2);     
   }

   if      ( bup && !bdown) aum = -0.2;      
   else if (!bup &&  bdown) aum = +0.2;

   return aum;
}
#endif

///////////////////////////////////////////////////////////
// maxIA:                                                //
// * Devuelve el máximo número válido de inteligencia    //
// Machine Learning Disponible                           //
///////////////////////////////////////////////////////////
unsigned
TPartido::maxIA() const {
   return s_numIAs - 1;
}
