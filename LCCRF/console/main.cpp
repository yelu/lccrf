#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "tclap/CmdLine.h"
#include "../lib/LCCRF.h"

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

		std::ifstream infile(trainingData.getValue());
		// read feature and tag counts.
		int fcount = 0;
		int tcount = 0;
		infile >> fcount >> tcount;

		LCCRF lccrf(fcount, tcount);
		int xFeatureCount = 0;
		int yLength = 0;
		XList x;
		YList y;
		while (infile >> yLength)
		{	
			Y ySample;
			for (int i = 0; i < yLength; i++)
			{
				int tag = 0;
				infile >> tag;
				ySample.AppendTag(tag);
			}
			y.Append(ySample);

			infile >> xFeatureCount;
			X xSample(yLength);
			for (int i = 0; i < xFeatureCount; i++)
			{
				int pos, preTag, curTag, fid;
				infile >> pos >> preTag >> curTag >> fid;
				xSample.SetFeature(pos, preTag, curTag, fid);
			}
			x.Append(xSample);
		}

		lccrf.Fit(x, y, maxIteration.getValue(), learningRate.getValue(), variance.getValue());
		lccrf.Save(modelPath.getValue());
		return 0;
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}
}
