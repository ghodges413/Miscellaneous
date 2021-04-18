//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "Matrix.h"
#include "Vector.h"
#include "Random.h"

const int maxEpochs = 1000;

/*
====================================================
NeuronLayer_t

// A layer of neurons will be the weight matrix and bias vector, that produces an activation layer L from input L-1
====================================================
*/
struct NeuronLayer_t {
	MatMN weights;	// weights connecting this layer to the previous layer
	VecN bias;		// bias of this layer

	VecN cachedZ;	// cached z
	VecN cachedA;	// cached activations := sigma( z )

	static VecN ActivateReLU( NeuronLayer_t & layer, const VecN & activationsIn );	// Rectified Linear Unit

	static float CostQuadratic( const VecN & activations, const VecN & desiredOutput );

	void Shuffle() {
		for ( int i = 0; i < weights.M; i++ ) {
			for ( int j = 0; j < weights.N; j++ ) {
				weights.rows[ i ][ j ] = Random::Gaussian();
			}
		}

		for ( int i = 0; i < bias.N; i++ ) {
			bias[ i ] = Random::Gaussian();
		}
	}
};

/*
====================================================
NeuronLayer_t::ActivateReLU
Rectified Linear Unit
====================================================
*/
VecN NeuronLayer_t::ActivateReLU( NeuronLayer_t & layer, const VecN & activationsIn ) {
	VecN z = layer.bias;
	z += layer.weights * activationsIn;
	layer.cachedZ = z;

	for ( int i = 0; i < z.N; i++ ) {
		float zz = z[ i ];

		z[ i ] = std::max( 0.0f, zz );
	}

	layer.cachedA = z;
	return z;
}

/*
====================================================
NeuronLayer_t::CostQuadratic
====================================================
*/
float NeuronLayer_t::CostQuadratic( const VecN & activations, const VecN & desiredOutput ) {
	assert( activations.N == desiredOutput.N );

	float cost = 0.0f;
	for ( int i = 0; i < activations.N; i++ ) {
		float delta = desiredOutput[ i ] - activations[ i ];
		cost += delta * delta;
	}

	cost *= 0.5f / float( activations.N );
	return cost;
}

/*
====================================================
PrintResult
====================================================
*/
void PrintResult( const VecN & input, const VecN & output, const VecN & correctOutput ) {
	const float cost = NeuronLayer_t::CostQuadratic( output, correctOutput );

	printf( "\n====================================\n" );
	printf( "Input:  " );
	for ( int i = 0; i < input.N; i++ ) {
		printf( " %.4f", input[ i ] );
	}

	printf( "\n" );
	printf( "Output:  " );
	for ( int i = 0; i < output.N; i++ ) {
		printf( " %.4f", output[ i ] );
	}

	printf( "\n" );
	printf( "Desired: " );
	for ( int i = 0; i < correctOutput.N; i++ ) {
		printf( " %.4f", correctOutput[ i ] );
	}
	printf( "\n" );
	printf( "Cost: %f", cost );
	printf( "\n====================================\n" );
}

/*
====================================================
ReLuDerivative
====================================================
*/
VecN ReLuDerivative( const VecN & z ) {
	VecN dSigma( z.N );

	for ( int i = 0; i < z.N; i++ ) {
		float slope = 1.0f;
// 		if ( z[ i ] < 0.0f ) {
// 			slope = 0.0f;
// 		}

		dSigma[ i ] = slope;
	}

	return dSigma;
}

/*
====================================================
AbsoluteError
====================================================
*/
VecN AbsoluteError( const VecN & activations, const VecN & z, const VecN & correctOutput ) {
	// The gradient of quadratic cost function = - ( y( x ) - a )
	VecN dCda = activations - correctOutput;

	VecN dSigma = ReLuDerivative( z );

	VecN delta = dCda.Hadamard( dSigma );
	return delta;
}

/*
====================================================
CostWeightGradient
====================================================
*/
MatMN CostWeightGradient( const VecN & errorDelta, const VecN & activationsPreviousLayer ) {
	int cols = activationsPreviousLayer.N;
	int rows = errorDelta.N;
	MatMN gradCW( rows, cols );

	for ( int m = 0; m < rows; m++ ) {
		for ( int n = 0; n < cols; n++ ) {
			gradCW.rows[ m ][ n ] = activationsPreviousLayer[ n ] * errorDelta[ m ];
		}
	}

	return gradCW;
}

/*
====================================================
PrintNeuralLayer
====================================================
*/
void PrintNeuralLayer( const NeuronLayer_t & layer ) {
	printf( "\nNeurons:\n" );
	for ( int i = 0; i < layer.weights.M; i++ ) {
		printf( "w:" );
		for ( int j = 0; j < layer.weights.N; j++ ) {		
			printf( " %.2f", layer.weights.rows[ i ][ j ] );
		}
		printf( "\n" );
	}

	printf( "\n" );
	for ( int j = 0; j < layer.bias.N; j++ ) {
		printf( "b: %.2f\n", layer.bias[ j ] );
	}
}

/*
====================================================
TestBinaryNetwork
====================================================
*/
void TestBinaryNetwork() {
	NeuronLayer_t outputLayer;
	VecN inputActivation( 3 );
	VecN outputActivations;

	VecN correctOutputs[ 4 ];
	correctOutputs[ 0 ] = VecN( 2 );
	correctOutputs[ 1 ] = VecN( 2 );
	correctOutputs[ 2 ] = VecN( 2 );
	correctOutputs[ 3 ] = VecN( 2 );

	correctOutputs[ 0 ].Zero();
	correctOutputs[ 1 ].Zero();
	correctOutputs[ 2 ].Zero();
	correctOutputs[ 3 ].Zero();

	correctOutputs[ 1 ][ 0 ] = 1.0f;
	correctOutputs[ 2 ][ 1 ] = 1.0f;
	correctOutputs[ 3 ][ 0 ] = 1.0f;
	correctOutputs[ 3 ][ 1 ] = 1.0f;


	outputLayer.weights = MatMN( 2, 3 );
	outputLayer.bias = VecN( 2 );
	outputLayer.Shuffle();

	PrintNeuralLayer( outputLayer );

	for ( int epoch = 0; epoch < maxEpochs; epoch++ ) {
		const bool doPrint = ( ( maxEpochs - 1 ) == epoch ) || ( 0 == epoch );
		if ( doPrint ) {
			printf( "\n\nEpoch %i\n", epoch );
		}
		float num = 0;

		// Training data 0
		num += 1.0f;
		inputActivation.Zero();
		outputActivations = NeuronLayer_t::ActivateReLU( outputLayer, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 0 ] );
		}

		VecN errors = AbsoluteError( outputActivations, outputLayer.cachedZ, correctOutputs[ 0 ] );
		MatMN gradCWeights = CostWeightGradient( errors, inputActivation );
		VecN gradCBias = errors;

		// Training data 1
		num += 1.0f;
		inputActivation.Zero();
		inputActivation[ 0 ] = 1.0f;
		outputActivations = NeuronLayer_t::ActivateReLU( outputLayer, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 1 ] );
		}

		errors = AbsoluteError( outputActivations, outputLayer.cachedZ, correctOutputs[ 1 ] );
		gradCWeights += CostWeightGradient( errors, inputActivation );
		gradCBias += errors;

		// Training data 2
		num += 1.0f;
		inputActivation.Zero();
		inputActivation[ 1 ] = 1.0f;
		outputActivations = NeuronLayer_t::ActivateReLU( outputLayer, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 2 ] );
		}

		errors = AbsoluteError( outputActivations, outputLayer.cachedZ, correctOutputs[ 2 ] );
		gradCWeights += CostWeightGradient( errors, inputActivation );
		gradCBias += errors;

		// Training data 3
		num += 1.0f;
		inputActivation.Zero();
		inputActivation[ 2 ] = 1.0f;
		outputActivations = NeuronLayer_t::ActivateReLU( outputLayer, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 3 ] );
		}

		errors = AbsoluteError( outputActivations, outputLayer.cachedZ, correctOutputs[ 3 ] );
		gradCWeights += CostWeightGradient( errors, inputActivation );
		gradCBias += errors;


		const float learningRate = 0.525f;
		outputLayer.bias += gradCBias * ( -learningRate / num );
		outputLayer.weights += gradCWeights * ( -learningRate / num );
	}

	PrintNeuralLayer( outputLayer );

	printf( "Finished TestBinaryNetwork\n\n\n" );
}

/*
====================================================
FeedForward
====================================================
*/
VecN FeedForward( NeuronLayer_t * layers, const int numLayers, const VecN & inputs ) {
	VecN activations = inputs;
	for ( int i = 0; i < numLayers; i++ ) {
		activations = NeuronLayer_t::ActivateReLU( layers[ i ], activations );
	}

	return activations;
}

/*
====================================================
BackPropagateError
====================================================
*/
VecN BackPropagateError( const MatMN & weights, const VecN & errors, const VecN & z ) {
	MatMN weightTransposed = weights.Transpose();
	VecN transformedError = weightTransposed * errors;

	VecN dSigma = ReLuDerivative( z );
	VecN propagatedError = transformedError.Hadamard( dSigma );
	return propagatedError;
}

/*
====================================================
BackPropagateErrors
====================================================
*/
void BackPropagateErrors( NeuronLayer_t * layers, const int numLayers, const VecN & inputs, const VecN & outputActivations, const VecN & correctOutput, MatMN * gradCostWeights, VecN * gradCostBias ) {
	const int lastLayer = numLayers - 1;

	VecN errors;
	for ( int layerIdx = lastLayer; layerIdx >= 0; layerIdx-- ) {
		// Calculate the error for this layer
		if ( layerIdx == lastLayer ) {
			errors = AbsoluteError( outputActivations, layers[ lastLayer ].cachedZ, correctOutput );
		} else {
			errors = BackPropagateError( layers[ layerIdx + 1 ].weights, errors, layers[ layerIdx ].cachedZ );
		}
		
		VecN previousActivations;
		if ( layerIdx > 0 ) {
			previousActivations = layers[ layerIdx - 1 ].cachedA;
		} else {
			previousActivations = inputs;
		}

		// Accumulate gradients
		gradCostWeights[ layerIdx ] += CostWeightGradient( errors, previousActivations );
		gradCostBias[ layerIdx ] += errors;
	}
}

/*
====================================================
TestHiddenLayerNetwork
====================================================
*/
void TestHiddenLayerNetwork() {
	NeuronLayer_t neuronLayers[ 2 ];	// one hidden layer, and one output layer
	VecN inputActivation( 3 );
	VecN outputActivations;

	VecN correctOutputs[ 4 ];
	correctOutputs[ 0 ] = VecN( 2 );
	correctOutputs[ 1 ] = VecN( 2 );
	correctOutputs[ 2 ] = VecN( 2 );
	correctOutputs[ 3 ] = VecN( 2 );

	correctOutputs[ 0 ].Zero();
	correctOutputs[ 1 ].Zero();
	correctOutputs[ 2 ].Zero();
	correctOutputs[ 3 ].Zero();

	correctOutputs[ 1 ][ 0 ] = 1.0f;
	correctOutputs[ 2 ][ 1 ] = 1.0f;
	correctOutputs[ 3 ][ 0 ] = 1.0f;
	correctOutputs[ 3 ][ 1 ] = 1.0f;

	// hidden layer
	neuronLayers[ 0 ].weights = MatMN( 3, 3 );
	neuronLayers[ 0 ].bias = VecN( 3 );
	neuronLayers[ 0 ].Shuffle();

	// output layer
	neuronLayers[ 1 ].weights = MatMN( 2, 3 );
	neuronLayers[ 1 ].bias = VecN( 2 );
	neuronLayers[ 1 ].Shuffle();
	

	//PrintNeuralLayer( outputLayer );
	for ( int epoch = 0; epoch < maxEpochs; epoch++ ) {
		const bool doPrint = ( ( maxEpochs - 1 ) == epoch ) || ( 0 == epoch );
		if ( doPrint ) {
			printf( "\n\nEpoch %i\n", epoch );
		}
		float num = 0;

		MatMN gradCostWeights[ 2 ];
		VecN gradCostBias[ 2 ];

		for ( int layerIdx = 0; layerIdx < 2; layerIdx++ ) {
			// Get the gradients to be the right dimensions
			gradCostWeights[ layerIdx ] = neuronLayers[ layerIdx ].weights;
			gradCostBias[ layerIdx ] = neuronLayers[ layerIdx ].bias;

			// Initialize gradients to zero (these will be accumulated in the back propagation)
			gradCostWeights[ layerIdx ].Zero();
			gradCostBias[ layerIdx ].Zero();
		}

		// Training data 0
		num += 1.0f;
		inputActivation.Zero();
		outputActivations = FeedForward( neuronLayers, 2, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 0 ] );
		}
		BackPropagateErrors( neuronLayers, 2, inputActivation, outputActivations, correctOutputs[ 0 ], gradCostWeights, gradCostBias );

		// Training data 1
		num += 1.0f;
		inputActivation.Zero();
		inputActivation[ 0 ] = 1.0f;
		outputActivations = FeedForward( neuronLayers, 2, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 1 ] );
		}
		BackPropagateErrors( neuronLayers, 2, inputActivation, outputActivations, correctOutputs[ 1 ], gradCostWeights, gradCostBias );

		// Training data 2
		num += 1.0f;
		inputActivation.Zero();
		inputActivation[ 1 ] = 1.0f;
		outputActivations = FeedForward( neuronLayers, 2, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 2 ] );
		}
		BackPropagateErrors( neuronLayers, 2, inputActivation, outputActivations, correctOutputs[ 2 ], gradCostWeights, gradCostBias );

		// Training data 3
		num += 1.0f;
		inputActivation.Zero();
		inputActivation[ 2 ] = 1.0f;
		outputActivations = FeedForward( neuronLayers, 2, inputActivation );
		if ( doPrint ) {
			PrintResult( inputActivation, outputActivations, correctOutputs[ 3 ] );
		}
		BackPropagateErrors( neuronLayers, 2, inputActivation, outputActivations, correctOutputs[ 3 ], gradCostWeights, gradCostBias );

		const float learningRate = 0.525f;
		for ( int layerIdx = 0; layerIdx < 2; layerIdx++ ) {
			neuronLayers[ layerIdx ].bias += gradCostBias[ layerIdx ] * ( -learningRate / num );
			neuronLayers[ layerIdx ].weights += gradCostWeights[ layerIdx ] * ( -learningRate / num );
		}
	}

	PrintNeuralLayer( neuronLayers[ 0 ] );
	PrintNeuralLayer( neuronLayers[ 1 ] );

	printf( "Finished TestHiddenLayerNetwork\n\n\n" );
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	TestBinaryNetwork();
	TestHiddenLayerNetwork();

	return 0;
}
