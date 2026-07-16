# Terminal Rendering EnginE (TREE)
 A rendering engine for a terminal.

 ![TREE Demo Image](Images/TREE_Demo.gif)

## Features
- Support for Windows and Linux.
- Basic drawing and other graphics operations.
- Basic application and navigation support.
- Several default controls, such as:
  - Label
  - Button
  - List
  - Dropdown
  - Text Input
  - Number Input
  - Checkbox
  - Progress Bar

## Building and Linking

To use TREE in your project, you have two options:
1) Add the header and source file directly to your project.
2) Build TREE, and link it to your program.

### C Library Build Scripts

- `TREE/Source/devenv.bat build release`
- `TREE/Source/devenv.bat build debug`

### Python Binding (PyTREE)

This repository includes a CPython extension module and a higher-level Python OO wrapper.

Main pieces:
- Native extension source: `PyTREE/Source/PyTREE.c`
- Python wrapper classes: `PyTREE/Source/pytre.py`
- Python demo: `PyTREE/Test/demo.py`

Install (after publishing to PyPI):

```bash
pip install pytre
```

Build the Python module:

1. Open a terminal in `PyTREE/Source`.
2. Run `devenv.bat build`.

Notes:
- The script auto-builds TREE Release dependency if `TREE.lib` is missing.
- Output module is created under `PyTREE/Source/Build/build/lib.win-amd64-cpython-313/` (path varies by Python version/platform).

Run the Python demo:

1. Open a terminal in `PyTREE/Test`.
2. Run `python demo.py`.

## PyPI Packaging and Publishing

Packaging files live in `PyTREE/Source`:
- `setup.py`
- `pyproject.toml`
- `MANIFEST.in`

Build artifacts locally:

```bash
cd PyTREE/Source
python -m pip install --upgrade pip build twine
python -m build
python -m twine check dist/*
```

Publish manually:

```bash
python -m twine upload dist/*
```

Or use the helper batch script from `PyTREE/Source`:

```bat
REM Validate build artifacts only (no upload)
upload_pypi.bat --dry-run

REM Upload to TestPyPI (uses TEST_PYPI_TOKEN env var)
upload_pypi.bat --repository testpypi

REM Upload to PyPI (uses PYPI_TOKEN env var)
upload_pypi.bat --repository pypi
```

Or publish with GitHub Actions:
- Workflow file: `.github/workflows/pypi-wheels.yml`
- Push a tag like `pytre-v1.0.0` to trigger wheel+sdist publishing.

### Intended Compatibility

- Python: 3.9+
- OS: Windows and Linux
- Architectures built in CI by default: `x86_64` / `AMD64`

Notes:
- Linux clipboard operations depend on `xclip` being available.
- macOS support is not enabled yet in CI, but can be added with a `macos-latest` runner.

## Python API (OO Wrapper)

The recommended Python surface is class-based and uses the C extension under the hood.

Core classes:
- `Theme`
- `Application`
- `Button`, `Label`, `TextInput`, `ListControl`, `Dropdown`, `Checkbox`, `NumberInput`, `ProgressBar`

Example:

```python
import pytre as tree

tree.init()
tree.set_window_title("My App")

theme = tree.Theme()
app = tree.Application(16)

def on_quit(_sender, _value):
  app.quit()

quit_button = tree.Button("Quit", theme, on_quit).set_transform(x=1, y=1)
app.add_control(quit_button)

app.run()
tree.deinit()
```

### Runtime Page Switching

`Application` supports page-style UI switching at runtime.

Methods:
- `register_page(name, controls)`
- `show_page(name)`
- `clear_controls()`

Minimal pattern:

```python
app.register_page("page1", [button_a, list_a])
app.register_page("page2", [back_button])

app.show_page("page1")
app.run()
```

The demo (`PyTREE/Test/demo.py`) includes a two-page setup where:
- Page 1 contains the full UI and a `Page 2` button.
- Page 2 contains a button that returns to page 1.

## Notes

- Navigation between controls is explicit and uses directional links.
- If an expected navigation path does not work, verify the link direction and whether it is single or double.
- Input carry-over to the shell after exit is mitigated by flushing pending input on shutdown.

## Examples/Guides

Check out the [Demo](Demo/) directory for the demo project.

Check out the [PyTREE Demo](PyTREE/Test/demo.py) for the Python class-based API and page switching.

Check out the [Tutorial](https://github.com/mtalyat/TerminalRenderingEnginE/wiki/Tutorial) Wiki page for a usage guide.
