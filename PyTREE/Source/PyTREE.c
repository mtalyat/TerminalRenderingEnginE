#include "TREE.h"
#include <Python.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef enum _PyTREE_ControlKind
{
    PYTREE_CONTROL_LABEL = 1,
    PYTREE_CONTROL_BUTTON,
    PYTREE_CONTROL_TEXT_INPUT,
    PYTREE_CONTROL_LIST,
    PYTREE_CONTROL_DROPDOWN,
    PYTREE_CONTROL_CHECKBOX,
    PYTREE_CONTROL_NUMBER_INPUT,
    PYTREE_CONTROL_PROGRESS_BAR,
} PyTREE_ControlKind;

typedef struct _PyTREE_ThemeWrapper
{
    TREE_Theme theme;
    TREE_Bool alive;
} PyTREE_ThemeWrapper;

typedef struct _PyTREE_ApplicationWrapper
{
    TREE_Application app;
    TREE_Bool alive;
    PyObject* on_event;
    struct _PyTREE_ApplicationWrapper* next;
} PyTREE_ApplicationWrapper;

typedef struct _PyTREE_ControlWrapper
{
    TREE_Control control;
    TREE_Bool alive;
    PyTREE_ControlKind kind;
    PyObject* on_submit;
    PyObject* on_change;
    PyObject* on_check;
    TREE_Char** ownedOptions;
    TREE_Size ownedOptionsSize;

    union
    {
        TREE_Control_LabelData label;
        TREE_Control_ButtonData button;
        TREE_Control_TextInputData textInput;
        TREE_Control_ListData list;
        TREE_Control_DropdownData dropdown;
        TREE_Control_CheckboxData checkbox;
        TREE_Control_NumberInputData numberInput;
        TREE_Control_ProgressBarData progressBar;
    } data;

    struct _PyTREE_ControlWrapper* next;
} PyTREE_ControlWrapper;

static PyTREE_ControlWrapper* g_controls = NULL;
static PyTREE_ApplicationWrapper* g_apps = NULL;

static PyObject* raise_tree_result(TREE_Result result)
{
    PyErr_SetString(PyExc_RuntimeError, TREE_Result_ToString(result));
    return NULL;
}

static char* pytree_strdup(const char* text)
{
    if (!text)
    {
        return NULL;
    }
    {
        size_t size = strlen(text) + 1;
        char* copy = (char*)malloc(size);
        if (!copy)
        {
            return NULL;
        }
        memcpy(copy, text, size);
        return copy;
    }
}

static void free_owned_options(TREE_Char** options, TREE_Size count)
{
    TREE_Size i;
    if (!options)
    {
        return;
    }
    for (i = 0; i < count; i++)
    {
        free(options[i]);
    }
    free(options);
}

static TREE_Result pyseq_to_cstrings(PyObject* sequence, TREE_Char*** outOptions, TREE_Size* outCount)
{
    PyObject* seqFast = NULL;
    TREE_Char** options = NULL;
    TREE_Size i;
    Py_ssize_t count;

    if (!PySequence_Check(sequence))
    {
        return TREE_ERROR_ARG_INVALID;
    }

    seqFast = PySequence_Fast(sequence, "options must be a sequence of strings");
    if (!seqFast)
    {
        return TREE_ERROR_ARG_INVALID;
    }

    count = PySequence_Fast_GET_SIZE(seqFast);
    options = (TREE_Char**)calloc((size_t)count, sizeof(TREE_Char*));
    if (!options)
    {
        Py_DECREF(seqFast);
        return TREE_ERROR_ALLOC;
    }

    for (i = 0; i < (TREE_Size)count; i++)
    {
        PyObject* item = PySequence_Fast_GET_ITEM(seqFast, (Py_ssize_t)i);
        const char* text;
        if (!PyUnicode_Check(item))
        {
            free_owned_options(options, (TREE_Size)count);
            Py_DECREF(seqFast);
            return TREE_ERROR_ARG_INVALID;
        }
        text = PyUnicode_AsUTF8(item);
        if (!text)
        {
            free_owned_options(options, (TREE_Size)count);
            Py_DECREF(seqFast);
            return TREE_ERROR_ARG_INVALID;
        }
        options[i] = pytree_strdup(text);
        if (!options[i])
        {
            free_owned_options(options, (TREE_Size)count);
            Py_DECREF(seqFast);
            return TREE_ERROR_ALLOC;
        }
    }

    Py_DECREF(seqFast);
    *outOptions = options;
    *outCount = (TREE_Size)count;
    return TREE_OK;
}

static int validate_callback(PyObject* callback)
{
    if (callback == Py_None)
    {
        return 1;
    }
    if (!PyCallable_Check(callback))
    {
        PyErr_SetString(PyExc_TypeError, "callback must be callable or None");
        return 0;
    }
    return 1;
}

static PyTREE_ThemeWrapper* get_theme_wrapper(PyObject* obj)
{
    PyTREE_ThemeWrapper* wrapper = (PyTREE_ThemeWrapper*)PyCapsule_GetPointer(obj, "PyTREE.Theme");
    if (!wrapper)
    {
        return NULL;
    }
    if (!wrapper->alive)
    {
        PyErr_SetString(PyExc_RuntimeError, "theme is already freed");
        return NULL;
    }
    return wrapper;
}

typedef struct _PyTREE_ImageWrapper
{
    TREE_Image image;
    TREE_Bool alive;
} PyTREE_ImageWrapper;

typedef struct _PyTREE_SurfaceWrapper
{
    TREE_Surface surface;
    TREE_Bool alive;
} PyTREE_SurfaceWrapper;

static PyTREE_ApplicationWrapper* get_application_wrapper(PyObject* obj)
{
    PyTREE_ApplicationWrapper* wrapper = (PyTREE_ApplicationWrapper*)PyCapsule_GetPointer(obj, "PyTREE.Application");
    if (!wrapper)
    {
        return NULL;
    }
    if (!wrapper->alive)
    {
        PyErr_SetString(PyExc_RuntimeError, "application is already freed");
        return NULL;
    }
    return wrapper;
}

static PyTREE_ImageWrapper* get_image_wrapper(PyObject* obj)
{
    PyTREE_ImageWrapper* wrapper = (PyTREE_ImageWrapper*)PyCapsule_GetPointer(obj, "PyTREE.Image");
    if (!wrapper)
    {
        return NULL;
    }
    if (!wrapper->alive)
    {
        PyErr_SetString(PyExc_RuntimeError, "image is already freed");
        return NULL;
    }
    return wrapper;
}

static PyTREE_SurfaceWrapper* get_surface_wrapper(PyObject* obj)
{
    PyTREE_SurfaceWrapper* wrapper = (PyTREE_SurfaceWrapper*)PyCapsule_GetPointer(obj, "PyTREE.Surface");
    if (!wrapper)
    {
        return NULL;
    }
    if (!wrapper->alive)
    {
        PyErr_SetString(PyExc_RuntimeError, "surface is already freed");
        return NULL;
    }
    return wrapper;
}

static PyTREE_ControlWrapper* get_control_wrapper(PyObject* obj)
{
    PyTREE_ControlWrapper* wrapper = (PyTREE_ControlWrapper*)PyCapsule_GetPointer(obj, "PyTREE.Control");
    if (!wrapper)
    {
        return NULL;
    }
    if (!wrapper->alive)
    {
        PyErr_SetString(PyExc_RuntimeError, "control is already freed");
        return NULL;
    }
    return wrapper;
}

static int ensure_control_kind(PyTREE_ControlWrapper* control, PyTREE_ControlKind kind, const char* apiName)
{
    if (control->kind != kind)
    {
        PyErr_Format(PyExc_TypeError, "%s called with incompatible control type", apiName);
        return 0;
    }
    return 1;
}

static void register_control(PyTREE_ControlWrapper* wrapper)
{
    wrapper->next = g_controls;
    g_controls = wrapper;
}

static void unregister_control(PyTREE_ControlWrapper* wrapper)
{
    PyTREE_ControlWrapper** current = &g_controls;
    while (*current)
    {
        if (*current == wrapper)
        {
            *current = wrapper->next;
            return;
        }
        current = &((*current)->next);
    }
}

static PyTREE_ControlWrapper* find_control_wrapper(TREE_Control* control)
{
    PyTREE_ControlWrapper* current = g_controls;
    while (current)
    {
        if (&current->control == control)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static void register_app(PyTREE_ApplicationWrapper* wrapper)
{
    wrapper->next = g_apps;
    g_apps = wrapper;
}

static void unregister_app(PyTREE_ApplicationWrapper* wrapper)
{
    PyTREE_ApplicationWrapper** current = &g_apps;
    while (*current)
    {
        if (*current == wrapper)
        {
            *current = wrapper->next;
            return;
        }
        current = &((*current)->next);
    }
}

static PyTREE_ApplicationWrapper* find_app_wrapper(TREE_Application* app)
{
    PyTREE_ApplicationWrapper* current = g_apps;
    while (current)
    {
        if (&current->app == app)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static PyObject* convert_control_callback_value(PyTREE_ControlWrapper* wrapper, const void* value)
{
    if (!value)
    {
        Py_RETURN_NONE;
    }

    switch (wrapper->kind)
    {
    case PYTREE_CONTROL_TEXT_INPUT:
    {
        TREE_Char* text = *((TREE_Char* const*)value);
        if (!text)
        {
            Py_RETURN_NONE;
        }
        return PyUnicode_FromString(text);
    }
    case PYTREE_CONTROL_LIST:
    {
        TREE_Control_ListData* data = &wrapper->data.list;
        if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
        {
            PyObject* selected = PyList_New(0);
            TREE_Size i;
            if (!selected)
            {
                return NULL;
            }
            for (i = 0; i < data->optionsSize; i++)
            {
                if (data->selectedIndices && data->selectedIndices[i])
                {
                    PyObject* idx = PyLong_FromUnsignedLongLong(i);
                    if (!idx || PyList_Append(selected, idx) < 0)
                    {
                        Py_XDECREF(idx);
                        Py_DECREF(selected);
                        return NULL;
                    }
                    Py_DECREF(idx);
                }
            }
            return selected;
        }
        return PyLong_FromUnsignedLongLong(*(const TREE_Size*)value);
    }
    case PYTREE_CONTROL_DROPDOWN:
        return PyLong_FromUnsignedLongLong(*(const TREE_Size*)value);
    case PYTREE_CONTROL_CHECKBOX:
        if (*(const TREE_Byte*)value)
        {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    case PYTREE_CONTROL_NUMBER_INPUT:
        return PyFloat_FromDouble(*(const TREE_Float*)value);
    default:
        Py_RETURN_NONE;
    }
}

typedef enum _PyTREE_CallbackKind
{
    PYTREE_CB_SUBMIT,
    PYTREE_CB_CHANGE,
    PYTREE_CB_CHECK,
} PyTREE_CallbackKind;

static void invoke_control_callback(void* sender, void const* value, PyTREE_CallbackKind cbKind)
{
    PyTREE_ControlWrapper* wrapper = find_control_wrapper((TREE_Control*)sender);
    PyObject* callback = NULL;
    PyGILState_STATE gil;
    PyObject* pySender = NULL;
    PyObject* pyValue = NULL;
    PyObject* pyResult = NULL;

    if (!wrapper)
    {
        return;
    }

    switch (cbKind)
    {
    case PYTREE_CB_SUBMIT:
        callback = wrapper->on_submit;
        break;
    case PYTREE_CB_CHANGE:
        callback = wrapper->on_change;
        break;
    case PYTREE_CB_CHECK:
        callback = wrapper->on_check;
        break;
    }

    if (!callback)
    {
        return;
    }

    gil = PyGILState_Ensure();

    pySender = PyLong_FromVoidPtr(sender);
    pyValue = convert_control_callback_value(wrapper, value);
    if (pySender && pyValue)
    {
        pyResult = PyObject_CallFunctionObjArgs(callback, pySender, pyValue, NULL);
        if (!pyResult)
        {
            PyErr_Print();
        }
    }
    Py_XDECREF(pyResult);
    Py_XDECREF(pyValue);
    Py_XDECREF(pySender);

    PyGILState_Release(gil);
}

static void control_submit_callback(void* sender, void const* value)
{
    invoke_control_callback(sender, value, PYTREE_CB_SUBMIT);
}

static void control_change_callback(void* sender, void const* value)
{
    invoke_control_callback(sender, value, PYTREE_CB_CHANGE);
}

static void control_check_callback(void* sender, void const* value)
{
    invoke_control_callback(sender, value, PYTREE_CB_CHECK);
}

static TREE_Result app_event_callback(TREE_Event const* event)
{
    PyTREE_ApplicationWrapper* wrapper = find_app_wrapper(event->application);
    PyGILState_STATE gil;
    PyObject* eventDict;
    PyObject* pyResult;

    if (!wrapper || !wrapper->on_event)
    {
        return TREE_OK;
    }

    gil = PyGILState_Ensure();

    eventDict = PyDict_New();
    if (!eventDict)
    {
        PyErr_Print();
        PyGILState_Release(gil);
        return TREE_OK;
    }

    PyDict_SetItemString(eventDict, "type", PyLong_FromLong(event->type));
    PyDict_SetItemString(eventDict, "control", PyLong_FromVoidPtr(event->control));

    if ((event->type == TREE_EVENT_TYPE_KEY_DOWN || event->type == TREE_EVENT_TYPE_KEY_HELD || event->type == TREE_EVENT_TYPE_KEY_UP) && event->data)
    {
        TREE_EventData_Key const* key = (TREE_EventData_Key const*)event->data;
        PyDict_SetItemString(eventDict, "key", PyLong_FromLong(key->key));
        PyDict_SetItemString(eventDict, "modifiers", PyLong_FromLong(key->modifiers));
    }
    else if (event->type == TREE_EVENT_TYPE_WINDOW_RESIZE && event->data)
    {
        TREE_EventData_WindowResize const* resize = (TREE_EventData_WindowResize const*)event->data;
        PyDict_SetItemString(eventDict, "width", PyLong_FromLong(resize->extent.width));
        PyDict_SetItemString(eventDict, "height", PyLong_FromLong(resize->extent.height));
    }

    pyResult = PyObject_CallFunctionObjArgs(wrapper->on_event, eventDict, NULL);
    if (!pyResult)
    {
        PyErr_Print();
    }
    Py_XDECREF(pyResult);
    Py_DECREF(eventDict);

    PyGILState_Release(gil);
    return TREE_OK;
}

static void theme_capsule_destructor(PyObject* capsule)
{
    PyTREE_ThemeWrapper* wrapper = (PyTREE_ThemeWrapper*)PyCapsule_GetPointer(capsule, "PyTREE.Theme");
    if (!wrapper)
    {
        return;
    }
    if (wrapper->alive)
    {
        TREE_Theme_Free(&wrapper->theme);
        wrapper->alive = TREE_FALSE;
    }
    free(wrapper);
}

static void app_capsule_destructor(PyObject* capsule)
{
    PyTREE_ApplicationWrapper* wrapper = (PyTREE_ApplicationWrapper*)PyCapsule_GetPointer(capsule, "PyTREE.Application");
    if (!wrapper)
    {
        return;
    }
    unregister_app(wrapper);
    if (wrapper->alive)
    {
        TREE_Application_Free(&wrapper->app);
        wrapper->alive = TREE_FALSE;
    }
    Py_XDECREF(wrapper->on_event);
    free(wrapper);
}

static void image_capsule_destructor(PyObject* capsule)
{
    PyTREE_ImageWrapper* wrapper = (PyTREE_ImageWrapper*)PyCapsule_GetPointer(capsule, "PyTREE.Image");
    if (!wrapper)
    {
        return;
    }
    if (wrapper->alive)
    {
        TREE_Image_Free(&wrapper->image);
        wrapper->alive = TREE_FALSE;
    }
    free(wrapper);
}

static void surface_capsule_destructor(PyObject* capsule)
{
    PyTREE_SurfaceWrapper* wrapper = (PyTREE_SurfaceWrapper*)PyCapsule_GetPointer(capsule, "PyTREE.Surface");
    if (!wrapper)
    {
        return;
    }
    if (wrapper->alive)
    {
        TREE_Surface_Free(&wrapper->surface);
        wrapper->alive = TREE_FALSE;
    }
    free(wrapper);
}

static void control_capsule_destructor(PyObject* capsule)
{
    PyTREE_ControlWrapper* wrapper = (PyTREE_ControlWrapper*)PyCapsule_GetPointer(capsule, "PyTREE.Control");
    if (!wrapper)
    {
        return;
    }
    unregister_control(wrapper);
    if (wrapper->alive)
    {
        TREE_Control_Free(&wrapper->control);
        wrapper->alive = TREE_FALSE;
    }
    Py_XDECREF(wrapper->on_submit);
    Py_XDECREF(wrapper->on_change);
    Py_XDECREF(wrapper->on_check);
    free_owned_options(wrapper->ownedOptions, wrapper->ownedOptionsSize);
    free(wrapper);
}

static PyObject* PyTREE_Init(PyObject* self, PyObject* args)
{
    TREE_Result result;
    (void)self;
    (void)args;

    result = TREE_Init();
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Deinit(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    TREE_Free();
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Time_Now(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    return PyLong_FromLongLong(TREE_Time_Now());
}

static PyObject* PyTREE_Result_ToString_Wrap(PyObject* self, PyObject* args)
{
    int code;
    (void)self;
    if (!PyArg_ParseTuple(args, "i", &code))
    {
        return NULL;
    }
    return PyUnicode_FromString(TREE_Result_ToString((TREE_Result)code));
}

static PyObject* PyTREE_Window_SetTitle_Wrap(PyObject* self, PyObject* args)
{
    const char* title;
    TREE_Result result;
    (void)self;
    if (!PyArg_ParseTuple(args, "s", &title))
    {
        return NULL;
    }
    result = TREE_Window_SetTitle(title);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Window_GetExtent_Wrap(PyObject* self, PyObject* args)
{
    TREE_Extent extent;
    (void)self;
    (void)args;

    extent = TREE_Window_GetExtent();
    return Py_BuildValue("ii", extent.width, extent.height);
}

static PyObject* PyTREE_Clipboard_SetText_Wrap(PyObject* self, PyObject* args)
{
    const char* text;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "s", &text))
    {
        return NULL;
    }

    result = TREE_Clipboard_SetText(text);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Clipboard_GetText_Wrap(PyObject* self, PyObject* args)
{
    TREE_Char* text = NULL;
    TREE_Result result;
    PyObject* out;
    (void)self;
    (void)args;

    result = TREE_Clipboard_GetText(&text);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    if (!text)
    {
        Py_RETURN_NONE;
    }

    out = PyUnicode_FromString(text);
    free(text);
    return out;
}

static PyObject* PyTREE_Image_Create(PyObject* self, PyObject* args)
{
    int width;
    int height;
    TREE_Extent extent;
    TREE_Result result;
    PyTREE_ImageWrapper* wrapper;
    PyObject* capsule;
    (void)self;

    if (!PyArg_ParseTuple(args, "ii", &width, &height))
    {
        return NULL;
    }
    if (width <= 0 || height <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "width and height must be > 0");
        return NULL;
    }

    wrapper = (PyTREE_ImageWrapper*)calloc(1, sizeof(PyTREE_ImageWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }

    extent.width = width;
    extent.height = height;
    result = TREE_Image_Init(&wrapper->image, extent);
    if (result != TREE_OK)
    {
        free(wrapper);
        return raise_tree_result(result);
    }
    wrapper->alive = TREE_TRUE;

    capsule = PyCapsule_New(wrapper, "PyTREE.Image", image_capsule_destructor);
    if (!capsule)
    {
        TREE_Image_Free(&wrapper->image);
        free(wrapper);
        return NULL;
    }

    return capsule;
}

static PyObject* PyTREE_Image_GetExtent_Wrap(PyObject* self, PyObject* args)
{
    PyObject* imageObj;
    PyTREE_ImageWrapper* image;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &imageObj))
    {
        return NULL;
    }

    image = get_image_wrapper(imageObj);
    if (!image)
    {
        return NULL;
    }

    return Py_BuildValue("ii", image->image.extent.width, image->image.extent.height);
}

static PyObject* PyTREE_Image_Resize_Wrap(PyObject* self, PyObject* args)
{
    PyObject* imageObj;
    int width;
    int height;
    TREE_Extent extent;
    PyTREE_ImageWrapper* image;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oii", &imageObj, &width, &height))
    {
        return NULL;
    }
    if (width <= 0 || height <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "width and height must be > 0");
        return NULL;
    }

    image = get_image_wrapper(imageObj);
    if (!image)
    {
        return NULL;
    }

    extent.width = width;
    extent.height = height;
    result = TREE_Image_Resize(&image->image, extent);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Image_SetPixel_Wrap(PyObject* self, PyObject* args)
{
    PyObject* imageObj;
    int x;
    int y;
    PyObject* charObj;
    int foreground;
    int background;
    const char* text;
    Py_ssize_t textLen;
    TREE_Offset offset;
    TREE_Pixel pixel;
    PyTREE_ImageWrapper* image;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiiOii", &imageObj, &x, &y, &charObj, &foreground, &background))
    {
        return NULL;
    }
    if (!PyUnicode_Check(charObj))
    {
        PyErr_SetString(PyExc_TypeError, "char must be a string");
        return NULL;
    }

    text = PyUnicode_AsUTF8AndSize(charObj, &textLen);
    if (!text)
    {
        return NULL;
    }
    if (textLen < 1)
    {
        PyErr_SetString(PyExc_ValueError, "char string must not be empty");
        return NULL;
    }

    image = get_image_wrapper(imageObj);
    if (!image)
    {
        return NULL;
    }

    offset.x = x;
    offset.y = y;
    pixel = TREE_Pixel_Create(text[0], (TREE_Color)foreground, (TREE_Color)background);
    result = TREE_Image_Set(&image->image, offset, pixel);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Image_GetPixel_Wrap(PyObject* self, PyObject* args)
{
    PyObject* imageObj;
    int x;
    int y;
    TREE_Offset offset;
    TREE_Pixel pixel;
    TREE_Color fg;
    TREE_Color bg;
    TREE_Char ch;
    PyTREE_ImageWrapper* image;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oii", &imageObj, &x, &y))
    {
        return NULL;
    }

    image = get_image_wrapper(imageObj);
    if (!image)
    {
        return NULL;
    }

    offset.x = x;
    offset.y = y;
    pixel = TREE_Image_Get(&image->image, offset);
    fg = TREE_ColorPair_GetForeground(pixel.colorPair);
    bg = TREE_ColorPair_GetBackground(pixel.colorPair);
    ch = pixel.character;
    return Py_BuildValue("Nii", PyUnicode_FromStringAndSize(&ch, 1), (int)fg, (int)bg);
}

static PyObject* PyTREE_Image_DrawString_Wrap(PyObject* self, PyObject* args)
{
    PyObject* imageObj;
    int x;
    int y;
    const char* text;
    unsigned int colorPair;
    TREE_Offset offset;
    TREE_Result result;
    PyTREE_ImageWrapper* image;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiisI", &imageObj, &x, &y, &text, &colorPair))
    {
        return NULL;
    }

    image = get_image_wrapper(imageObj);
    if (!image)
    {
        return NULL;
    }

    offset.x = x;
    offset.y = y;
    result = TREE_Image_DrawString(&image->image, offset, text, (TREE_ColorPair)colorPair);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Image_Clear_Wrap(PyObject* self, PyObject* args)
{
    PyObject* imageObj;
    PyObject* charObj;
    int foreground;
    int background;
    const char* text;
    Py_ssize_t textLen;
    TREE_Pixel pixel;
    TREE_Result result;
    PyTREE_ImageWrapper* image;
    (void)self;

    if (!PyArg_ParseTuple(args, "OOii", &imageObj, &charObj, &foreground, &background))
    {
        return NULL;
    }
    if (!PyUnicode_Check(charObj))
    {
        PyErr_SetString(PyExc_TypeError, "char must be a string");
        return NULL;
    }

    text = PyUnicode_AsUTF8AndSize(charObj, &textLen);
    if (!text)
    {
        return NULL;
    }
    if (textLen < 1)
    {
        PyErr_SetString(PyExc_ValueError, "char string must not be empty");
        return NULL;
    }

    image = get_image_wrapper(imageObj);
    if (!image)
    {
        return NULL;
    }

    pixel = TREE_Pixel_Create(text[0], (TREE_Color)foreground, (TREE_Color)background);
    result = TREE_Image_Clear(&image->image, pixel);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Surface_Create(PyObject* self, PyObject* args)
{
    int width;
    int height;
    TREE_Extent extent;
    TREE_Result result;
    PyTREE_SurfaceWrapper* wrapper;
    PyObject* capsule;
    (void)self;

    if (!PyArg_ParseTuple(args, "ii", &width, &height))
    {
        return NULL;
    }
    if (width <= 0 || height <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "width and height must be > 0");
        return NULL;
    }

    wrapper = (PyTREE_SurfaceWrapper*)calloc(1, sizeof(PyTREE_SurfaceWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }

    extent.width = width;
    extent.height = height;
    result = TREE_Surface_Init(&wrapper->surface, extent);
    if (result != TREE_OK)
    {
        free(wrapper);
        return raise_tree_result(result);
    }
    wrapper->alive = TREE_TRUE;

    capsule = PyCapsule_New(wrapper, "PyTREE.Surface", surface_capsule_destructor);
    if (!capsule)
    {
        TREE_Surface_Free(&wrapper->surface);
        free(wrapper);
        return NULL;
    }

    return capsule;
}

static PyObject* PyTREE_Surface_GetExtent_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    PyTREE_SurfaceWrapper* surface;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &surfaceObj))
    {
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    return Py_BuildValue("ii", surface->surface.image.extent.width, surface->surface.image.extent.height);
}

static PyObject* PyTREE_Surface_Resize_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    int width;
    int height;
    TREE_Extent extent;
    TREE_Result result;
    PyTREE_SurfaceWrapper* surface;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oii", &surfaceObj, &width, &height))
    {
        return NULL;
    }
    if (width <= 0 || height <= 0)
    {
        PyErr_SetString(PyExc_ValueError, "width and height must be > 0");
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    TREE_Surface_Free(&surface->surface);
    extent.width = width;
    extent.height = height;
    result = TREE_Surface_Init(&surface->surface, extent);
    if (result != TREE_OK)
    {
        surface->alive = TREE_FALSE;
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Surface_SetPixel_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    int x;
    int y;
    PyObject* charObj;
    int foreground;
    int background;
    const char* text;
    Py_ssize_t textLen;
    TREE_Offset offset;
    TREE_Pixel pixel;
    TREE_Result result;
    PyTREE_SurfaceWrapper* surface;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiiOii", &surfaceObj, &x, &y, &charObj, &foreground, &background))
    {
        return NULL;
    }
    if (!PyUnicode_Check(charObj))
    {
        PyErr_SetString(PyExc_TypeError, "char must be a string");
        return NULL;
    }

    text = PyUnicode_AsUTF8AndSize(charObj, &textLen);
    if (!text)
    {
        return NULL;
    }
    if (textLen < 1)
    {
        PyErr_SetString(PyExc_ValueError, "char string must not be empty");
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    offset.x = x;
    offset.y = y;
    pixel = TREE_Pixel_Create(text[0], (TREE_Color)foreground, (TREE_Color)background);
    result = TREE_Image_Set(&surface->surface.image, offset, pixel);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Surface_GetPixel_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    int x;
    int y;
    TREE_Offset offset;
    TREE_Pixel pixel;
    TREE_Color fg;
    TREE_Color bg;
    TREE_Char ch;
    PyTREE_SurfaceWrapper* surface;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oii", &surfaceObj, &x, &y))
    {
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    offset.x = x;
    offset.y = y;
    pixel = TREE_Image_Get(&surface->surface.image, offset);
    fg = TREE_ColorPair_GetForeground(pixel.colorPair);
    bg = TREE_ColorPair_GetBackground(pixel.colorPair);
    ch = pixel.character;
    return Py_BuildValue("Nii", PyUnicode_FromStringAndSize(&ch, 1), (int)fg, (int)bg);
}

static PyObject* PyTREE_Surface_DrawString_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    int x;
    int y;
    const char* text;
    unsigned int colorPair;
    TREE_Offset offset;
    TREE_Result result;
    PyTREE_SurfaceWrapper* surface;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiisI", &surfaceObj, &x, &y, &text, &colorPair))
    {
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    offset.x = x;
    offset.y = y;
    result = TREE_Image_DrawString(&surface->surface.image, offset, text, (TREE_ColorPair)colorPair);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Surface_Clear_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    PyObject* charObj;
    int foreground;
    int background;
    const char* text;
    Py_ssize_t textLen;
    TREE_Pixel pixel;
    TREE_Result result;
    PyTREE_SurfaceWrapper* surface;
    (void)self;

    if (!PyArg_ParseTuple(args, "OOii", &surfaceObj, &charObj, &foreground, &background))
    {
        return NULL;
    }
    if (!PyUnicode_Check(charObj))
    {
        PyErr_SetString(PyExc_TypeError, "char must be a string");
        return NULL;
    }

    text = PyUnicode_AsUTF8AndSize(charObj, &textLen);
    if (!text)
    {
        return NULL;
    }
    if (textLen < 1)
    {
        PyErr_SetString(PyExc_ValueError, "char string must not be empty");
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    pixel = TREE_Pixel_Create(text[0], (TREE_Color)foreground, (TREE_Color)background);
    result = TREE_Image_Clear(&surface->surface.image, pixel);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Surface_Refresh_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    PyTREE_SurfaceWrapper* surface;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &surfaceObj))
    {
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    result = TREE_Surface_Refresh(&surface->surface);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Window_Present_Wrap(PyObject* self, PyObject* args)
{
    PyObject* surfaceObj;
    PyTREE_SurfaceWrapper* surface;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &surfaceObj))
    {
        return NULL;
    }

    surface = get_surface_wrapper(surfaceObj);
    if (!surface)
    {
        return NULL;
    }

    result = TREE_Window_Present(&surface->surface);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Window_Beep_Wrap(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    TREE_Window_Beep();
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Cursor_SetVisible_Wrap(PyObject* self, PyObject* args)
{
    int visible;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "p", &visible))
    {
        return NULL;
    }

    result = TREE_Cursor_SetVisible((TREE_Bool)visible);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Theme_Create(PyObject* self, PyObject* args)
{
    PyTREE_ThemeWrapper* wrapper;
    PyObject* capsule;
    TREE_Result result;
    (void)self;
    (void)args;

    wrapper = (PyTREE_ThemeWrapper*)calloc(1, sizeof(PyTREE_ThemeWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }

    result = TREE_Theme_Init(&wrapper->theme);
    if (result != TREE_OK)
    {
        free(wrapper);
        return raise_tree_result(result);
    }
    wrapper->alive = TREE_TRUE;

    capsule = PyCapsule_New(wrapper, "PyTREE.Theme", theme_capsule_destructor);
    if (!capsule)
    {
        TREE_Theme_Free(&wrapper->theme);
        free(wrapper);
        return NULL;
    }

    return capsule;
}

static PyObject* PyTREE_ColorPair_Create_Wrap(PyObject* self, PyObject* args)
{
    int foreground;
    int background;
    (void)self;

    if (!PyArg_ParseTuple(args, "ii", &foreground, &background))
    {
        return NULL;
    }

    return PyLong_FromUnsignedLong((unsigned long)TREE_ColorPair_Create((TREE_Color)foreground, (TREE_Color)background));
}

static PyObject* PyTREE_ColorPair_GetForeground_Wrap(PyObject* self, PyObject* args)
{
    unsigned int colorPair;
    (void)self;

    if (!PyArg_ParseTuple(args, "I", &colorPair))
    {
        return NULL;
    }

    return PyLong_FromLong((long)TREE_ColorPair_GetForeground((TREE_ColorPair)colorPair));
}

static PyObject* PyTREE_ColorPair_GetBackground_Wrap(PyObject* self, PyObject* args)
{
    unsigned int colorPair;
    (void)self;

    if (!PyArg_ParseTuple(args, "I", &colorPair))
    {
        return NULL;
    }

    return PyLong_FromLong((long)TREE_ColorPair_GetBackground((TREE_ColorPair)colorPair));
}

static PyObject* PyTREE_Theme_SetChar(PyObject* self, PyObject* args)
{
    PyObject* themeObj;
    int cid;
    PyObject* valueObj;
    PyTREE_ThemeWrapper* theme;
    const char* text;
    Py_ssize_t textLen;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiO", &themeObj, &cid, &valueObj))
    {
        return NULL;
    }

    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    if (cid < 0 || cid >= TREE_THEME_CID_COUNT)
    {
        PyErr_SetString(PyExc_ValueError, "theme character id out of range");
        return NULL;
    }

    if (!PyUnicode_Check(valueObj))
    {
        PyErr_SetString(PyExc_TypeError, "value must be a string");
        return NULL;
    }

    text = PyUnicode_AsUTF8AndSize(valueObj, &textLen);
    if (!text)
    {
        return NULL;
    }
    if (textLen < 1)
    {
        PyErr_SetString(PyExc_ValueError, "value string must not be empty");
        return NULL;
    }

    theme->theme.characters[cid] = text[0];
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Theme_GetChar(PyObject* self, PyObject* args)
{
    PyObject* themeObj;
    int cid;
    PyTREE_ThemeWrapper* theme;
    TREE_Char value;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oi", &themeObj, &cid))
    {
        return NULL;
    }

    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    if (cid < 0 || cid >= TREE_THEME_CID_COUNT)
    {
        PyErr_SetString(PyExc_ValueError, "theme character id out of range");
        return NULL;
    }

    value = theme->theme.characters[cid];
    return PyUnicode_FromStringAndSize(&value, 1);
}

static PyObject* PyTREE_Theme_SetPixel(PyObject* self, PyObject* args)
{
    PyObject* themeObj;
    int pid;
    PyObject* charObj;
    int foreground;
    int background;
    PyTREE_ThemeWrapper* theme;
    const char* text;
    Py_ssize_t textLen;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiOii", &themeObj, &pid, &charObj, &foreground, &background))
    {
        return NULL;
    }

    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    if (pid < 0 || pid >= TREE_THEME_PID_COUNT)
    {
        PyErr_SetString(PyExc_ValueError, "theme pixel id out of range");
        return NULL;
    }

    if (!PyUnicode_Check(charObj))
    {
        PyErr_SetString(PyExc_TypeError, "char must be a string");
        return NULL;
    }

    text = PyUnicode_AsUTF8AndSize(charObj, &textLen);
    if (!text)
    {
        return NULL;
    }
    if (textLen < 1)
    {
        PyErr_SetString(PyExc_ValueError, "char string must not be empty");
        return NULL;
    }

    theme->theme.pixels[pid] = TREE_Pixel_Create(text[0], (TREE_Color)foreground, (TREE_Color)background);
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Theme_GetPixel(PyObject* self, PyObject* args)
{
    PyObject* themeObj;
    int pid;
    PyTREE_ThemeWrapper* theme;
    TREE_Pixel pixel;
    TREE_Color fg;
    TREE_Color bg;
    TREE_Char ch;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oi", &themeObj, &pid))
    {
        return NULL;
    }

    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    if (pid < 0 || pid >= TREE_THEME_PID_COUNT)
    {
        PyErr_SetString(PyExc_ValueError, "theme pixel id out of range");
        return NULL;
    }

    pixel = theme->theme.pixels[pid];
    fg = TREE_ColorPair_GetForeground(pixel.colorPair);
    bg = TREE_ColorPair_GetBackground(pixel.colorPair);
    ch = pixel.character;

    return Py_BuildValue("Nii", PyUnicode_FromStringAndSize(&ch, 1), (int)fg, (int)bg);
}

static PyObject* PyTREE_Application_Create(PyObject* self, PyObject* args)
{
    unsigned long long capacity;
    PyObject* callback = Py_None;
    PyTREE_ApplicationWrapper* wrapper;
    TREE_Result result;
    PyObject* capsule;
    TREE_EventHandler eventHandler = NULL;
    (void)self;

    if (!PyArg_ParseTuple(args, "K|O", &capacity, &callback))
    {
        return NULL;
    }

    if (!validate_callback(callback))
    {
        return NULL;
    }

    wrapper = (PyTREE_ApplicationWrapper*)calloc(1, sizeof(PyTREE_ApplicationWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }

    if (callback != Py_None)
    {
        Py_INCREF(callback);
        wrapper->on_event = callback;
        eventHandler = app_event_callback;
    }

    result = TREE_Application_Init(&wrapper->app, (TREE_Size)capacity, eventHandler);
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_event);
        free(wrapper);
        return raise_tree_result(result);
    }
    wrapper->alive = TREE_TRUE;
    register_app(wrapper);

    capsule = PyCapsule_New(wrapper, "PyTREE.Application", app_capsule_destructor);
    if (!capsule)
    {
        unregister_app(wrapper);
        TREE_Application_Free(&wrapper->app);
        Py_XDECREF(wrapper->on_event);
        free(wrapper);
        return NULL;
    }
    return capsule;
}

static PyObject* PyTREE_Application_AddControl_Wrap(PyObject* self, PyObject* args)
{
    PyObject* appObj;
    PyObject* controlObj;
    PyTREE_ApplicationWrapper* app;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "OO", &appObj, &controlObj))
    {
        return NULL;
    }

    app = get_application_wrapper(appObj);
    if (!app)
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control)
    {
        return NULL;
    }

    result = TREE_Application_AddControl(&app->app, &control->control);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Application_Run_Wrap(PyObject* self, PyObject* args)
{
    PyObject* appObj;
    PyTREE_ApplicationWrapper* app;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &appObj))
    {
        return NULL;
    }

    app = get_application_wrapper(appObj);
    if (!app)
    {
        return NULL;
    }

    result = TREE_Application_Run(&app->app);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Application_ClearControls_Wrap(PyObject* self, PyObject* args)
{
    PyObject* appObj;
    PyTREE_ApplicationWrapper* app;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &appObj))
    {
        return NULL;
    }

    app = get_application_wrapper(appObj);
    if (!app)
    {
        return NULL;
    }

    result = TREE_Application_ClearControls(&app->app);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Application_Quit_Wrap(PyObject* self, PyObject* args)
{
    PyObject* appObj;
    PyTREE_ApplicationWrapper* app;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &appObj))
    {
        return NULL;
    }

    app = get_application_wrapper(appObj);
    if (!app)
    {
        return NULL;
    }

    TREE_Application_Quit(&app->app);
    Py_RETURN_NONE;
}

static PyObject* create_control_wrapper(PyTREE_ControlWrapper* wrapper)
{
    PyObject* capsule = PyCapsule_New(wrapper, "PyTREE.Control", control_capsule_destructor);
    if (!capsule)
    {
        return NULL;
    }
    register_control(wrapper);
    return capsule;
}

static PyObject* PyTREE_Create_Label(PyObject* self, PyObject* args)
{
    const char* text;
    PyObject* themeObj;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "sO", &text, &themeObj))
    {
        return NULL;
    }

    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_LABEL;

    result = TREE_Control_LabelData_Init(&wrapper->data.label, text, &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_Label_Init(&wrapper->control, NULL, &wrapper->data.label);
    }
    if (result != TREE_OK)
    {
        TREE_Control_LabelData_Free(&wrapper->data.label);
        free(wrapper);
        return raise_tree_result(result);
    }
    wrapper->alive = TREE_TRUE;

    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_Button(PyObject* self, PyObject* args)
{
    const char* text;
    PyObject* themeObj;
    PyObject* onSubmit = Py_None;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "sO|O", &text, &themeObj, &onSubmit))
    {
        return NULL;
    }
    if (!validate_callback(onSubmit))
    {
        return NULL;
    }
    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_BUTTON;
    if (onSubmit != Py_None)
    {
        Py_INCREF(onSubmit);
        wrapper->on_submit = onSubmit;
    }

    result = TREE_Control_ButtonData_Init(&wrapper->data.button, text, wrapper->on_submit ? control_submit_callback : NULL, &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_Button_Init(&wrapper->control, NULL, &wrapper->data.button);
    }
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_submit);
        TREE_Control_ButtonData_Free(&wrapper->data.button);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_TextInput(PyObject* self, PyObject* args)
{
    const char* text;
    unsigned long long capacity;
    const char* placeholder;
    int type;
    PyObject* themeObj;
    PyObject* onChange = Py_None;
    PyObject* onSubmit = Py_None;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "sKsiO|OO", &text, &capacity, &placeholder, &type, &themeObj, &onChange, &onSubmit))
    {
        return NULL;
    }
    if (!validate_callback(onChange) || !validate_callback(onSubmit))
    {
        return NULL;
    }
    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_TEXT_INPUT;
    if (onChange != Py_None)
    {
        Py_INCREF(onChange);
        wrapper->on_change = onChange;
    }
    if (onSubmit != Py_None)
    {
        Py_INCREF(onSubmit);
        wrapper->on_submit = onSubmit;
    }

    result = TREE_Control_TextInputData_Init(
        &wrapper->data.textInput,
        text,
        (TREE_Size)capacity,
        placeholder,
        (TREE_Control_TextInputType)type,
        wrapper->on_change ? control_change_callback : NULL,
        wrapper->on_submit ? control_submit_callback : NULL,
        &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_TextInput_Init(&wrapper->control, NULL, &wrapper->data.textInput);
    }
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_change);
        Py_XDECREF(wrapper->on_submit);
        TREE_Control_TextInputData_Free(&wrapper->data.textInput);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_List(PyObject* self, PyObject* args)
{
    int flags;
    PyObject* options;
    PyObject* themeObj;
    PyObject* onChange = Py_None;
    PyObject* onSubmit = Py_None;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "iOO|OO", &flags, &options, &themeObj, &onChange, &onSubmit))
    {
        return NULL;
    }
    if (!validate_callback(onChange) || !validate_callback(onSubmit))
    {
        return NULL;
    }
    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_LIST;

    result = pyseq_to_cstrings(options, &wrapper->ownedOptions, &wrapper->ownedOptionsSize);
    if (result != TREE_OK)
    {
        free(wrapper);
        return raise_tree_result(result);
    }

    if (onChange != Py_None)
    {
        Py_INCREF(onChange);
        wrapper->on_change = onChange;
    }
    if (onSubmit != Py_None)
    {
        Py_INCREF(onSubmit);
        wrapper->on_submit = onSubmit;
    }

    result = TREE_Control_ListData_Init(
        &wrapper->data.list,
        (TREE_Control_ListFlags)flags,
        wrapper->ownedOptions,
        wrapper->ownedOptionsSize,
        wrapper->on_change ? control_change_callback : NULL,
        wrapper->on_submit ? control_submit_callback : NULL,
        &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_List_Init(&wrapper->control, NULL, &wrapper->data.list);
    }
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_change);
        Py_XDECREF(wrapper->on_submit);
        TREE_Control_ListData_Free(&wrapper->data.list);
        free_owned_options(wrapper->ownedOptions, wrapper->ownedOptionsSize);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_Dropdown(PyObject* self, PyObject* args)
{
    PyObject* options;
    unsigned long long selected;
    PyObject* themeObj;
    PyObject* onSubmit = Py_None;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "OKO|O", &options, &selected, &themeObj, &onSubmit))
    {
        return NULL;
    }
    if (!validate_callback(onSubmit))
    {
        return NULL;
    }
    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_DROPDOWN;

    result = pyseq_to_cstrings(options, &wrapper->ownedOptions, &wrapper->ownedOptionsSize);
    if (result != TREE_OK)
    {
        free(wrapper);
        return raise_tree_result(result);
    }

    if (onSubmit != Py_None)
    {
        Py_INCREF(onSubmit);
        wrapper->on_submit = onSubmit;
    }

    result = TREE_Control_DropdownData_Init(
        &wrapper->data.dropdown,
        wrapper->ownedOptions,
        wrapper->ownedOptionsSize,
        (TREE_Size)selected,
        wrapper->on_submit ? control_submit_callback : NULL,
        &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_Dropdown_Init(&wrapper->control, NULL, &wrapper->data.dropdown);
    }
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_submit);
        TREE_Control_DropdownData_Free(&wrapper->data.dropdown);
        free_owned_options(wrapper->ownedOptions, wrapper->ownedOptionsSize);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_Checkbox(PyObject* self, PyObject* args)
{
    const char* text;
    int flags;
    PyObject* themeObj;
    PyObject* onCheck = Py_None;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "siO|O", &text, &flags, &themeObj, &onCheck))
    {
        return NULL;
    }
    if (!validate_callback(onCheck))
    {
        return NULL;
    }
    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_CHECKBOX;

    if (onCheck != Py_None)
    {
        Py_INCREF(onCheck);
        wrapper->on_check = onCheck;
    }

    result = TREE_Control_CheckboxData_Init(&wrapper->data.checkbox, text, (TREE_Control_CheckboxFlags)flags, wrapper->on_check ? control_check_callback : NULL, &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_Checkbox_Init(&wrapper->control, NULL, &wrapper->data.checkbox);
    }
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_check);
        TREE_Control_CheckboxData_Free(&wrapper->data.checkbox);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_NumberInput(PyObject* self, PyObject* args)
{
    double value;
    double minValue;
    double maxValue;
    double increment;
    int decimalPlaces;
    PyObject* themeObj;
    PyObject* onChange = Py_None;
    PyObject* onSubmit = Py_None;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "ddddiO|OO", &value, &minValue, &maxValue, &increment, &decimalPlaces, &themeObj, &onChange, &onSubmit))
    {
        return NULL;
    }
    if (!validate_callback(onChange) || !validate_callback(onSubmit))
    {
        return NULL;
    }
    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_NUMBER_INPUT;
    if (onChange != Py_None)
    {
        Py_INCREF(onChange);
        wrapper->on_change = onChange;
    }
    if (onSubmit != Py_None)
    {
        Py_INCREF(onSubmit);
        wrapper->on_submit = onSubmit;
    }

    result = TREE_Control_NumberInputData_Init(
        &wrapper->data.numberInput,
        value,
        minValue,
        maxValue,
        increment,
        decimalPlaces,
        wrapper->on_change ? control_change_callback : NULL,
        wrapper->on_submit ? control_submit_callback : NULL,
        &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_NumberInput_Init(&wrapper->control, NULL, &wrapper->data.numberInput);
    }
    if (result != TREE_OK)
    {
        Py_XDECREF(wrapper->on_change);
        Py_XDECREF(wrapper->on_submit);
        TREE_Control_NumberInputData_Free(&wrapper->data.numberInput);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Create_ProgressBar(PyObject* self, PyObject* args)
{
    PyObject* themeObj;
    PyTREE_ThemeWrapper* theme;
    PyTREE_ControlWrapper* wrapper;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &themeObj))
    {
        return NULL;
    }

    theme = get_theme_wrapper(themeObj);
    if (!theme)
    {
        return NULL;
    }

    wrapper = (PyTREE_ControlWrapper*)calloc(1, sizeof(PyTREE_ControlWrapper));
    if (!wrapper)
    {
        return PyErr_NoMemory();
    }
    wrapper->kind = PYTREE_CONTROL_PROGRESS_BAR;

    result = TREE_Control_ProgressBarData_Init(&wrapper->data.progressBar, &theme->theme);
    if (result == TREE_OK)
    {
        result = TREE_Control_ProgressBar_Init(&wrapper->control, NULL, &wrapper->data.progressBar);
    }
    if (result != TREE_OK)
    {
        TREE_Control_ProgressBarData_Free(&wrapper->data.progressBar);
        free(wrapper);
        return raise_tree_result(result);
    }

    wrapper->alive = TREE_TRUE;
    return create_control_wrapper(wrapper);
}

static PyObject* PyTREE_Control_SetTransform(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static char* keywords[] = {"control", "x", "y", "width", "height", "alignment", NULL};
    PyObject* controlObj;
    int x = INT_MIN;
    int y = INT_MIN;
    int width = INT_MIN;
    int height = INT_MIN;
    int alignment = INT_MIN;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiiii", keywords, &controlObj, &x, &y, &width, &height, &alignment))
    {
        return NULL;
    }

    control = get_control_wrapper(controlObj);
    if (!control)
    {
        return NULL;
    }

    if (x != INT_MIN)
    {
        control->control.transform->localOffset.x = x;
    }
    if (y != INT_MIN)
    {
        control->control.transform->localOffset.y = y;
    }
    if (width != INT_MIN)
    {
        control->control.transform->localExtent.width = width;
    }
    if (height != INT_MIN)
    {
        control->control.transform->localExtent.height = height;
    }
    if (alignment != INT_MIN)
    {
        control->control.transform->localAlignment = (TREE_Alignment)alignment;
    }

    result = TREE_Transform_Dirty(control->control.transform);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }

    Py_RETURN_NONE;
}

static PyObject* PyTREE_Control_Link_Wrap(PyObject* self, PyObject* args)
{
    PyObject* srcObj;
    int direction;
    int linkType;
    PyObject* dstObj;
    PyTREE_ControlWrapper* src;
    PyTREE_ControlWrapper* dst;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "OiiO", &srcObj, &direction, &linkType, &dstObj))
    {
        return NULL;
    }

    src = get_control_wrapper(srcObj);
    if (!src)
    {
        return NULL;
    }
    dst = get_control_wrapper(dstObj);
    if (!dst)
    {
        return NULL;
    }

    result = TREE_Control_Link(&src->control, (TREE_Direction)direction, (TREE_ControlLink)linkType, &dst->control);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_NumberInput_GetValue_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }

    control = get_control_wrapper(controlObj);
    if (!control)
    {
        return NULL;
    }
    return PyFloat_FromDouble(TREE_Control_NumberInput_GetValue(&control->control));
}

static PyObject* PyTREE_ProgressBar_SetValue_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    double value;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Od", &controlObj, &value))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control)
    {
        return NULL;
    }

    result = TREE_Control_ProgressBar_SetValue(&control->control, value);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Label_SetText_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    const char* text;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Os", &controlObj, &text))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_LABEL, "label_set_text"))
    {
        return NULL;
    }

    result = TREE_Control_Label_SetText(&control->control, text);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Label_GetText_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    TREE_String text;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_LABEL, "label_get_text"))
    {
        return NULL;
    }

    text = TREE_Control_Label_GetText(&control->control);
    if (!text)
    {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(text);
}

static PyObject* PyTREE_Button_SetText_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    const char* text;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Os", &controlObj, &text))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_BUTTON, "button_set_text"))
    {
        return NULL;
    }

    result = TREE_Control_Button_SetText(&control->control, text);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Button_GetText_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    TREE_String text;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_BUTTON, "button_get_text"))
    {
        return NULL;
    }

    text = TREE_Control_Button_GetText(&control->control);
    if (!text)
    {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(text);
}

static PyObject* PyTREE_TextInput_SetText_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    const char* text;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Os", &controlObj, &text))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_TEXT_INPUT, "text_input_set_text"))
    {
        return NULL;
    }

    result = TREE_Control_TextInput_SetText(&control->control, text);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_TextInput_GetText_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    TREE_String text;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_TEXT_INPUT, "text_input_get_text"))
    {
        return NULL;
    }

    text = TREE_Control_TextInput_GetText(&control->control);
    if (!text)
    {
        Py_RETURN_NONE;
    }
    return PyUnicode_FromString(text);
}

static PyObject* PyTREE_List_SetSelected_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    unsigned long long index;
    int selected;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "OKp", &controlObj, &index, &selected))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_LIST, "list_set_selected"))
    {
        return NULL;
    }

    result = TREE_Control_List_SetSelected(&control->control, (TREE_Size)index, (TREE_Bool)selected);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_List_GetSelected_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    TREE_Size index;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_LIST, "list_get_selected"))
    {
        return NULL;
    }

    index = TREE_Control_List_GetSelected(&control->control);
    return PyLong_FromUnsignedLongLong(index);
}

static PyObject* PyTREE_Dropdown_SetSelected_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    unsigned long long index;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "OK", &controlObj, &index))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_DROPDOWN, "dropdown_set_selected"))
    {
        return NULL;
    }

    result = TREE_Control_Dropdown_SetSelected(&control->control, (TREE_Size)index);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Dropdown_GetSelected_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    TREE_Size index;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_DROPDOWN, "dropdown_get_selected"))
    {
        return NULL;
    }

    index = TREE_Control_Dropdown_GetSelected(&control->control);
    return PyLong_FromUnsignedLongLong(index);
}

static PyObject* PyTREE_Checkbox_SetChecked_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    int checked;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Op", &controlObj, &checked))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_CHECKBOX, "checkbox_set_checked"))
    {
        return NULL;
    }

    result = TREE_Control_Checkbox_SetChecked(&control->control, (TREE_Byte)checked);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_Checkbox_GetChecked_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    TREE_Bool checked;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_CHECKBOX, "checkbox_get_checked"))
    {
        return NULL;
    }

    checked = TREE_Control_Checkbox_GetChecked(&control->control);
    if (checked)
    {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

static PyObject* PyTREE_NumberInput_SetValue_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    double value;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Od", &controlObj, &value))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_NUMBER_INPUT, "number_input_set_value"))
    {
        return NULL;
    }

    result = TREE_Control_NumberInput_SetValue(&control->control, value);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_ProgressBar_GetValue_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_PROGRESS_BAR, "progress_bar_get_value"))
    {
        return NULL;
    }
    return PyFloat_FromDouble(TREE_Control_ProgressBar_GetValue(&control->control));
}

static PyObject* PyTREE_ProgressBar_SetDirection_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    int direction;
    PyTREE_ControlWrapper* control;
    TREE_Result result;
    (void)self;

    if (!PyArg_ParseTuple(args, "Oi", &controlObj, &direction))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_PROGRESS_BAR, "progress_bar_set_direction"))
    {
        return NULL;
    }

    result = TREE_Control_ProgressBar_SetDirection(&control->control, (TREE_Direction)direction);
    if (result != TREE_OK)
    {
        return raise_tree_result(result);
    }
    Py_RETURN_NONE;
}

static PyObject* PyTREE_ProgressBar_GetDirection_Wrap(PyObject* self, PyObject* args)
{
    PyObject* controlObj;
    PyTREE_ControlWrapper* control;
    (void)self;

    if (!PyArg_ParseTuple(args, "O", &controlObj))
    {
        return NULL;
    }
    control = get_control_wrapper(controlObj);
    if (!control || !ensure_control_kind(control, PYTREE_CONTROL_PROGRESS_BAR, "progress_bar_get_direction"))
    {
        return NULL;
    }
    return PyLong_FromLong((long)TREE_Control_ProgressBar_GetDirection(&control->control));
}

static PyMethodDef MyMethods[] = {
    {"init", PyTREE_Init, METH_NOARGS, "Initialize the TREE library."},
    {"deinit", PyTREE_Deinit, METH_NOARGS, "Deinitialize the TREE library."},
    {"time_now", PyTREE_Time_Now, METH_NOARGS, "Get current time in milliseconds."},
    {"result_to_string", PyTREE_Result_ToString_Wrap, METH_VARARGS, "Convert TREE result code to readable string."},
    {"window_set_title", PyTREE_Window_SetTitle_Wrap, METH_VARARGS, "Set terminal window title."},
    {"window_get_extent", PyTREE_Window_GetExtent_Wrap, METH_NOARGS, "Get terminal window size as (width, height)."},
    {"window_present", PyTREE_Window_Present_Wrap, METH_VARARGS, "Present a refreshed surface to terminal output."},
    {"window_beep", PyTREE_Window_Beep_Wrap, METH_NOARGS, "Play terminal beep sound."},
    {"cursor_set_visible", PyTREE_Cursor_SetVisible_Wrap, METH_VARARGS, "Set cursor visibility."},
    {"clipboard_set_text", PyTREE_Clipboard_SetText_Wrap, METH_VARARGS, "Set system clipboard text."},
    {"clipboard_get_text", PyTREE_Clipboard_GetText_Wrap, METH_NOARGS, "Get system clipboard text or None."},
    {"image_create", PyTREE_Image_Create, METH_VARARGS, "Create an image with width and height."},
    {"image_get_extent", PyTREE_Image_GetExtent_Wrap, METH_VARARGS, "Get image size as (width, height)."},
    {"image_resize", PyTREE_Image_Resize_Wrap, METH_VARARGS, "Resize an image."},
    {"image_set_pixel", PyTREE_Image_SetPixel_Wrap, METH_VARARGS, "Set image pixel at x/y with char and colors."},
    {"image_get_pixel", PyTREE_Image_GetPixel_Wrap, METH_VARARGS, "Get image pixel at x/y as (char, fg, bg)."},
    {"image_draw_string", PyTREE_Image_DrawString_Wrap, METH_VARARGS, "Draw string at x/y with color pair."},
    {"image_clear", PyTREE_Image_Clear_Wrap, METH_VARARGS, "Clear image with char and colors."},
    {"surface_create", PyTREE_Surface_Create, METH_VARARGS, "Create a surface with width and height."},
    {"surface_get_extent", PyTREE_Surface_GetExtent_Wrap, METH_VARARGS, "Get surface size as (width, height)."},
    {"surface_resize", PyTREE_Surface_Resize_Wrap, METH_VARARGS, "Resize a surface."},
    {"surface_set_pixel", PyTREE_Surface_SetPixel_Wrap, METH_VARARGS, "Set surface image pixel at x/y with char and colors."},
    {"surface_get_pixel", PyTREE_Surface_GetPixel_Wrap, METH_VARARGS, "Get surface image pixel at x/y as (char, fg, bg)."},
    {"surface_draw_string", PyTREE_Surface_DrawString_Wrap, METH_VARARGS, "Draw string onto surface image at x/y with color pair."},
    {"surface_clear", PyTREE_Surface_Clear_Wrap, METH_VARARGS, "Clear surface image with char and colors."},
    {"surface_refresh", PyTREE_Surface_Refresh_Wrap, METH_VARARGS, "Refresh surface text buffer from image data."},
    {"theme_create", PyTREE_Theme_Create, METH_NOARGS, "Create a TREE theme object."},
    {"theme_set_char", PyTREE_Theme_SetChar, METH_VARARGS, "Set one theme character by id."},
    {"theme_get_char", PyTREE_Theme_GetChar, METH_VARARGS, "Get one theme character by id."},
    {"theme_set_pixel", PyTREE_Theme_SetPixel, METH_VARARGS, "Set one theme pixel by id with char/fg/bg."},
    {"theme_get_pixel", PyTREE_Theme_GetPixel, METH_VARARGS, "Get one theme pixel by id as (char, fg, bg)."},
    {"colorpair_create", PyTREE_ColorPair_Create_Wrap, METH_VARARGS, "Create packed color pair from foreground/background colors."},
    {"colorpair_get_foreground", PyTREE_ColorPair_GetForeground_Wrap, METH_VARARGS, "Get foreground color from a packed color pair."},
    {"colorpair_get_background", PyTREE_ColorPair_GetBackground_Wrap, METH_VARARGS, "Get background color from a packed color pair."},
    {"application_create", PyTREE_Application_Create, METH_VARARGS, "Create a TREE application. Optional callback receives event dicts."},
    {"application_add_control", PyTREE_Application_AddControl_Wrap, METH_VARARGS, "Add a control to an application."},
    {"application_clear_controls", PyTREE_Application_ClearControls_Wrap, METH_VARARGS, "Remove all controls from an application without freeing them."},
    {"application_run", PyTREE_Application_Run_Wrap, METH_VARARGS, "Run the application main loop."},
    {"application_quit", PyTREE_Application_Quit_Wrap, METH_VARARGS, "Stop the application main loop."},
    {"create_label", PyTREE_Create_Label, METH_VARARGS, "Create a label control."},
    {"create_button", PyTREE_Create_Button, METH_VARARGS, "Create a button control."},
    {"create_text_input", PyTREE_Create_TextInput, METH_VARARGS, "Create a text input control."},
    {"create_list", PyTREE_Create_List, METH_VARARGS, "Create a list control."},
    {"create_dropdown", PyTREE_Create_Dropdown, METH_VARARGS, "Create a dropdown control."},
    {"create_checkbox", PyTREE_Create_Checkbox, METH_VARARGS, "Create a checkbox control."},
    {"create_number_input", PyTREE_Create_NumberInput, METH_VARARGS, "Create a number input control."},
    {"create_progress_bar", PyTREE_Create_ProgressBar, METH_VARARGS, "Create a progress bar control."},
    {"control_set_transform", (PyCFunction)PyTREE_Control_SetTransform, METH_VARARGS | METH_KEYWORDS, "Set local transform values (x, y, width, height, alignment)."},
    {"control_link", PyTREE_Control_Link_Wrap, METH_VARARGS, "Link two controls for directional navigation."},
    {"number_input_get_value", PyTREE_NumberInput_GetValue_Wrap, METH_VARARGS, "Get value from number input control."},
    {"number_input_set_value", PyTREE_NumberInput_SetValue_Wrap, METH_VARARGS, "Set value on number input control."},
    {"progress_bar_set_value", PyTREE_ProgressBar_SetValue_Wrap, METH_VARARGS, "Set value on progress bar (0.0 to 1.0)."},
    {"progress_bar_get_value", PyTREE_ProgressBar_GetValue_Wrap, METH_VARARGS, "Get value from progress bar."},
    {"progress_bar_set_direction", PyTREE_ProgressBar_SetDirection_Wrap, METH_VARARGS, "Set direction of progress bar."},
    {"progress_bar_get_direction", PyTREE_ProgressBar_GetDirection_Wrap, METH_VARARGS, "Get direction of progress bar."},
    {"label_set_text", PyTREE_Label_SetText_Wrap, METH_VARARGS, "Set label text."},
    {"label_get_text", PyTREE_Label_GetText_Wrap, METH_VARARGS, "Get label text."},
    {"button_set_text", PyTREE_Button_SetText_Wrap, METH_VARARGS, "Set button text."},
    {"button_get_text", PyTREE_Button_GetText_Wrap, METH_VARARGS, "Get button text."},
    {"text_input_set_text", PyTREE_TextInput_SetText_Wrap, METH_VARARGS, "Set text input text."},
    {"text_input_get_text", PyTREE_TextInput_GetText_Wrap, METH_VARARGS, "Get text input text."},
    {"list_set_selected", PyTREE_List_SetSelected_Wrap, METH_VARARGS, "Set list selection state by index."},
    {"list_get_selected", PyTREE_List_GetSelected_Wrap, METH_VARARGS, "Get selected list index."},
    {"dropdown_set_selected", PyTREE_Dropdown_SetSelected_Wrap, METH_VARARGS, "Set dropdown selected index."},
    {"dropdown_get_selected", PyTREE_Dropdown_GetSelected_Wrap, METH_VARARGS, "Get dropdown selected index."},
    {"checkbox_set_checked", PyTREE_Checkbox_SetChecked_Wrap, METH_VARARGS, "Set checkbox checked state."},
    {"checkbox_get_checked", PyTREE_Checkbox_GetChecked_Wrap, METH_VARARGS, "Get checkbox checked state."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef PyTREE = {
    PyModuleDef_HEAD_INIT,
    "PyTREE",
    NULL,
    -1,
    MyMethods
};

PyMODINIT_FUNC PyInit_PyTREE(void) {
    PyObject* module = PyModule_Create(&PyTREE);
    if(!module)
    {
        return NULL;
    }

    PyModule_AddIntConstant(module, "CONTROL_TEXT_INPUT_TYPE_NONE", TREE_CONTROL_TEXT_INPUT_TYPE_NONE);
    PyModule_AddIntConstant(module, "CONTROL_TEXT_INPUT_TYPE_NORMAL", TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL);
    PyModule_AddIntConstant(module, "CONTROL_TEXT_INPUT_TYPE_PASSWORD", TREE_CONTROL_TEXT_INPUT_TYPE_PASSWORD);

    PyModule_AddIntConstant(module, "CONTROL_LIST_FLAGS_NONE", TREE_CONTROL_LIST_FLAGS_NONE);
    PyModule_AddIntConstant(module, "CONTROL_LIST_FLAGS_MULTISELECT", TREE_CONTROL_LIST_FLAGS_MULTISELECT);

    PyModule_AddIntConstant(module, "CONTROL_CHECKBOX_FLAGS_NONE", TREE_CONTROL_CHECKBOX_FLAGS_NONE);
    PyModule_AddIntConstant(module, "CONTROL_CHECKBOX_FLAGS_CHECKED", TREE_CONTROL_CHECKBOX_FLAGS_CHECKED);
    PyModule_AddIntConstant(module, "CONTROL_CHECKBOX_FLAGS_REVERSE", TREE_CONTROL_CHECKBOX_FLAGS_REVERSE);
    PyModule_AddIntConstant(module, "CONTROL_CHECKBOX_FLAGS_RADIO", TREE_CONTROL_CHECKBOX_FLAGS_RADIO);

    PyModule_AddIntConstant(module, "CONTROL_LINK_NONE", TREE_CONTROL_LINK_NONE);
    PyModule_AddIntConstant(module, "CONTROL_LINK_SINGLE", TREE_CONTROL_LINK_SINGLE);
    PyModule_AddIntConstant(module, "CONTROL_LINK_DOUBLE", TREE_CONTROL_LINK_DOUBLE);

    PyModule_AddIntConstant(module, "DIRECTION_NONE", TREE_DIRECTION_NONE);
    PyModule_AddIntConstant(module, "DIRECTION_EAST", TREE_DIRECTION_EAST);
    PyModule_AddIntConstant(module, "DIRECTION_NORTH", TREE_DIRECTION_NORTH);
    PyModule_AddIntConstant(module, "DIRECTION_WEST", TREE_DIRECTION_WEST);
    PyModule_AddIntConstant(module, "DIRECTION_SOUTH", TREE_DIRECTION_SOUTH);

    PyModule_AddIntConstant(module, "ALIGNMENT_NONE", TREE_ALIGNMENT_NONE);
    PyModule_AddIntConstant(module, "ALIGNMENT_LEFT", TREE_ALIGNMENT_LEFT);
    PyModule_AddIntConstant(module, "ALIGNMENT_RIGHT", TREE_ALIGNMENT_RIGHT);
    PyModule_AddIntConstant(module, "ALIGNMENT_TOP", TREE_ALIGNMENT_TOP);
    PyModule_AddIntConstant(module, "ALIGNMENT_BOTTOM", TREE_ALIGNMENT_BOTTOM);
    PyModule_AddIntConstant(module, "ALIGNMENT_TOPLEFT", TREE_ALIGNMENT_TOPLEFT);
    PyModule_AddIntConstant(module, "ALIGNMENT_TOPRIGHT", TREE_ALIGNMENT_TOPRIGHT);
    PyModule_AddIntConstant(module, "ALIGNMENT_LEFTSTRETCH", TREE_ALIGNMENT_LEFTSTRETCH);

    PyModule_AddIntConstant(module, "EVENT_TYPE_NONE", TREE_EVENT_TYPE_NONE);
    PyModule_AddIntConstant(module, "EVENT_TYPE_REFRESH", TREE_EVENT_TYPE_REFRESH);
    PyModule_AddIntConstant(module, "EVENT_TYPE_DRAW", TREE_EVENT_TYPE_DRAW);
    PyModule_AddIntConstant(module, "EVENT_TYPE_KEY_DOWN", TREE_EVENT_TYPE_KEY_DOWN);
    PyModule_AddIntConstant(module, "EVENT_TYPE_KEY_HELD", TREE_EVENT_TYPE_KEY_HELD);
    PyModule_AddIntConstant(module, "EVENT_TYPE_KEY_UP", TREE_EVENT_TYPE_KEY_UP);
    PyModule_AddIntConstant(module, "EVENT_TYPE_WINDOW_RESIZE", TREE_EVENT_TYPE_WINDOW_RESIZE);

    PyModule_AddIntConstant(module, "COLOR_BLACK", TREE_COLOR_BLACK);
    PyModule_AddIntConstant(module, "COLOR_RED", TREE_COLOR_RED);
    PyModule_AddIntConstant(module, "COLOR_GREEN", TREE_COLOR_GREEN);
    PyModule_AddIntConstant(module, "COLOR_YELLOW", TREE_COLOR_YELLOW);
    PyModule_AddIntConstant(module, "COLOR_BLUE", TREE_COLOR_BLUE);
    PyModule_AddIntConstant(module, "COLOR_MAGENTA", TREE_COLOR_MAGENTA);
    PyModule_AddIntConstant(module, "COLOR_CYAN", TREE_COLOR_CYAN);
    PyModule_AddIntConstant(module, "COLOR_WHITE", TREE_COLOR_WHITE);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_BLACK", TREE_COLOR_BRIGHT_BLACK);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_RED", TREE_COLOR_BRIGHT_RED);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_GREEN", TREE_COLOR_BRIGHT_GREEN);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_YELLOW", TREE_COLOR_BRIGHT_YELLOW);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_BLUE", TREE_COLOR_BRIGHT_BLUE);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_MAGENTA", TREE_COLOR_BRIGHT_MAGENTA);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_CYAN", TREE_COLOR_BRIGHT_CYAN);
    PyModule_AddIntConstant(module, "COLOR_BRIGHT_WHITE", TREE_COLOR_BRIGHT_WHITE);
    PyModule_AddIntConstant(module, "COLOR_DEFAULT_FOREGROUND", TREE_COLOR_DEFAULT_FOREGROUND);
    PyModule_AddIntConstant(module, "COLOR_DEFAULT_BACKGROUND", TREE_COLOR_DEFAULT_BACKGROUND);

    PyModule_AddIntConstant(module, "THEME_CID_EMPTY", TREE_THEME_CID_EMPTY);
    PyModule_AddIntConstant(module, "THEME_CID_SCROLL_V_AREA", TREE_THEME_CID_SCROLL_V_AREA);
    PyModule_AddIntConstant(module, "THEME_CID_SCROLL_H_AREA", TREE_THEME_CID_SCROLL_H_AREA);
    PyModule_AddIntConstant(module, "THEME_CID_SCROLL_V_BAR", TREE_THEME_CID_SCROLL_V_BAR);
    PyModule_AddIntConstant(module, "THEME_CID_SCROLL_H_BAR", TREE_THEME_CID_SCROLL_H_BAR);
    PyModule_AddIntConstant(module, "THEME_CID_UP", TREE_THEME_CID_UP);
    PyModule_AddIntConstant(module, "THEME_CID_DOWN", TREE_THEME_CID_DOWN);
    PyModule_AddIntConstant(module, "THEME_CID_LEFT", TREE_THEME_CID_LEFT);
    PyModule_AddIntConstant(module, "THEME_CID_RIGHT", TREE_THEME_CID_RIGHT);
    PyModule_AddIntConstant(module, "THEME_CID_CHECKBOX_UNCHECKED", TREE_THEME_CID_CHECKBOX_UNCHECKED);
    PyModule_AddIntConstant(module, "THEME_CID_CHECKBOX_CHECKED", TREE_THEME_CID_CHECKBOX_CHECKED);
    PyModule_AddIntConstant(module, "THEME_CID_CHECKBOX_LEFT", TREE_THEME_CID_CHECKBOX_LEFT);
    PyModule_AddIntConstant(module, "THEME_CID_CHECKBOX_RIGHT", TREE_THEME_CID_CHECKBOX_RIGHT);
    PyModule_AddIntConstant(module, "THEME_CID_RADIOBOX_UNCHECKED", TREE_THEME_CID_RADIOBOX_UNCHECKED);
    PyModule_AddIntConstant(module, "THEME_CID_RADIOBOX_CHECKED", TREE_THEME_CID_RADIOBOX_CHECKED);
    PyModule_AddIntConstant(module, "THEME_CID_RADIOBOX_LEFT", TREE_THEME_CID_RADIOBOX_LEFT);
    PyModule_AddIntConstant(module, "THEME_CID_RADIOBOX_RIGHT", TREE_THEME_CID_RADIOBOX_RIGHT);

    PyModule_AddIntConstant(module, "THEME_PID_NORMAL", TREE_THEME_PID_NORMAL);
    PyModule_AddIntConstant(module, "THEME_PID_FOCUSED", TREE_THEME_PID_FOCUSED);
    PyModule_AddIntConstant(module, "THEME_PID_ACTIVE", TREE_THEME_PID_ACTIVE);
    PyModule_AddIntConstant(module, "THEME_PID_HOVERED", TREE_THEME_PID_HOVERED);
    PyModule_AddIntConstant(module, "THEME_PID_NORMAL_SELECTED", TREE_THEME_PID_NORMAL_SELECTED);
    PyModule_AddIntConstant(module, "THEME_PID_FOCUSED_SELECTED", TREE_THEME_PID_FOCUSED_SELECTED);
    PyModule_AddIntConstant(module, "THEME_PID_ACTIVE_SELECTED", TREE_THEME_PID_ACTIVE_SELECTED);
    PyModule_AddIntConstant(module, "THEME_PID_HOVERED_SELECTED", TREE_THEME_PID_HOVERED_SELECTED);
    PyModule_AddIntConstant(module, "THEME_PID_NORMAL_TEXT", TREE_THEME_PID_NORMAL_TEXT);
    PyModule_AddIntConstant(module, "THEME_PID_FOCUSED_TEXT", TREE_THEME_PID_FOCUSED_TEXT);
    PyModule_AddIntConstant(module, "THEME_PID_NORMAL_SCROLL_AREA", TREE_THEME_PID_NORMAL_SCROLL_AREA);
    PyModule_AddIntConstant(module, "THEME_PID_FOCUSED_SCROLL_AREA", TREE_THEME_PID_FOCUSED_SCROLL_AREA);
    PyModule_AddIntConstant(module, "THEME_PID_ACTIVE_SCROLL_AREA", TREE_THEME_PID_ACTIVE_SCROLL_AREA);
    PyModule_AddIntConstant(module, "THEME_PID_NORMAL_SCROLL_BAR", TREE_THEME_PID_NORMAL_SCROLL_BAR);
    PyModule_AddIntConstant(module, "THEME_PID_FOCUSED_SCROLL_BAR", TREE_THEME_PID_FOCUSED_SCROLL_BAR);
    PyModule_AddIntConstant(module, "THEME_PID_ACTIVE_SCROLL_BAR", TREE_THEME_PID_ACTIVE_SCROLL_BAR);
    PyModule_AddIntConstant(module, "THEME_PID_CURSOR", TREE_THEME_PID_CURSOR);
    PyModule_AddIntConstant(module, "THEME_PID_PROGRESS_BAR", TREE_THEME_PID_PROGRESS_BAR);
    PyModule_AddIntConstant(module, "THEME_PID_BACKGROUND", TREE_THEME_PID_BACKGROUND);

    return module;
}