from setuptools import setup, Extension

module1 = Extension('bl_bl',
                    sources = ['main.c'],
                    extra_compile_args=["-O3"])

setup (name = 'bl_bl',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [module1])