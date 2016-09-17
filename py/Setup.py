from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
	name = 'LCCRFPy',
    py_modules=['log', 'CRFTagger', 'LCCRFFeaturizer', 'NGramFeaturizer', 'CFGFeaturizer'],
	ext_modules=cythonize(
	[
		Extension(
			"LCCRFPy", 
			['LCCRFPy.pyx'], 
			language="c++", 
            extra_compile_args=["-std=c++11"],
            include_dirs=['../c/lib'],			
			)
	]),
)


