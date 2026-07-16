import os
import sys

BUILD_DIR = os.path.join(
    os.path.dirname(__file__),
    "..",
    "Source",
    "Build",
    "build",
    "lib.win-amd64-cpython-313",
)
sys.path.append(BUILD_DIR)

import PyTREE

APP = None
PROGRESS = None


def on_quit(_sender, _value):
    PyTREE.application_quit(APP)


def on_number_change(_sender, value):
    percent = (float(value) - 0.0) / (100.0 - 0.0)
    PyTREE.progress_bar_set_value(PROGRESS, percent)


def build_demo():
    global APP
    global PROGRESS

    PyTREE.init()
    PyTREE.window_set_title("TREE Test")

    theme = PyTREE.theme_create()
    APP = PyTREE.application_create(32)

    options = [f"Option {i}" for i in range(1, 21)]

    quit_button = PyTREE.create_button("Quit", theme, on_quit)
    PyTREE.control_set_transform(quit_button, x=1, y=1)

    text_input = PyTREE.create_text_input(
        "Enter text here",
        256,
        "Placeholder",
        PyTREE.CONTROL_TEXT_INPUT_TYPE_NORMAL,
        theme,
    )
    PyTREE.control_set_transform(text_input, x=50, y=1)

    list_control = PyTREE.create_list(PyTREE.CONTROL_LIST_FLAGS_MULTISELECT, options, theme)
    PyTREE.control_set_transform(
        list_control,
        x=1,
        y=5,
        width=20,
        height=3,
        alignment=PyTREE.ALIGNMENT_LEFTSTRETCH,
    )

    multi_line_text_input = PyTREE.create_text_input(
        "Enter multi-line text here.\n\nHello world!",
        256,
        "Placeholder",
        PyTREE.CONTROL_TEXT_INPUT_TYPE_NORMAL,
        theme,
    )
    PyTREE.control_set_transform(multi_line_text_input, x=50, y=6, width=30, height=20)

    drop_controls = []
    for i in range(3):
        dropdown = PyTREE.create_dropdown(options, 0, theme)
        PyTREE.control_set_transform(dropdown, x=23, y=3 + i * 11)
        drop_controls.append(dropdown)

    checkboxes = []
    for i in range(8):
        flags = i if (i & 2) == 0 else (i | PyTREE.CONTROL_CHECKBOX_FLAGS_REVERSE)
        checkbox = PyTREE.create_checkbox(" Normal" if (i & 2) == 0 else "Reversed ", flags, theme)
        PyTREE.control_set_transform(checkbox, x=82, y=3 + i)
        checkboxes.append(checkbox)

    label = PyTREE.create_label("Checkboxes:", theme)
    PyTREE.control_set_transform(label, x=82, y=2)

    number_input = PyTREE.create_number_input(0.0, 0.0, 100.0, 1.0, 0, theme, on_number_change)
    PyTREE.control_set_transform(number_input, x=-21, y=3, width=7, height=1, alignment=PyTREE.ALIGNMENT_TOPRIGHT)

    PROGRESS = PyTREE.create_progress_bar(theme)
    PyTREE.control_set_transform(PROGRESS, x=-21, y=1, width=20, height=1, alignment=PyTREE.ALIGNMENT_TOPRIGHT)

    PyTREE.control_link(list_control, PyTREE.DIRECTION_NORTH, PyTREE.CONTROL_LINK_DOUBLE, quit_button)
    PyTREE.control_link(quit_button, PyTREE.DIRECTION_EAST, PyTREE.CONTROL_LINK_DOUBLE, drop_controls[0])

    for i in range(3):
        PyTREE.control_link(
            drop_controls[i],
            PyTREE.DIRECTION_SOUTH,
            PyTREE.CONTROL_LINK_DOUBLE,
            drop_controls[(i + 1) % 3],
        )

    PyTREE.control_link(drop_controls[0], PyTREE.DIRECTION_EAST, PyTREE.CONTROL_LINK_DOUBLE, text_input)
    PyTREE.control_link(text_input, PyTREE.DIRECTION_SOUTH, PyTREE.CONTROL_LINK_DOUBLE, multi_line_text_input)
    PyTREE.control_link(text_input, PyTREE.DIRECTION_EAST, PyTREE.CONTROL_LINK_DOUBLE, checkboxes[0])

    for i in range(8):
        PyTREE.control_link(
            checkboxes[i],
            PyTREE.DIRECTION_SOUTH,
            PyTREE.CONTROL_LINK_DOUBLE,
            checkboxes[(i + 1) % 8],
        )

    PyTREE.control_link(checkboxes[0], PyTREE.DIRECTION_EAST, PyTREE.CONTROL_LINK_DOUBLE, number_input)

    controls = [
        quit_button,
        list_control,
        *drop_controls,
        text_input,
        multi_line_text_input,
        *checkboxes,
        label,
        number_input,
        PROGRESS,
    ]

    for control in controls:
        PyTREE.application_add_control(APP, control)

    PyTREE.application_run(APP)
    PyTREE.deinit()


if __name__ == "__main__":
    build_demo()
