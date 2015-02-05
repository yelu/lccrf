#include "Viterbi.h"
#include <limits>


Viterbi::Viterbi()
{
}


Viterbi::~Viterbi(void)
{
}

void Viterbi::GetPath(const MultiArray<double, 3>& graph, vector<int>& res)
{
    int nStep = graph.Dim1();
    int nState = graph.Dim2();
	vector<double> pi(nState, 0);
	vector<vector<int>> backTraceMatrix(nStep, vector<int>(nState, -1));

	for(int s = 0; s < nState; s++)
	{
		backTraceMatrix[0][s] = -1;
		pi[s] = graph[0][0][s];
	}

	vector<double> newPi(nState, std::numeric_limits<double>::lowest());
	for(int j = 1; j < nStep; j++)
	{		
		for(int s1 = 0; s1 < nState; s1++)
		{
			for(int s2 = 0; s2 < nState; s2++)
			{
				double d = pi[s2] + graph[j][s2][s1];
				if(d > newPi[s1])
				{
					backTraceMatrix[j][s1] = s2;
					newPi[s1] = d;
				}
			}
		}
		pi.swap(newPi);
	}

	// find the end point of optimal path.
	int maxJ = -1;
	double maxPath = std::numeric_limits<double>::lowest();
	for(int s = 0; s < nState; s++)
	{
		if(pi[s] > maxPath)
		{
			maxPath = pi[s];
			maxJ = s;
		}
	}
	res[nStep - 1] = maxJ;
	for(int j = nStep - 2; j >= 0; j--)
	{
		res[j] = backTraceMatrix[j + 1][res[j + 1]];
	}
}
