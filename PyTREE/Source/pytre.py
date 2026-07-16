from __future__ import annotations

"""High-level Python wrapper for the native PyTREE extension.

This module provides typed, documented classes over the C-extension API so
VS Code/Pylance can show useful hover information and argument types.
"""

from typing import Callable, TypedDict

import PyTREE as _c


class AppEvent(TypedDict, total=False):
    """Application-level event dictionary passed to Application callbacks.

    Keys are populated depending on event type.
    - type: TREE_EVENT_TYPE_* integer
    - control: Native control pointer value (int) or 0
    - key: TREE_KEY_* integer (for key events)
    - modifiers: TREE_KEY_MODIFIER_FLAGS_* bitmask
    - width/height: new terminal size (for resize events)
    """

    type: int
    control: int
    key: int
    modifiers: int
    width: int
    height: int


CallbackValue = None | str | float | bool | int | list[int]
ControlSubmitCallback = Callable[["Control", CallbackValue], None]
ControlChangeCallback = Callable[["Control", CallbackValue], None]
ControlCheckCallback = Callable[["Control", bool], None]
ApplicationEventCallback = Callable[["Application", AppEvent], None]


CONTROL_LINK_SINGLE: int = _c.CONTROL_LINK_SINGLE
CONTROL_LINK_DOUBLE: int = _c.CONTROL_LINK_DOUBLE

DIRECTION_NONE: int = _c.DIRECTION_NONE
DIRECTION_EAST: int = _c.DIRECTION_EAST
DIRECTION_NORTH: int = _c.DIRECTION_NORTH
DIRECTION_WEST: int = _c.DIRECTION_WEST
DIRECTION_SOUTH: int = _c.DIRECTION_SOUTH

ALIGNMENT_NONE: int = _c.ALIGNMENT_NONE
ALIGNMENT_TOPLEFT: int = _c.ALIGNMENT_TOPLEFT
ALIGNMENT_TOPRIGHT: int = _c.ALIGNMENT_TOPRIGHT
ALIGNMENT_LEFTSTRETCH: int = _c.ALIGNMENT_LEFTSTRETCH

TEXT_INPUT_TYPE_NONE: int = _c.CONTROL_TEXT_INPUT_TYPE_NONE
TEXT_INPUT_TYPE_NORMAL: int = _c.CONTROL_TEXT_INPUT_TYPE_NORMAL
TEXT_INPUT_TYPE_PASSWORD: int = _c.CONTROL_TEXT_INPUT_TYPE_PASSWORD

LIST_FLAGS_NONE: int = _c.CONTROL_LIST_FLAGS_NONE
LIST_FLAGS_MULTISELECT: int = _c.CONTROL_LIST_FLAGS_MULTISELECT

CHECKBOX_FLAGS_NONE: int = _c.CONTROL_CHECKBOX_FLAGS_NONE
CHECKBOX_FLAGS_CHECKED: int = _c.CONTROL_CHECKBOX_FLAGS_CHECKED
CHECKBOX_FLAGS_REVERSE: int = _c.CONTROL_CHECKBOX_FLAGS_REVERSE
CHECKBOX_FLAGS_RADIO: int = _c.CONTROL_CHECKBOX_FLAGS_RADIO

COLOR_BLACK: int = _c.COLOR_BLACK
COLOR_RED: int = _c.COLOR_RED
COLOR_GREEN: int = _c.COLOR_GREEN
COLOR_YELLOW: int = _c.COLOR_YELLOW
COLOR_BLUE: int = _c.COLOR_BLUE
COLOR_MAGENTA: int = _c.COLOR_MAGENTA
COLOR_CYAN: int = _c.COLOR_CYAN
COLOR_WHITE: int = _c.COLOR_WHITE
COLOR_BRIGHT_BLACK: int = _c.COLOR_BRIGHT_BLACK
COLOR_BRIGHT_RED: int = _c.COLOR_BRIGHT_RED
COLOR_BRIGHT_GREEN: int = _c.COLOR_BRIGHT_GREEN
COLOR_BRIGHT_YELLOW: int = _c.COLOR_BRIGHT_YELLOW
COLOR_BRIGHT_BLUE: int = _c.COLOR_BRIGHT_BLUE
COLOR_BRIGHT_MAGENTA: int = _c.COLOR_BRIGHT_MAGENTA
COLOR_BRIGHT_CYAN: int = _c.COLOR_BRIGHT_CYAN
COLOR_BRIGHT_WHITE: int = _c.COLOR_BRIGHT_WHITE
COLOR_DEFAULT_FOREGROUND: int = _c.COLOR_DEFAULT_FOREGROUND
COLOR_DEFAULT_BACKGROUND: int = _c.COLOR_DEFAULT_BACKGROUND

THEME_CID_EMPTY: int = _c.THEME_CID_EMPTY
THEME_CID_SCROLL_V_AREA: int = _c.THEME_CID_SCROLL_V_AREA
THEME_CID_SCROLL_H_AREA: int = _c.THEME_CID_SCROLL_H_AREA
THEME_CID_SCROLL_V_BAR: int = _c.THEME_CID_SCROLL_V_BAR
THEME_CID_SCROLL_H_BAR: int = _c.THEME_CID_SCROLL_H_BAR
THEME_CID_UP: int = _c.THEME_CID_UP
THEME_CID_DOWN: int = _c.THEME_CID_DOWN
THEME_CID_LEFT: int = _c.THEME_CID_LEFT
THEME_CID_RIGHT: int = _c.THEME_CID_RIGHT
THEME_CID_CHECKBOX_UNCHECKED: int = _c.THEME_CID_CHECKBOX_UNCHECKED
THEME_CID_CHECKBOX_CHECKED: int = _c.THEME_CID_CHECKBOX_CHECKED
THEME_CID_CHECKBOX_LEFT: int = _c.THEME_CID_CHECKBOX_LEFT
THEME_CID_CHECKBOX_RIGHT: int = _c.THEME_CID_CHECKBOX_RIGHT
THEME_CID_RADIOBOX_UNCHECKED: int = _c.THEME_CID_RADIOBOX_UNCHECKED
THEME_CID_RADIOBOX_CHECKED: int = _c.THEME_CID_RADIOBOX_CHECKED
THEME_CID_RADIOBOX_LEFT: int = _c.THEME_CID_RADIOBOX_LEFT
THEME_CID_RADIOBOX_RIGHT: int = _c.THEME_CID_RADIOBOX_RIGHT

THEME_PID_NORMAL: int = _c.THEME_PID_NORMAL
THEME_PID_FOCUSED: int = _c.THEME_PID_FOCUSED
THEME_PID_ACTIVE: int = _c.THEME_PID_ACTIVE
THEME_PID_HOVERED: int = _c.THEME_PID_HOVERED
THEME_PID_NORMAL_SELECTED: int = _c.THEME_PID_NORMAL_SELECTED
THEME_PID_FOCUSED_SELECTED: int = _c.THEME_PID_FOCUSED_SELECTED
THEME_PID_ACTIVE_SELECTED: int = _c.THEME_PID_ACTIVE_SELECTED
THEME_PID_HOVERED_SELECTED: int = _c.THEME_PID_HOVERED_SELECTED
THEME_PID_NORMAL_TEXT: int = _c.THEME_PID_NORMAL_TEXT
THEME_PID_FOCUSED_TEXT: int = _c.THEME_PID_FOCUSED_TEXT
THEME_PID_NORMAL_SCROLL_AREA: int = _c.THEME_PID_NORMAL_SCROLL_AREA
THEME_PID_FOCUSED_SCROLL_AREA: int = _c.THEME_PID_FOCUSED_SCROLL_AREA
THEME_PID_ACTIVE_SCROLL_AREA: int = _c.THEME_PID_ACTIVE_SCROLL_AREA
THEME_PID_NORMAL_SCROLL_BAR: int = _c.THEME_PID_NORMAL_SCROLL_BAR
THEME_PID_FOCUSED_SCROLL_BAR: int = _c.THEME_PID_FOCUSED_SCROLL_BAR
THEME_PID_ACTIVE_SCROLL_BAR: int = _c.THEME_PID_ACTIVE_SCROLL_BAR
THEME_PID_CURSOR: int = _c.THEME_PID_CURSOR
THEME_PID_PROGRESS_BAR: int = _c.THEME_PID_PROGRESS_BAR
THEME_PID_BACKGROUND: int = _c.THEME_PID_BACKGROUND


def init() -> None:
    """Initialize TREE runtime for terminal rendering/input."""

    _c.init()


def deinit() -> None:
    """Shutdown TREE runtime and restore terminal state."""

    _c.deinit()


def time_now() -> int:
    """Return current time in milliseconds."""

    return _c.time_now()


def set_window_title(title: str) -> None:
    """Set terminal window title."""

    _c.window_set_title(title)


def beep() -> None:
    """Emit a terminal beep."""

    _c.window_beep()


def set_cursor_visible(visible: bool) -> None:
    """Show or hide terminal cursor."""

    _c.cursor_set_visible(bool(visible))


def get_window_extent() -> tuple[int, int]:
    """Get terminal window size as (width, height)."""

    return _c.window_get_extent()


def clipboard_set_text(text: str) -> None:
    """Set system clipboard text."""

    _c.clipboard_set_text(text)


def clipboard_get_text() -> str | None:
    """Get system clipboard text, or None when unavailable/empty."""

    return _c.clipboard_get_text()


def colorpair_create(foreground: int, background: int) -> int:
    """Create packed TREE color-pair byte from foreground/background."""

    return _c.colorpair_create(foreground, background)


def colorpair_get_foreground(color_pair: int) -> int:
    """Extract foreground color from packed color-pair."""

    return _c.colorpair_get_foreground(color_pair)


def colorpair_get_background(color_pair: int) -> int:
    """Extract background color from packed color-pair."""

    return _c.colorpair_get_background(color_pair)


class Theme:
    """Theme object used by controls for character/pixel styling."""

    _handle: object

    def __init__(self) -> None:
        self._handle = _c.theme_create()

    def set_char(self, character_id: int, value: str) -> Theme:
        """Set theme character by TREE_THEME_CID_* id.

        value should be a non-empty string; only first character is used.
        """

        _c.theme_set_char(self._handle, character_id, value)
        return self

    def get_char(self, character_id: int) -> str:
        """Get theme character by TREE_THEME_CID_* id."""

        return _c.theme_get_char(self._handle, character_id)

    def set_pixel(self, pixel_id: int, char: str, foreground: int, background: int) -> Theme:
        """Set theme pixel by TREE_THEME_PID_* id using char and colors."""

        _c.theme_set_pixel(self._handle, pixel_id, char, foreground, background)
        return self

    def get_pixel(self, pixel_id: int) -> tuple[str, int, int]:
        """Get theme pixel as (char, foreground, background)."""

        return _c.theme_get_pixel(self._handle, pixel_id)


class Application:
    """Application run-loop container for controls and pages."""

    _controls: list[Control]
    _pages: dict[str, list[Control]]
    _current_page: str | None
    _on_event: ApplicationEventCallback | None
    _event_adapter: Callable[[AppEvent], None] | None
    _handle: object

    def __init__(self, capacity: int = 32, on_event: ApplicationEventCallback | None = None) -> None:
        self._controls = []
        self._pages = {}
        self._current_page = None
        self._on_event = on_event
        self._event_adapter = None
        if on_event is not None:

            def _adapter(event: AppEvent) -> None:
                on_event(self, event)

            self._event_adapter = _adapter
        self._handle = _c.application_create(capacity, self._event_adapter)

    def add_control(self, control: Control) -> Application:
        """Add a control to the active application page."""

        _c.application_add_control(self._handle, control._handle)
        self._controls.append(control)
        control._application = self
        return self

    def clear_controls(self) -> Application:
        """Remove all controls from current application view."""

        _c.application_clear_controls(self._handle)
        self._controls = []
        return self

    def register_page(self, name: str, controls: list[Control]) -> Application:
        """Register named page with a list of controls."""

        self._pages[name] = list(controls)
        return self

    def show_page(self, name: str) -> Application:
        """Switch to a previously registered page."""

        if name not in self._pages:
            raise KeyError(f"Unknown page: {name}")

        self.clear_controls()
        for control in self._pages[name]:
            self.add_control(control)
        self._current_page = name
        return self

    def run(self) -> None:
        """Run blocking application loop until quit."""

        _c.application_run(self._handle)

    def quit(self) -> None:
        """Request application loop exit."""

        _c.application_quit(self._handle)


class Control:
    """Base control wrapper for transform/link operations."""

    _handle: object
    _application: Application | None

    def __init__(self) -> None:
        self._handle = None
        self._application = None

    def set_transform(
        self,
        x: int | None = None,
        y: int | None = None,
        width: int | None = None,
        height: int | None = None,
        alignment: int | None = None,
    ) -> Control:
        """Set local transform values.

        Any omitted argument keeps existing native value.
        """

        kwargs: dict[str, int] = {}
        if x is not None:
            kwargs["x"] = x
        if y is not None:
            kwargs["y"] = y
        if width is not None:
            kwargs["width"] = width
        if height is not None:
            kwargs["height"] = height
        if alignment is not None:
            kwargs["alignment"] = alignment
        _c.control_set_transform(self._handle, **kwargs)
        return self

    def link(self, direction: int, other: Control, link: int = CONTROL_LINK_DOUBLE) -> Control:
        """Link this control to another for keyboard navigation."""

        _c.control_link(self._handle, direction, link, other._handle)
        return self


class Label(Control):
    """Label control."""

    def __init__(self, text: str, theme: Theme) -> None:
        super().__init__()
        self._handle = _c.create_label(text, theme._handle)

    @property
    def text(self) -> str:
        """Current label text."""

        return _c.label_get_text(self._handle)

    @text.setter
    def text(self, value: str) -> None:
        _c.label_set_text(self._handle, value)


class Button(Control):
    """Button control with submit callback."""

    _on_submit: ControlSubmitCallback | None
    _submit_adapter: Callable[[int, CallbackValue], None] | None

    def __init__(self, text: str, theme: Theme, on_submit: ControlSubmitCallback | None = None) -> None:
        super().__init__()
        self._on_submit = on_submit
        self._submit_adapter = None

        if on_submit is not None:

            def _adapter(_sender: int, value: CallbackValue) -> None:
                on_submit(self, value)

            self._submit_adapter = _adapter

        self._handle = _c.create_button(text, theme._handle, self._submit_adapter)

    @property
    def text(self) -> str:
        return _c.button_get_text(self._handle)

    @text.setter
    def text(self, value: str) -> None:
        _c.button_set_text(self._handle, value)


class TextInput(Control):
    """Text input control."""

    _on_change: ControlChangeCallback | None
    _on_submit: ControlSubmitCallback | None
    _change_adapter: Callable[[int, CallbackValue], None] | None
    _submit_adapter: Callable[[int, CallbackValue], None] | None

    def __init__(
        self,
        text: str,
        capacity: int,
        placeholder: str,
        input_type: int,
        theme: Theme,
        on_change: ControlChangeCallback | None = None,
        on_submit: ControlSubmitCallback | None = None,
    ) -> None:
        super().__init__()
        self._on_change = on_change
        self._on_submit = on_submit
        self._change_adapter = None
        self._submit_adapter = None

        if on_change is not None:

            def _change(_sender: int, value: CallbackValue) -> None:
                on_change(self, value)

            self._change_adapter = _change

        if on_submit is not None:

            def _submit(_sender: int, value: CallbackValue) -> None:
                on_submit(self, value)

            self._submit_adapter = _submit

        self._handle = _c.create_text_input(
            text,
            capacity,
            placeholder,
            input_type,
            theme._handle,
            self._change_adapter,
            self._submit_adapter,
        )

    @property
    def text(self) -> str:
        return _c.text_input_get_text(self._handle)

    @text.setter
    def text(self, value: str) -> None:
        _c.text_input_set_text(self._handle, value)


class ListControl(Control):
    """List control."""

    _on_change: ControlChangeCallback | None
    _on_submit: ControlSubmitCallback | None
    _change_adapter: Callable[[int, CallbackValue], None] | None
    _submit_adapter: Callable[[int, CallbackValue], None] | None

    def __init__(
        self,
        flags: int,
        options: list[str],
        theme: Theme,
        on_change: ControlChangeCallback | None = None,
        on_submit: ControlSubmitCallback | None = None,
    ) -> None:
        super().__init__()
        self._on_change = on_change
        self._on_submit = on_submit
        self._change_adapter = None
        self._submit_adapter = None

        if on_change is not None:

            def _change(_sender: int, value: CallbackValue) -> None:
                on_change(self, value)

            self._change_adapter = _change

        if on_submit is not None:

            def _submit(_sender: int, value: CallbackValue) -> None:
                on_submit(self, value)

            self._submit_adapter = _submit

        self._handle = _c.create_list(
            flags,
            options,
            theme._handle,
            self._change_adapter,
            self._submit_adapter,
        )

    def set_selected(self, index: int, selected: bool) -> None:
        """Set selection state for an item index."""

        _c.list_set_selected(self._handle, index, selected)

    @property
    def selected_index(self) -> int:
        """Selected item index for single-select mode."""

        return _c.list_get_selected(self._handle)


class Dropdown(Control):
    """Dropdown control."""

    _on_submit: ControlSubmitCallback | None
    _submit_adapter: Callable[[int, CallbackValue], None] | None

    def __init__(
        self,
        options: list[str],
        selected_index: int,
        theme: Theme,
        on_submit: ControlSubmitCallback | None = None,
    ) -> None:
        super().__init__()
        self._on_submit = on_submit
        self._submit_adapter = None

        if on_submit is not None:

            def _submit(_sender: int, value: CallbackValue) -> None:
                on_submit(self, value)

            self._submit_adapter = _submit

        self._handle = _c.create_dropdown(options, selected_index, theme._handle, self._submit_adapter)

    @property
    def selected_index(self) -> int:
        return _c.dropdown_get_selected(self._handle)

    @selected_index.setter
    def selected_index(self, value: int) -> None:
        _c.dropdown_set_selected(self._handle, value)


class Checkbox(Control):
    """Checkbox control."""

    _on_check: ControlCheckCallback | None
    _check_adapter: Callable[[int, bool], None] | None

    def __init__(
        self,
        text: str,
        flags: int,
        theme: Theme,
        on_check: ControlCheckCallback | None = None,
    ) -> None:
        super().__init__()
        self._on_check = on_check
        self._check_adapter = None

        if on_check is not None:

            def _check(_sender: int, value: bool) -> None:
                on_check(self, value)

            self._check_adapter = _check

        self._handle = _c.create_checkbox(text, flags, theme._handle, self._check_adapter)

    @property
    def checked(self) -> bool:
        return bool(_c.checkbox_get_checked(self._handle))

    @checked.setter
    def checked(self, value: bool) -> None:
        _c.checkbox_set_checked(self._handle, bool(value))


class NumberInput(Control):
    """Numeric input control."""

    _on_change: ControlChangeCallback | None
    _on_submit: ControlSubmitCallback | None
    _change_adapter: Callable[[int, CallbackValue], None] | None
    _submit_adapter: Callable[[int, CallbackValue], None] | None

    def __init__(
        self,
        value: float,
        min_value: float,
        max_value: float,
        increment: float,
        decimal_places: int,
        theme: Theme,
        on_change: ControlChangeCallback | None = None,
        on_submit: ControlSubmitCallback | None = None,
    ) -> None:
        super().__init__()
        self._on_change = on_change
        self._on_submit = on_submit
        self._change_adapter = None
        self._submit_adapter = None

        if on_change is not None:

            def _change(_sender: int, cb_value: CallbackValue) -> None:
                on_change(self, cb_value)

            self._change_adapter = _change

        if on_submit is not None:

            def _submit(_sender: int, cb_value: CallbackValue) -> None:
                on_submit(self, cb_value)

            self._submit_adapter = _submit

        self._handle = _c.create_number_input(
            value,
            min_value,
            max_value,
            increment,
            decimal_places,
            theme._handle,
            self._change_adapter,
            self._submit_adapter,
        )

    @property
    def value(self) -> float:
        return float(_c.number_input_get_value(self._handle))

    @value.setter
    def value(self, new_value: float) -> None:
        _c.number_input_set_value(self._handle, new_value)


class ProgressBar(Control):
    """Progress bar control."""

    def __init__(self, theme: Theme) -> None:
        super().__init__()
        self._handle = _c.create_progress_bar(theme._handle)

    @property
    def value(self) -> float:
        return float(_c.progress_bar_get_value(self._handle))

    @value.setter
    def value(self, new_value: float) -> None:
        _c.progress_bar_set_value(self._handle, new_value)

    @property
    def direction(self) -> int:
        return int(_c.progress_bar_get_direction(self._handle))

    @direction.setter
    def direction(self, value: int) -> None:
        _c.progress_bar_set_direction(self._handle, value)


class Image:
    """Mutable off-screen image made of text characters and color pairs."""

    _handle: object

    def __init__(self, width: int, height: int) -> None:
        self._handle = _c.image_create(width, height)

    @property
    def extent(self) -> tuple[int, int]:
        """Current image size as (width, height)."""

        return _c.image_get_extent(self._handle)

    def resize(self, width: int, height: int) -> None:
        """Resize image to a new extent."""

        _c.image_resize(self._handle, width, height)

    def set_pixel(self, x: int, y: int, char: str, foreground: int, background: int) -> None:
        """Set one pixel at (x, y)."""

        _c.image_set_pixel(self._handle, x, y, char, foreground, background)

    def get_pixel(self, x: int, y: int) -> tuple[str, int, int]:
        """Get one pixel at (x, y) as (char, foreground, background)."""

        return _c.image_get_pixel(self._handle, x, y)

    def draw_string(self, x: int, y: int, text: str, color_pair: int) -> None:
        """Draw text starting at (x, y) with packed color pair."""

        _c.image_draw_string(self._handle, x, y, text, color_pair)

    def clear(self, char: str = " ", foreground: int = COLOR_DEFAULT_FOREGROUND, background: int = COLOR_DEFAULT_BACKGROUND) -> None:
        """Clear entire image with one repeated pixel."""

        _c.image_clear(self._handle, char, foreground, background)


class Surface:
    """Presentable surface that owns an image and printable terminal text."""

    _handle: object

    def __init__(self, width: int, height: int) -> None:
        self._handle = _c.surface_create(width, height)

    @property
    def extent(self) -> tuple[int, int]:
        """Current surface size as (width, height)."""

        return _c.surface_get_extent(self._handle)

    def resize(self, width: int, height: int) -> None:
        """Resize the surface and rebuild backing buffers."""

        _c.surface_resize(self._handle, width, height)

    def set_pixel(self, x: int, y: int, char: str, foreground: int, background: int) -> None:
        """Set one surface image pixel at (x, y)."""

        _c.surface_set_pixel(self._handle, x, y, char, foreground, background)

    def get_pixel(self, x: int, y: int) -> tuple[str, int, int]:
        """Get one surface image pixel as (char, foreground, background)."""

        return _c.surface_get_pixel(self._handle, x, y)

    def draw_string(self, x: int, y: int, text: str, color_pair: int) -> None:
        """Draw text into the surface image."""

        _c.surface_draw_string(self._handle, x, y, text, color_pair)

    def clear(self, char: str = " ", foreground: int = COLOR_DEFAULT_FOREGROUND, background: int = COLOR_DEFAULT_BACKGROUND) -> None:
        """Clear surface image with one repeated pixel."""

        _c.surface_clear(self._handle, char, foreground, background)

    def refresh(self) -> None:
        """Refresh printable terminal text from surface image."""

        _c.surface_refresh(self._handle)

    def present(self) -> None:
        """Present current surface text to terminal output."""

        _c.window_present(self._handle)
