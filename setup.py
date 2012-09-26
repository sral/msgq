#!/usr/bin/env python
#
# Setup script for msgq

from distutils.core import setup
from distutils.extension import Extension
setup(name='msgq',
      version='0.1',
      description='System V IPC Message Queue Python Extension Module',
      author='Lars Djerf',
      author_email='lars.djerf@gmail.com',
      url='',
      ext_modules=[Extension('msgq', ['msgq.c'])]
      )
