#ifndef NEURONAL_NETWORK_H
#define NEURONAL_NETWORK_H


#include <vector>

using namespace std;

class Neuron {
    public:
        Neuron(int index=0);
        void initWeights(int n_outputs);
        double getOutputValue() const { return outputValue; }
        vector<double>& getOutputWeights() { return weights; }
        double outputValue;
        vector<double> weights;
        int myIndex;
};

class Layer {
    public:
        Layer(int n_neurons=0);
        void initNeurons(int n_neurons);
        vector<Neuron>& getNeurons() { return neurons; }
        vector<Neuron> neurons;
        vector<double> bias;
};

class Network {
    public:
        Network(int n_inputs, int n_hidden, int n_outputs, double lr);
        void add_layer(Layer);
        void train(const vector<vector<double>>& inputs,
                   const vector<vector<double>>& targets,
                   int epochs);
        void forward_propagate(const vector<double>& input);
        void back_propagate(const vector<double>& target);
        vector<double> softmax(const vector<double>&);
        double sigmoid(double);
        double sigmoid_derivative(double);
        double relu(double);
        vector<vector<double>> getWeights(int layer_index) const;


    private:
        double learning_rate;
        vector<Layer> layers;
        int n_layers;
};

#endif
