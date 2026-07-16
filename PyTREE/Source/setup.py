from pathlib import Path
import platform

from setuptools import Extension, setup


ROOT = Path(__file__).resolve().parent
README = ROOT / "README.md"


libraries = []
if platform.system() == "Windows":
    libraries.append("user32")


module = Extension(
    "PyTREE",
    sources=["PyTREE.c", "TREE.c"],
    include_dirs=["."],
    libraries=libraries,
)


setup(
    name="pytre",
    version="1.0.0",
    description="Python bindings for Terminal Rendering EnginE (TREE)",
    long_description=README.read_text(encoding="utf-8"),
    long_description_content_type="text/markdown",
    author="mtalyat",
    url="https://github.com/mtalyat/TerminalRenderingEnginE",
    project_urls={
        "Source": "https://github.com/mtalyat/TerminalRenderingEnginE",
        "Issues": "https://github.com/mtalyat/TerminalRenderingEnginE/issues",
    },
    license="MIT",
    python_requires=">=3.9",
    ext_modules=[module],
    py_modules=["pytre"],
    data_files=[("", ["PyTREE.pyi"])],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: Python :: 3.13",
        "Programming Language :: C",
        "Topic :: Software Development :: User Interfaces",
        "Topic :: Terminals",
    ],
)