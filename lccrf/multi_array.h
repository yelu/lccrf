#pragma once

#include <cmath>

template <typename T, size_t Dim, size_t N = 5120>
class MultiArray
{
};

template <typename T, size_t N>
class MultiArray<T, 1, N>
{
public:
    MultiArray(size_t d1)
    {
        AllocMemory(d1);
    }

    MultiArray(size_t d1, T initValue)
    {
        AllocMemory(d1);
        for(int i = 0; i < _d1; i++)
        {
            _pData[i] = initValue;
        }
    }

    MultiArray(T* externalData, size_t d1)
    {
        _pData = externalData;
        _d1 = d1;
        _ownership = false;
    }

    void AllocMemory(size_t d1)
    {
        if(d1 > N)
        {
            _pData = (T*)malloc(sizeof(T) * d1);
            _onStack = false;
        }
        else
        {
            _pData = _stack;
            _onStack = true;
        }
        _ownership = true;
        _d1 = d1;
    }

    virtual ~MultiArray()
    {
        if(!_ownership)
        {
            return;
        }
        if(!_onStack)
        {
            free(_pData);
        }
    }
    
    T& operator[] (size_t idx)
    {
        return _pData[idx];
    }

    const T& operator[] (size_t idx) const
    {
        return _pData[idx];
    }
    
    const T& operator() (size_t idx) const
    {
        return _pData[idx];
    }

    T& operator() (size_t idx)
    {
        return _pData[idx];
    }

	size_t Dim1() const
    {
        return _d1;
    }

    double NormalizeInPlace()
    {
        double sum = 0.0;
        for(int i = 0; i < _d1; i++)
        {
            sum += _pData[i];
        }
        for(int i = 0; i < _d1; i++)
        {
            _pData[i] /= sum;
        }
        return sum;
    }

    void LogInPlace()
    {
        for(int i = 0; i < _d1; i++)
        {
            _pData[i] = std::log(_pData[i]);
        }
    }

    void ExpInPlace()
    {
        for(int i = 0; i < _d1; i++)
        {
            _pData[i] = std::exp(_pData[i]);
        }
    }

private:
     bool _ownership;
     bool _onStack;
	 size_t _d1;
     T* _pData;
     T _stack[N];
};


template <typename T, size_t N>
class MultiArray<T, 2, N>
{
public:
    MultiArray(size_t d1, size_t d2)
    {
        AllocMemory(d1, d2);
    }

    MultiArray(size_t d1, size_t d2, T initValue)
    {
        AllocMemory(d1, d2);
        for(int i = 0; i < _d1 * _d2; i++)
        {
            _pData[i] = initValue;
        }
    }

    MultiArray(T* externalData, size_t d1, size_t d2)
    {
        _pData = externalData;
        _d1 = d1;
        _d2 = d2;
        _ownership = false;
    }

    void AllocMemory(size_t d1, size_t d2)
    {
        if(d1 * d2 > N)
        {
            _pData = (T*)malloc(sizeof(T) * d1 * d2);
            _onStack = false;
        }
        else
        {
            _pData = _stack;
            _onStack = true;
        }
        _ownership = true;
        _d1 = d1;
        _d2 = d2;
    }

    virtual ~MultiArray()
    {
        if(!_ownership)
        {
            return;
        }
        if(!_onStack)
        {
            free(_pData);
        }
    }
    
    MultiArray<T, 1, 0> operator[] (size_t idx)
    {
        MultiArray<T, 1, 0> subArray(_pData + idx * _d2, _d2);
        return subArray;
    }

    const MultiArray<T, 1, 0> operator[] (size_t idx) const
    {
        MultiArray<T, 1, 0> subArray(_pData + idx * _d2, _d2);
        return subArray;
    }
    
    const T& operator() (const size_t& idx1, const size_t& idx2) const
    {
        return _pData[idx1 * _d2 + idx2];
    }

    T& operator() (const size_t& idx1, const size_t& idx2)
    {
        return _pData[idx1 * _d2 + idx2];
    }

    int Dim1() const
    {
        return _d1;
    }

    int Dim2() const
    {
        return _d2;
    }

    void ExpInPlace()
    {
        for (int i = 0; i < _d1 * _d2; i++)
        {
            _pData[i] = std::exp(_pData[i]);
        }
    }

private:
     bool _ownership;
     bool _onStack;
	 size_t _d1;
	 size_t _d2;
     T* _pData;
     T _stack[N];
};

template <typename T, size_t N>
class MultiArray<T, 3, N>
{
public:
    MultiArray(size_t d1, size_t d2, size_t d3)
    {
        AllocMemory(d1, d2, d3);
    }

    MultiArray(size_t d1, size_t d2, size_t d3, T initValue)
    {
        AllocMemory(d1, d2, d3);
        for(int i = 0; i < _d1 * _d2 * _d3; i++)
        {
            _pData[i] = initValue;
        }
    }

    MultiArray(const MultiArray& ma)
    {
        AllocMemory(ma._d1, ma._d2, ma._d3);
        for(int i = 0; i < _d1 * _d2 * _d3; i++)
        {
            _pData[i] = ma._pData[i];
        }
    }

    MultiArray(T* externalData, size_t d1, size_t d2, size_t d3)
    {
        _pData = externalData;
        _d1 = d1;
        _d2 = d2;
        _d3 = d3;
        _ownership = false;
    }

    void AllocMemory(size_t d1, size_t d2, size_t d3)
    {
        if(d1 * d2 * d3 > N)
        {
            _pData = (T*)malloc(sizeof(T) * d1 * d2 * d3);
            _onStack = false;
        }
        else
        {
            _pData = _stack;
            _onStack = true;
        }
        _ownership = true;
        _d1 = d1;
        _d2 = d2;
        _d3 = d3;
    }

    virtual ~MultiArray()
    {
        if(!_ownership)
        {
            return;
        }
        if(!_onStack)
        {
            free(_pData);
        }
    }

    MultiArray<T, 2, 0> operator[] (size_t idx)
    {
        MultiArray<T, 2, 0> subArray(_pData + idx * _d2 * _d3, _d2, _d3);
        return subArray;
    }

    const MultiArray<T, 2, 0> operator[] (size_t idx) const
    {
        MultiArray<T, 2, 0> subArray(_pData + idx * _d2 * _d3, _d2, _d3);
        return subArray;
    }
    
    const T& operator() (size_t idx1, size_t idx2, size_t idx3) const
    {
        return _pData[idx1 * _d2 * _d3 + idx2 * _d3 + idx3];
    }

    T& operator() (size_t idx1, size_t idx2, size_t idx3)
    {
        return _pData[idx1 * _d2 * _d3 + idx2 * _d3 + idx3];
    }

	size_t Dim1() const
    {
        return _d1;
    }

	size_t Dim2() const
    {
        return _d2;
    }

	size_t Dim3() const
    {
        return _d3;
    }

    void ExpInPlace()
    {
        for(int i = 0; i < _d1 * _d2 * _d3; i++)
        {
            _pData[i] = std::exp(_pData[i]);
        }
    }

private:
     bool _ownership;
     bool _onStack;
	 size_t _d1;
	 size_t _d2;
	 size_t _d3;
     T* _pData;
     T _stack[N];
};
