from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
	name = 'LCCRFPy',
	ext_modules=cythonize(
	[
		Extension(
			"LCCRFPy", 
			['LCCRFPy.pyx'], 
			language="c++", 
            include_dirs=['../LCCRF'],
			extra_compile_args=["-std=c++11", "-IC:\\local\\boost_1_55_0"],
			)
	]),
)