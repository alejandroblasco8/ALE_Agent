#include "neuronal_network.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <algorithm>

Neuron::Neuron(int n_weights, int index) {
    //Inicializar pesos teniendo en cuenta el número de pesos (n_weights)
    this->initWeights(n_weights);

    //Atributo para facilitar la identificación de cada neurona (se quitará si no ayuda)
    this->myIndex = index;
}

void
Neuron::initWeights(int n_weights){
    for(int i = 0; i < n_weights; i++){
        //Habrá que modificar el 0 por pesos random
        weights.push_back(0);
    }
}

Layer::Layer(int n_neurons, int n_weights) {
    this->initNeurons(n_neurons, n_weights);
}

void
Layer::initNeurons(int n_neurons, int n_weights) {
    for(int i = 0; i < n_neurons; i++){
        neurons.push_back(Neuron(n_weights, i));
    }
}

Network::Network(int n_inputs, int n_hidden, int n_outputs) {
    this->n_layers = 0;
    //Añadimos la capa oculta. Añadimos 1 al segunddo argumento para el bias.
    this->add_layer(Layer(n_hidden, n_inputs+1));
    //Añadimos capa de salida. Si es, por ejemplo, binaria, n_outputs será 2
    this->add_layer(Layer(n_outputs, n_hidden+1));
}

void
Network::add_layer(Layer new_layer) {
    layers.push_back(new_layer);
    this->n_layers++;
}
