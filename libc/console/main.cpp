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

		std::ifstream infile(trainingData.getValue());

		// read feature and tag counts.
		int fcount = 0;
		int tcount = 0;
		std::string line;
		std::getline(infile, line);
		std::istringstream iss(line);
		iss >> fcount >> tcount;
		LCCRF lccrf(fcount, tcount);

		// get common features.
		std::getline(infile, line);
		while (std::getline(infile, line) && !line.empty())
		{
			vector<string> tokens;
			split(line, ' ', tokens);
			vector<string> feats;
			split(tokens[0], ',', feats);
			vector<string> poses;
			split(tokens[1], ',', poses);
			
			for (auto ite = poses.begin(); ite != poses.end(); ite++)
			{
				X::SetCommonFeature(atoi(ite->c_str()),
					atoi(feats[1].c_str()),
					atoi(feats[2].c_str()),
					atoi(feats[0].c_str()));
			}
		}

		XList xList;
		YList yList;
		
		while (std::getline(infile, line))
		{
			vector<string> tags;
			split(line, ',', tags);
			Y y;
			for (auto ite = tags.begin(); ite != tags.end(); ite++)
			{
				int tag = atoi(ite->c_str());
				y.AppendTag(tag);
			}
			yList.Append(y);

			X x(tags.size());
			while (std::getline(infile, line) && !line.empty())
			{
				vector<string> tokens;
				split(line, ' ', tokens);
				vector<string> feats;
				split(tokens[0], ',', feats);
				vector<string> poses;
				split(tokens[1], ',', poses);

				for (auto ite = poses.begin(); ite != poses.end(); ite++)
				{
					x.SetFeature(atoi(ite->c_str()),
						atoi(feats[1].c_str()),
						atoi(feats[2].c_str()),
						atoi(feats[0].c_str()));
				}
			}
			xList.Append(x);
		}

		lccrf.Fit(xList, yList, maxIteration.getValue(), learningRate.getValue(), variance.getValue());
		lccrf.Save(modelPath.getValue());
		return 0;
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}
}
