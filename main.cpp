#include "include/neuronal_network.h"

#include <memory>
#include <iostream>

int main() {
	Network nn = Network();	

	nn.addLayer(Layer(32, 0, std::make_unique<ReLU>()));
	std::cout << "Layer added" << '\n';

	nn.addLayer(Layer(64, 0, std::make_unique<ReLU>()));
	std::cout << "Layer added" << '\n';

	std::cout << "Total number of layers: " << nn.getNumLayers() << '\n';
	
	return 0;
}
