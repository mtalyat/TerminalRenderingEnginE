from setuptools import setup, Extension

module = Extension(
    "PyTREE",  # Module name
    sources=["../PyTREE.c", "../../../TREE/Source/TREE.c"],
    include_dirs=["../../../Source"],
    libraries=["TREE", "user32"],
    library_dirs=["../../../TREE/Source/Build", "../../../TREE/Source/Build/Debug", "../../../TREE/Source/Build/Release"],
)

setup(
    name="PyTREE",
    version="1.0",
    description="Python bindings for TREE library: https://github.com/mtalyat/TerminalRenderingEnginE",
    ext_modules=[module],
)