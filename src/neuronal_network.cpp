#include "../include/neuronal_network.h"
#include <cstdlib>
#include <cmath>
#include <ctime>

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

void
Network::addLayer(Layer layer) {
    layers.push_back(std::move(layer));
}

unsigned int
Network::getNumLayers() {
	return layers.size();
}
