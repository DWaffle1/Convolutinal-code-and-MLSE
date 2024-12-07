#include "mex.hpp"
#include "mexAdapter.hpp"
#include <iostream>
#include <cmath>

using namespace matlab::data;
using matlab::mex::ArgumentList;

class MexFunction : public matlab::mex::Function {
public:
	void operator () (ArgumentList outputs, ArgumentList inputs) {
        //initializing auxiliary variables
		const int lenght = inputs[1].getNumberOfElements();
		ArrayFactory factory;
		TypedArray<double> OBits = factory.createArray<double>({ inputs[1].getNumberOfElements() / 2  ,inputs[2].getNumberOfElements() });
		TypedArray<double> IBits = std::move(inputs[0]);
		const int variants = 4;
		const int time = int(lenght / 2+1);
		int p;
		int t;
		int q;

        //running through arrays with diffirent noise
		for (int j = 0; j < inputs[2].getNumberOfElements(); j++) {
            //initializing main variables
			TypedArray<double> Path = factory.createArray<double>({ inputs[1].getNumberOfElements() / 2 + 1 });
			TypedArray<double> M = factory.createArray<double>({ 4 ,inputs[1].getNumberOfElements() / 2 + 1 });
			TypedArray<double> u = factory.createArray<double>({ 4 ,2 });
			TypedArray<double> m = factory.createArray<double>({ 4 ,2 });
			M[0][0] = 0;
			M[1][0] = 100;
			M[2][0] = 100;
			M[3][0] = 100;

            //running through received bits
			for (t = 0; t < time - 1; t++) {
                //set relations between t and t+1 steps and calculating Hamming distance
				for (q = 0; q < variants; q++) {
					switch (q) {
					case 0:
						u[q][0] = abs(IBits[2 * t][j] - 0) + abs(IBits[2 * t + 1][j] - 0);//AA
						u[q][1] = abs(IBits[2 * t][j] - 1) + abs(IBits[2 * t + 1][j] - 1);//CA
						break;
					case 1:
						u[q][0] = abs(IBits[2 * t][j] - 1) + abs(IBits[2 * t + 1][j] - 1);//AB
						u[q][1] = abs(IBits[2 * t][j] - 0) + abs(IBits[2 * t + 1][j] - 0);//CB
						break;
					case 2:
						u[q][0] = abs(IBits[2 * t][j] - 0) + abs(IBits[2 * t + 1][j] - 1);//BC
						u[q][1] = abs(IBits[2 * t][j] - 1) + abs(IBits[2 * t + 1][j] - 0);//DC
						break;
					case 3:
						u[q][0] = abs(IBits[2 * t][j] - 1) + abs(IBits[2 * t + 1][j] - 0);//BD
						u[q][1] = abs(IBits[2 * t][j] - 0) + abs(IBits[2 * t + 1][j] - 1);//DD
						break;
					}
				}
                //summarizing path metric with branch metric
				for (p = 0; p < variants; p++) {
					switch (p) {
					case 0:
						m[0][0] = M[p][t] + u[0][0];
						m[1][0] = M[p][t] + u[1][0];
						break;
					case 1:
						m[2][0] = M[p][t] + u[2][0];
						m[3][0] = M[p][t] + u[3][0];
						break;
					case 2:
						m[0][1] = M[p][t] + u[0][1];
						m[1][1] = M[p][t] + u[1][1];
						break;
					case 3:
						m[2][1] = M[p][t] + u[2][1];
						m[3][1] = M[p][t] + u[3][1];
						break;
					}
				}
                //finding minimal path metric for t+1
				for (q = 0; q < variants; q++) {
					if (m[q][0] >= m[q][1]) {
						M[q][t + 1] = m[q][1];
					}
					else if (m[q][0] < m[q][1]) {
						M[q][t + 1] = m[q][0];
					}
				}
			}
            //building path 
			for (t = time-1; t >0 ; t--) {
				int comp = 100;
				for (p = 0; p < variants; p++) {
					if (comp > M[p][t]) {
						comp = M[p][t];
						Path[t] = p;
					}
					else if (comp == M[p][t] && t != time - 1) {
						if (Path[t + 1] == 0 || Path[t + 1] == 1) {
							if (M[0][t] >= M[2][t]) {
								Path[t] = 2;
							}
							else
								Path[t] = 0;
						}
						else
							if (M[1][t] >= M[3][t]) {
								Path[t] = 3;
							}
							else
								Path[t] = 1;
					}
					
				}
                //correlate bits with received path
				if (Path[t] == 0 || Path[t] == 2) {
					OBits[t - 1][j] = 0;
				}
				else if (Path[t] == 1 || Path[t] == 3) {
					OBits[t - 1][j] = 1;
				}
			}
		}
		//output decoded sequance into Matlab
		outputs[0] = OBits;
	}
};
