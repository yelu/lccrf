#include "Viterbi.h"


Viterbi::Viterbi(DistanceFunciton distanceFunciton, int nState, int nStep):
	_distance(distanceFunciton), 
	_nState(nState), 
	_nStep(nStep), 
	_path(nStep, -1),
	_backTraceMatrix(boost::extents[nStep][nState])
{
}


Viterbi::~Viterbi(void)
{
}

const vector<int>& Viterbi::GetPath()
{
	vector<double> pi(_nState, 0);
	for(int s = 0; s < _nState; s++)
	{
		_backTraceMatrix[0][s] = -1;
		pi[s] = _distance(0, -1, s);
	}
	for(int j = 1; j < _nStep; j++)
	{
		vector<double> newPi(_nState, std::numeric_limits<double>::lowest());
		for(int s1 = 0; s1 < _nState; s1++)
		{
			for(int s2 = 0; s2 < _nState; s2++)
			{
				double d = pi[s2] + _distance(j, s2, s1);
				if(d > newPi[s1])
				{
					_backTraceMatrix[j][s1] = s2;
					newPi[s1] = d;
				}
			}
		}
		pi.swap(newPi);
	}
	int maxJ = -1;
	double maxPath = std::numeric_limits<double>::lowest();
	for(int s = 0; s < _nState; s++)
	{
		if(pi[s] > maxPath)
		{
			maxPath = pi[s];
			maxJ = s;
		}
	}
	_path[_nStep - 1] = maxJ;
	for(int j = _nStep - 2; j >= 0; j--)
	{
		_path[j] = _backTraceMatrix[j + 1][_path[j + 1]];
	}
}
