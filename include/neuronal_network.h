#ifndef NEURONAL_NETWORK_H
#define NEURONAL_NETWORK_H

#include "activation_function.h"
#include <vector>
#include <memory>

using namespace std;

class Neuron {
    public:
        Neuron(int, int);
        void initWeights(int);
        double getOutputValue() const { return outputValue; }
        vector<double>& getOutputWeights() { return weights; }

    
    private:
        double outputValue;
        vector<double> weights;
        int myIndex;

};

class Layer {
    public:
        Layer(int, int, unique_ptr<ActivationFunction>);
        void initNeurons(int, int);
        vector<Neuron>& getNeurons() { return neurons; }

    private:
        vector<Neuron> neurons;
		unique_ptr<ActivationFunction> activationFunction;
};

class Network {
    public:
        Network() = default;
        void addLayer(Layer);

		unsigned int getNumLayers();

    private:
        vector<Layer> layers;
};

#endif