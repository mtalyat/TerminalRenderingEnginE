import os
import sys

import PyTREE.Source
import PyTREE.Source.setup

BUILD_DIR = os.path.join(os.path.dirname(__file__), '..', 'Source', 'Build', 'build', 'lib.win-amd64-cpython-313')
sys.path.append(BUILD_DIR)

import PyTREE

PyTREE.init()

print("PyTREE initialized successfully.")

PyTREE.deinit()