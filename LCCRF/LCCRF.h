#pragma once

#include <functional>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include "SGD.h"
using std::function;
using std::list;
using std::wstring;
using std::vector;
using std::set;
using std::map;
using std::shared_ptr;

struct X
{
public:
	struct Key
	{
		Key(int _j, int _s1, int _s2)
		{
			j = _j;
			s1 = _s1;
			s2 = _s2;
		};
		int j;
		int s1;
		int s2;
	};

	class KeyCompare 
	{ // simple comparison function
	   public:
		  bool operator()(const X::Key x1,const X::Key x2) 
		  { 
			  if(x1.j < x2.j)
			  {
				  return true;
			  }
			  else if(x1.j > x2.j)
			  {
				  return false;
			  }

			  if(x1.s1 < x2.s1)
			  {
				  return true;
			  }
			  else if(x1.s1 > x2.s1)
			  {
				  return false;
			  }

			  return x1.s2 < x2.s2;
		  }
	};

	X()
	{
		length = 0;
	}

	shared_ptr<std::set<int>> GetFeatures(int j, int s1, int s2) const
	{
		Key key(j, s1, s2);
		if(features.count(key) == 0)
		{
			shared_ptr<std::set<int>> ret(new std::set<int>());
			return ret;
		}
		return features.at(key);
	}

	double GetFeatureValue(int j, int s1, int s2, int featureID)
	{
		Key key(j, s1, s2);
		if(features.count(key) == 0 || (features[key])->count(featureID) == 0)
		{
			return 0;
		}
		return 1;
	}

	size_t Length() const
	{
		return length;
	}

public:
	std::map<Key, shared_ptr<std::set<int>>, KeyCompare> features;
	size_t length;
};

struct Y
{
	size_t Length()
	{
		return tags.size();
	}
public:
	std::vector<int> tags;
};

class LCCRF
{
public:

	LCCRF(int, int, double);
	virtual ~LCCRF(void);

	void Fit(list<X>& xs, list<Y> ys, double learningRate = 0.01, int batch = 1, int maxIteration = 1);

	void Predict(const X& doc, Y& tags);

	void Debug(const X&, const Y&);

private:

	static double _Phi(int s1, int s2, int j,
		const X& doc, 
		vector<double>& weights);

	void _MakeDervative();

	void _MakeLikelihood();

private:

	function<double (vector<double>&, X&, Y&)> _likelihood;
	vector<function<double (vector<double>&, X&, Y&)>> _derivatives;
	const list<X>* _xs;
	const list<Y>* _ys;
	double _lambda;
	vector<double> _weights;
	int _featureCount;
	int _labelCount;
};

