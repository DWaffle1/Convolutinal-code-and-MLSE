#include "mex.hpp"
#include "mexAdapter.hpp"
#include <iostream>

using namespace matlab::data;
using matlab::mex::ArgumentList;

class MexFunction : public matlab::mex::Function {
public:
    void operator () (ArgumentList outputs, ArgumentList inputs) {
		//initializing auxiliary variables
		int step;
		int stepR;
		int lenght = inputs[0].getNumberOfElements();
        ArrayFactory factory;
		TypedArray<double> OBits = factory.createArray<double>({ inputs[0].getNumberOfElements() * 2 });
		TypedArray<double> IBits = std::move(inputs[0]);

        //initializing main variables
		int block[3];
		int sum1;
		int sum2;

        //set starting states
		block[0] = 0;
		block[1] = 0;
		block[2] = IBits[0];
        //running through input sequance
		for (step = 0; step < lenght; step++) {
            // calculating new bits
			sum1 = (block[0] + block[2]) % 2;
			sum2 = (block[0] + block[1] + block[2]) % 2;


			if (step != 0) {
				stepR = step * 2;
			}
			else
				stepR = step;

            //wtiting new received bits into output array
			OBits[stepR] = sum1;
			OBits[stepR + 1] = sum2;
            //writing next bits  
			block[0] = block[1];
			block[1] = block[2];
			if (step < lenght - 1) {
				block[2] = IBits[step + 1];
			}
		}		
        //output new sequance into Matlab
        outputs[0] = OBits;
    }
};
