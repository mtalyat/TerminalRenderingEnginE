import PyTREE as _c


CONTROL_LINK_SINGLE = _c.CONTROL_LINK_SINGLE
CONTROL_LINK_DOUBLE = _c.CONTROL_LINK_DOUBLE

DIRECTION_NONE = _c.DIRECTION_NONE
DIRECTION_EAST = _c.DIRECTION_EAST
DIRECTION_NORTH = _c.DIRECTION_NORTH
DIRECTION_WEST = _c.DIRECTION_WEST
DIRECTION_SOUTH = _c.DIRECTION_SOUTH

ALIGNMENT_NONE = _c.ALIGNMENT_NONE
ALIGNMENT_TOPLEFT = _c.ALIGNMENT_TOPLEFT
ALIGNMENT_TOPRIGHT = _c.ALIGNMENT_TOPRIGHT
ALIGNMENT_LEFTSTRETCH = _c.ALIGNMENT_LEFTSTRETCH

TEXT_INPUT_TYPE_NONE = _c.CONTROL_TEXT_INPUT_TYPE_NONE
TEXT_INPUT_TYPE_NORMAL = _c.CONTROL_TEXT_INPUT_TYPE_NORMAL
TEXT_INPUT_TYPE_PASSWORD = _c.CONTROL_TEXT_INPUT_TYPE_PASSWORD

LIST_FLAGS_NONE = _c.CONTROL_LIST_FLAGS_NONE
LIST_FLAGS_MULTISELECT = _c.CONTROL_LIST_FLAGS_MULTISELECT

CHECKBOX_FLAGS_NONE = _c.CONTROL_CHECKBOX_FLAGS_NONE
CHECKBOX_FLAGS_CHECKED = _c.CONTROL_CHECKBOX_FLAGS_CHECKED
CHECKBOX_FLAGS_REVERSE = _c.CONTROL_CHECKBOX_FLAGS_REVERSE
CHECKBOX_FLAGS_RADIO = _c.CONTROL_CHECKBOX_FLAGS_RADIO


def init():
    _c.init()


def deinit():
    _c.deinit()


def time_now():
    return _c.time_now()


def set_window_title(title):
    _c.window_set_title(title)


class Theme:
    def __init__(self):
        self._handle = _c.theme_create()


class Application:
    def __init__(self, capacity=32, on_event=None):
        self._controls = []
        self._pages = {}
        self._current_page = None
        self._on_event = on_event
        self._event_adapter = None
        if on_event is not None:
            def _adapter(event):
                self._on_event(self, event)
            self._event_adapter = _adapter
        self._handle = _c.application_create(capacity, self._event_adapter)

    def add_control(self, control):
        _c.application_add_control(self._handle, control._handle)
        self._controls.append(control)
        control._application = self
        return self

    def clear_controls(self):
        _c.application_clear_controls(self._handle)
        self._controls = []
        return self

    def register_page(self, name, controls):
        self._pages[name] = list(controls)
        return self

    def show_page(self, name):
        if name not in self._pages:
            raise KeyError(f"Unknown page: {name}")

        self.clear_controls()
        for control in self._pages[name]:
            self.add_control(control)
        self._current_page = name
        return self

    def run(self):
        _c.application_run(self._handle)

    def quit(self):
        _c.application_quit(self._handle)


class Control:
    def __init__(self):
        self._handle = None
        self._application = None

    def set_transform(self, x=None, y=None, width=None, height=None, alignment=None):
        kwargs = {}
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

    def link(self, direction, other, link=CONTROL_LINK_DOUBLE):
        _c.control_link(self._handle, direction, link, other._handle)
        return self


class Label(Control):
    def __init__(self, text, theme):
        super().__init__()
        self._handle = _c.create_label(text, theme._handle)


class Button(Control):
    def __init__(self, text, theme, on_submit=None):
        super().__init__()
        self._on_submit = on_submit
        self._submit_adapter = None

        if on_submit is not None:
            def _adapter(_sender, value):
                self._on_submit(self, value)
            self._submit_adapter = _adapter

        self._handle = _c.create_button(text, theme._handle, self._submit_adapter)


class TextInput(Control):
    def __init__(self, text, capacity, placeholder, input_type, theme, on_change=None, on_submit=None):
        super().__init__()
        self._on_change = on_change
        self._on_submit = on_submit
        self._change_adapter = None
        self._submit_adapter = None

        if on_change is not None:
            def _change(_sender, value):
                self._on_change(self, value)
            self._change_adapter = _change

        if on_submit is not None:
            def _submit(_sender, value):
                self._on_submit(self, value)
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


class ListControl(Control):
    def __init__(self, flags, options, theme, on_change=None, on_submit=None):
        super().__init__()
        self._on_change = on_change
        self._on_submit = on_submit
        self._change_adapter = None
        self._submit_adapter = None

        if on_change is not None:
            def _change(_sender, value):
                self._on_change(self, value)
            self._change_adapter = _change

        if on_submit is not None:
            def _submit(_sender, value):
                self._on_submit(self, value)
            self._submit_adapter = _submit

        self._handle = _c.create_list(
            flags,
            options,
            theme._handle,
            self._change_adapter,
            self._submit_adapter,
        )


class Dropdown(Control):
    def __init__(self, options, selected_index, theme, on_submit=None):
        super().__init__()
        self._on_submit = on_submit
        self._submit_adapter = None

        if on_submit is not None:
            def _submit(_sender, value):
                self._on_submit(self, value)
            self._submit_adapter = _submit

        self._handle = _c.create_dropdown(options, selected_index, theme._handle, self._submit_adapter)


class Checkbox(Control):
    def __init__(self, text, flags, theme, on_check=None):
        super().__init__()
        self._on_check = on_check
        self._check_adapter = None

        if on_check is not None:
            def _check(_sender, value):
                self._on_check(self, value)
            self._check_adapter = _check

        self._handle = _c.create_checkbox(text, flags, theme._handle, self._check_adapter)


class NumberInput(Control):
    def __init__(self, value, min_value, max_value, increment, decimal_places, theme, on_change=None, on_submit=None):
        super().__init__()
        self._on_change = on_change
        self._on_submit = on_submit
        self._change_adapter = None
        self._submit_adapter = None

        if on_change is not None:
            def _change(_sender, cb_value):
                self._on_change(self, cb_value)
            self._change_adapter = _change

        if on_submit is not None:
            def _submit(_sender, cb_value):
                self._on_submit(self, cb_value)
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
    def value(self):
        return _c.number_input_get_value(self._handle)


class ProgressBar(Control):
    def __init__(self, theme):
        super().__init__()
        self._handle = _c.create_progress_bar(theme._handle)

    @property
    def value(self):
        return None

    @value.setter
    def value(self, new_value):
        _c.progress_bar_set_value(self._handle, new_value)
