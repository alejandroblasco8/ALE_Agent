#ifndef ACTIVATION_FUNCTION_H
#define ACTIVATION_FUNCTION_H

class ActivationFunction {
	public:
		virtual double forward(double x) = 0;
		virtual double backward(double x) = 0;

		virtual ~ActivationFunction() = default;
};

class ReLU : public ActivationFunction {
		double forward(double x) override;
		double backward(double x) override;
};

#endif
