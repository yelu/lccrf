#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "tclap/CmdLine.h"
#include "../lib/LCCRF.h"
#include "../lib/Str.h"

int main(int argc, char** argv)
{

	try {

		TCLAP::CmdLine cmd("Command description message", ' ', "0.9");

		TCLAP::ValueArg<std::string> trainingData("d", "data", "training data", true, "", "string");
		cmd.add(trainingData);
		TCLAP::ValueArg<int> maxIteration("i", "iter", "max iterations", true, 10, "int");
		cmd.add(maxIteration);
		TCLAP::ValueArg<double> learningRate("s", "step", "leraning rate", true, 0.001, "double");
		cmd.add(learningRate);
		TCLAP::ValueArg<double> variance("l", "l1", "variance", true, 0.001, "double");
		cmd.add(variance);
		TCLAP::ValueArg<string> modelPath("m", "model", "model path", true, "./lccrf.weights.txt", "string");
		cmd.add(modelPath);

		// Parse the argv array.
		cmd.parse(argc, argv);

		LCCRF lccrf;
		lccrf.Fit(trainingData.getValue(), maxIteration.getValue(), learningRate.getValue(), variance.getValue());
		lccrf.Save(modelPath.getValue());
		return 0;
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}
}
