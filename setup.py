from setuptools import setup, Extension


setup(
    name='cstring',
    version='0.1.0',
    author='Andrew Palmer',
    author_email='atp@sdf.org',
    description='String implementation based on UTF-8 C-strings',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/atpalmer-python/python-cstring',
    ext_modules=[Extension('cstring', sources=['src/cstring.c'])],
    classifiers=[
    ],
    python_requires='>=3.6',
)

