import os
import sys

BUILD_DIR = os.path.join(os.path.dirname(__file__), '..', 'Source', 'Build', 'build', 'lib.win-amd64-cpython-313')
sys.path.append(BUILD_DIR)

import PyTREE

PyTREE.init()

print("PyTREE initialized successfully.")
print(f"Time: {PyTREE.time_now()}")
i = input("Input something: ")
print(f"You entered: {i}")
print(f"Time: {PyTREE.time_now()}")


PyTREE.deinit()