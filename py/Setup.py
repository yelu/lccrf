from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

ext = Extension(
			"LCCRFPy", 
			['LCCRFPy.pyx'], 
			language="c++", 
            extra_compile_args=["-std=c++11"],
            extra_link_args=["-std=c++11"],
            include_dirs=['../c/lib'],			
			)

setup(
	name = 'LCCRFPy',
    py_modules=['log', 'CRFTagger', 'LCCRFFeaturizer', 'NGramFeaturizer', 'CFGFeaturizer', 'FeatureShifter'],
	ext_modules=cythonize([ext]),
)

