#include "../include/activation_function.h"

double ReLU::forward(double x) {
	return x > 0 ? x : 0;
}

double ReLU::backward(double x) {
	return x > 0 ? 1 : 0;
}
