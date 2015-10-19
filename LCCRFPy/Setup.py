from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
	name = 'LCCRFPy',
    py_modules=['log', 'AnyFeaturizer', 'CRFTagger', 'CRFTaggerFeaturizer', 'NGramFeaturizer'],
	ext_modules=cythonize(
	[
		Extension(
			"LCCRFPy", 
			['LCCRFPy.pyx'], 
			language="c++", 
            extra_compile_args=["-std=c++11"],
            include_dirs=['../LCCRF'],			
			)
	]),
)

#config for windows.
# If want to use msvc, set this in command line. then use 'python Setup.py build --compiler=msvc'
# SET VS90COMNTOOLS=%VS100COMNTOOLS% (vs2010)
# SET VS90COMNTOOLS=%VS110COMNTOOLS% (vs2012)
# SET VS90COMNTOOLS=%VS120COMNTOOLS% (vs2013)

'''
setup(
	name = 'LCCRFPy',
	ext_modules=cythonize(
	[
		Extension(
			"LCCRFPy", 
			['LCCRFPy.pyx'], 
			language="c++", 
            extra_compile_args=["/Zi", "/Od", "-std=c++11"],
            extra_link_args=["-debug"],
            include_dirs=['../LCCRF'],			
			)
	]),
)
'''

