#pragma once

#include "types.h"

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE(CRFTypes)
{
	class_<X>("X")
		.def("SetFeature", &X::SetFeature);

	class_<Y>("Y")
		.def("AppendTag", &Y::AppendTag);

    class_<ListX>("ListX")
	    .def("Append", &ListX::Append);
}