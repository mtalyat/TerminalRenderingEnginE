from typing import Callable

# Opaque native handles returned by the extension.
ThemeHandle = object
ApplicationHandle = object
ControlHandle = object
ImageHandle = object
SurfaceHandle = object

AppEvent = dict[str, int]
ControlCallback = Callable[[int, object], None]
AppCallback = Callable[[AppEvent], None]

# Constants
CONTROL_LINK_NONE: int
CONTROL_LINK_SINGLE: int
CONTROL_LINK_DOUBLE: int

DIRECTION_NONE: int
DIRECTION_EAST: int
DIRECTION_NORTH: int
DIRECTION_WEST: int
DIRECTION_SOUTH: int

ALIGNMENT_NONE: int
ALIGNMENT_LEFT: int
ALIGNMENT_RIGHT: int
ALIGNMENT_TOP: int
ALIGNMENT_BOTTOM: int
ALIGNMENT_TOPLEFT: int
ALIGNMENT_TOPRIGHT: int
ALIGNMENT_LEFTSTRETCH: int

CONTROL_TEXT_INPUT_TYPE_NONE: int
CONTROL_TEXT_INPUT_TYPE_NORMAL: int
CONTROL_TEXT_INPUT_TYPE_PASSWORD: int

CONTROL_LIST_FLAGS_NONE: int
CONTROL_LIST_FLAGS_MULTISELECT: int

CONTROL_CHECKBOX_FLAGS_NONE: int
CONTROL_CHECKBOX_FLAGS_CHECKED: int
CONTROL_CHECKBOX_FLAGS_REVERSE: int
CONTROL_CHECKBOX_FLAGS_RADIO: int

EVENT_TYPE_NONE: int
EVENT_TYPE_REFRESH: int
EVENT_TYPE_DRAW: int
EVENT_TYPE_KEY_DOWN: int
EVENT_TYPE_KEY_HELD: int
EVENT_TYPE_KEY_UP: int
EVENT_TYPE_WINDOW_RESIZE: int

COLOR_BLACK: int
COLOR_RED: int
COLOR_GREEN: int
COLOR_YELLOW: int
COLOR_BLUE: int
COLOR_MAGENTA: int
COLOR_CYAN: int
COLOR_WHITE: int
COLOR_BRIGHT_BLACK: int
COLOR_BRIGHT_RED: int
COLOR_BRIGHT_GREEN: int
COLOR_BRIGHT_YELLOW: int
COLOR_BRIGHT_BLUE: int
COLOR_BRIGHT_MAGENTA: int
COLOR_BRIGHT_CYAN: int
COLOR_BRIGHT_WHITE: int
COLOR_DEFAULT_FOREGROUND: int
COLOR_DEFAULT_BACKGROUND: int

THEME_CID_EMPTY: int
THEME_CID_SCROLL_V_AREA: int
THEME_CID_SCROLL_H_AREA: int
THEME_CID_SCROLL_V_BAR: int
THEME_CID_SCROLL_H_BAR: int
THEME_CID_UP: int
THEME_CID_DOWN: int
THEME_CID_LEFT: int
THEME_CID_RIGHT: int
THEME_CID_CHECKBOX_UNCHECKED: int
THEME_CID_CHECKBOX_CHECKED: int
THEME_CID_CHECKBOX_LEFT: int
THEME_CID_CHECKBOX_RIGHT: int
THEME_CID_RADIOBOX_UNCHECKED: int
THEME_CID_RADIOBOX_CHECKED: int
THEME_CID_RADIOBOX_LEFT: int
THEME_CID_RADIOBOX_RIGHT: int

THEME_PID_NORMAL: int
THEME_PID_FOCUSED: int
THEME_PID_ACTIVE: int
THEME_PID_HOVERED: int
THEME_PID_NORMAL_SELECTED: int
THEME_PID_FOCUSED_SELECTED: int
THEME_PID_ACTIVE_SELECTED: int
THEME_PID_HOVERED_SELECTED: int
THEME_PID_NORMAL_TEXT: int
THEME_PID_FOCUSED_TEXT: int
THEME_PID_NORMAL_SCROLL_AREA: int
THEME_PID_FOCUSED_SCROLL_AREA: int
THEME_PID_ACTIVE_SCROLL_AREA: int
THEME_PID_NORMAL_SCROLL_BAR: int
THEME_PID_FOCUSED_SCROLL_BAR: int
THEME_PID_ACTIVE_SCROLL_BAR: int
THEME_PID_CURSOR: int
THEME_PID_PROGRESS_BAR: int
THEME_PID_BACKGROUND: int

# Lifecycle / utilities

def init() -> None: ...
def deinit() -> None: ...
def time_now() -> int: ...
def result_to_string(code: int) -> str: ...
def window_set_title(title: str) -> None: ...
def window_get_extent() -> tuple[int, int]: ...
def window_present(surface: SurfaceHandle) -> None: ...
def window_beep() -> None: ...
def cursor_set_visible(visible: bool) -> None: ...
def clipboard_set_text(text: str) -> None: ...
def clipboard_get_text() -> str | None: ...

# Image / surface

def image_create(width: int, height: int) -> ImageHandle: ...
def image_get_extent(image: ImageHandle) -> tuple[int, int]: ...
def image_resize(image: ImageHandle, width: int, height: int) -> None: ...
def image_set_pixel(image: ImageHandle, x: int, y: int, char: str, foreground: int, background: int) -> None: ...
def image_get_pixel(image: ImageHandle, x: int, y: int) -> tuple[str, int, int]: ...
def image_draw_string(image: ImageHandle, x: int, y: int, text: str, color_pair: int) -> None: ...
def image_clear(image: ImageHandle, char: str, foreground: int, background: int) -> None: ...

def surface_create(width: int, height: int) -> SurfaceHandle: ...
def surface_get_extent(surface: SurfaceHandle) -> tuple[int, int]: ...
def surface_resize(surface: SurfaceHandle, width: int, height: int) -> None: ...
def surface_set_pixel(surface: SurfaceHandle, x: int, y: int, char: str, foreground: int, background: int) -> None: ...
def surface_get_pixel(surface: SurfaceHandle, x: int, y: int) -> tuple[str, int, int]: ...
def surface_draw_string(surface: SurfaceHandle, x: int, y: int, text: str, color_pair: int) -> None: ...
def surface_clear(surface: SurfaceHandle, char: str, foreground: int, background: int) -> None: ...
def surface_refresh(surface: SurfaceHandle) -> None: ...

# Theme / colors

def theme_create() -> ThemeHandle: ...
def theme_set_char(theme: ThemeHandle, character_id: int, value: str) -> None: ...
def theme_get_char(theme: ThemeHandle, character_id: int) -> str: ...
def theme_set_pixel(theme: ThemeHandle, pixel_id: int, char: str, foreground: int, background: int) -> None: ...
def theme_get_pixel(theme: ThemeHandle, pixel_id: int) -> tuple[str, int, int]: ...
def colorpair_create(foreground: int, background: int) -> int: ...
def colorpair_get_foreground(color_pair: int) -> int: ...
def colorpair_get_background(color_pair: int) -> int: ...

# Application

def application_create(capacity: int, callback: AppCallback | None = ...) -> ApplicationHandle: ...
def application_add_control(application: ApplicationHandle, control: ControlHandle) -> None: ...
def application_clear_controls(application: ApplicationHandle) -> None: ...
def application_run(application: ApplicationHandle) -> None: ...
def application_quit(application: ApplicationHandle) -> None: ...

# Control creation

def create_label(text: str, theme: ThemeHandle) -> ControlHandle: ...
def create_button(text: str, theme: ThemeHandle, on_submit: ControlCallback | None = ...) -> ControlHandle: ...
def create_text_input(
    text: str,
    capacity: int,
    placeholder: str,
    input_type: int,
    theme: ThemeHandle,
    on_change: ControlCallback | None = ...,
    on_submit: ControlCallback | None = ...,
) -> ControlHandle: ...
def create_list(
    flags: int,
    options: list[str],
    theme: ThemeHandle,
    on_change: ControlCallback | None = ...,
    on_submit: ControlCallback | None = ...,
) -> ControlHandle: ...
def create_dropdown(
    options: list[str],
    selected_index: int,
    theme: ThemeHandle,
    on_submit: ControlCallback | None = ...,
) -> ControlHandle: ...
def create_checkbox(
    text: str,
    flags: int,
    theme: ThemeHandle,
    on_check: ControlCallback | None = ...,
) -> ControlHandle: ...
def create_number_input(
    value: float,
    min_value: float,
    max_value: float,
    increment: float,
    decimal_places: int,
    theme: ThemeHandle,
    on_change: ControlCallback | None = ...,
    on_submit: ControlCallback | None = ...,
) -> ControlHandle: ...
def create_progress_bar(theme: ThemeHandle) -> ControlHandle: ...

# Generic control ops

def control_set_transform(
    control: ControlHandle,
    x: int | None = ...,
    y: int | None = ...,
    width: int | None = ...,
    height: int | None = ...,
    alignment: int | None = ...,
) -> None: ...
def control_link(control: ControlHandle, direction: int, link_type: int, other: ControlHandle) -> None: ...

# Control mutators/getters

def label_set_text(control: ControlHandle, text: str) -> None: ...
def label_get_text(control: ControlHandle) -> str: ...
def button_set_text(control: ControlHandle, text: str) -> None: ...
def button_get_text(control: ControlHandle) -> str: ...
def text_input_set_text(control: ControlHandle, text: str) -> None: ...
def text_input_get_text(control: ControlHandle) -> str: ...
def list_set_selected(control: ControlHandle, index: int, selected: bool) -> None: ...
def list_get_selected(control: ControlHandle) -> int: ...
def dropdown_set_selected(control: ControlHandle, index: int) -> None: ...
def dropdown_get_selected(control: ControlHandle) -> int: ...
def checkbox_set_checked(control: ControlHandle, checked: bool) -> None: ...
def checkbox_get_checked(control: ControlHandle) -> bool: ...
def number_input_set_value(control: ControlHandle, value: float) -> None: ...
def number_input_get_value(control: ControlHandle) -> float: ...
def progress_bar_set_value(control: ControlHandle, value: float) -> None: ...
def progress_bar_get_value(control: ControlHandle) -> float: ...
def progress_bar_set_direction(control: ControlHandle, direction: int) -> None: ...
def progress_bar_get_direction(control: ControlHandle) -> int: ...
