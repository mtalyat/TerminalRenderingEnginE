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

import pytre as tree

APP = None
PROGRESS = None


def on_quit(_sender, _value):
    APP.quit()


def on_switch_to_page_two(_sender, _value):
    APP.show_page("page2")


def on_switch_to_page_one(_sender, _value):
    APP.show_page("page1")


def on_number_change(_sender, value):
    percent = (float(value) - 0.0) / (100.0 - 0.0)
    PROGRESS.value = percent


def build_demo():
    global APP
    global PROGRESS

    tree.init()
    tree.set_window_title("TREE Test")

    theme = tree.Theme()
    APP = tree.Application(32)

    options = [f"Option {i}" for i in range(1, 21)]

    quit_button = tree.Button("Quit", theme, on_quit).set_transform(x=1, y=1)
    page_two_button = tree.Button("Page 2", theme, on_switch_to_page_two).set_transform(x=1, y=5)

    text_input = tree.TextInput(
        "Enter text here",
        256,
        "Placeholder",
        tree.TEXT_INPUT_TYPE_NORMAL,
        theme,
    ).set_transform(x=50, y=1)

    list_control = tree.ListControl(
        tree.LIST_FLAGS_MULTISELECT,
        options,
        theme,
    ).set_transform(x=1, y=9, width=20, height=3, alignment=tree.ALIGNMENT_LEFTSTRETCH)

    multi_line_text_input = tree.TextInput(
        "Enter multi-line text here.\n\nHello world!",
        256,
        "Placeholder",
        tree.TEXT_INPUT_TYPE_NORMAL,
        theme,
    ).set_transform(x=50, y=6, width=30, height=20)

    drop_controls = []
    for i in range(3):
        dropdown = tree.Dropdown(options, 0, theme).set_transform(x=23, y=3 + i * 11)
        drop_controls.append(dropdown)

    checkboxes = []
    for i in range(8):
        flags = i if (i & 2) == 0 else (i | tree.CHECKBOX_FLAGS_REVERSE)
        checkbox = tree.Checkbox(
            " Normal" if (i & 2) == 0 else "Reversed ",
            flags,
            theme,
        ).set_transform(x=82, y=3 + i)
        checkboxes.append(checkbox)

    label = tree.Label("Checkboxes:", theme).set_transform(x=82, y=2)

    number_input = tree.NumberInput(
        0.0,
        0.0,
        100.0,
        1.0,
        0,
        theme,
        on_number_change,
    ).set_transform(x=-21, y=3, width=7, height=1, alignment=tree.ALIGNMENT_TOPRIGHT)

    PROGRESS = tree.ProgressBar(theme).set_transform(x=-21, y=1, width=20, height=1, alignment=tree.ALIGNMENT_TOPRIGHT)

    list_control.link(tree.DIRECTION_NORTH, quit_button)
    quit_button.link(tree.DIRECTION_EAST, drop_controls[0])
    quit_button.link(tree.DIRECTION_SOUTH, page_two_button, tree.CONTROL_LINK_SINGLE)
    page_two_button.link(tree.DIRECTION_NORTH, quit_button, tree.CONTROL_LINK_SINGLE)
    page_two_button.link(tree.DIRECTION_SOUTH, list_control, tree.CONTROL_LINK_SINGLE)

    for i in range(3):
        drop_controls[i].link(tree.DIRECTION_SOUTH, drop_controls[(i + 1) % 3])

    drop_controls[0].link(tree.DIRECTION_EAST, text_input)
    text_input.link(tree.DIRECTION_SOUTH, multi_line_text_input)
    text_input.link(tree.DIRECTION_EAST, checkboxes[0])

    for i in range(8):
        checkboxes[i].link(tree.DIRECTION_SOUTH, checkboxes[(i + 1) % 8])

    checkboxes[0].link(tree.DIRECTION_EAST, number_input)

    page_one_controls = [
        quit_button,
        page_two_button,
        list_control,
        *drop_controls,
        text_input,
        multi_line_text_input,
        *checkboxes,
        label,
        number_input,
        PROGRESS,
    ]

    return_button = tree.Button("Return to Page 1", theme, on_switch_to_page_one).set_transform(x=1, y=1)
    page_two_controls = [
        return_button,
    ]

    APP.register_page("page1", page_one_controls)
    APP.register_page("page2", page_two_controls)
    APP.show_page("page1")

    APP.run()
    tree.deinit()


if __name__ == "__main__":
    build_demo()
