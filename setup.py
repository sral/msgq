#!/usr/bin/env python
#
# Setup script for msgq

from setuptools import setup, Extension

setup(name="msgq",
      version="0.1",
      description="System V IPC Message Queue Python Extension Module",
      author="Lars Djerf",
      author_email="lars.djerf@gmail.com",
      url="http://github.com/sral/msgq",
      ext_modules=[Extension("msgq", ["msgq.c"])],
      #packages=find_packages(),
      test_suite="tests")
