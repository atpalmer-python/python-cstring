from setuptools import setup, Extension


setup(
    name='cstring',
    ext_modules=[Extension('cstring', sources=['src/cstring.c'])]
)
