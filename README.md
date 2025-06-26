Makefile en /build:
USE_SDL : Para que se muestre en pantalla y no solo entrene de fondo
BUILD_EXAMPLES : Son agentes de ejemplo, lo suyo es desactivarlo para que no compile cosas que no vamos a usar

cmake .. -DUSE_SDL=ON -DBUILD_EXAMPLES=OFF
--------------------------------------------
para compilar:

make -jx
x = Hilos

Esto devuelve fuera de la carpeta build el arhchivo libale.so
--------------------------------------------
Fuera de /build:
Para compilar el minimal agent(leer todo):
g++ -o min minimal_agent.cpp libale.so $(sdl-config --libs) -Wl,-rpath=.

///////////////////
/// Explicación
///////////////////

g++ -o min minimal_agent.cpp libale.so

PERO todavia NO podemos hacer ./min. Aunque hayamos enlazado ale no forma parte del fichero min ya que es una librería compartida y debe estar compartida en la RAM. Al poner el comando "$ldd min" podemos ver que, de todos los ficheros de los que depende "min", libale.so no se encuentra en la RAM ya que asume que va a estar en la carpeta del sistema (como pasa con la mayoria de las librerias).

g++ -o min minimal_agent.cpp libale.so -Wl,-rpath=.

Esto hace que busque también las librerías en la carpeta actual, donde sí que se encuentra libale.so. Lo que pasa es que ahora lo que nos falta es la biblioteca SDL, así que necesitamos añadirla con:

g++ -o min minimal_agent.cpp libale.so $(sdl-config --libs) -Wl,-rpath=.
