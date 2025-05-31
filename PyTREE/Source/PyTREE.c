#include "../../TREE/Source/TREE.h"
#include <Python.h>

static PyObject* PyTREE_Init()
{
    TREE_Result result = TREE_Init();
    if (result != TREE_OK)
    {
        PyErr_SetString(PyExc_RuntimeError, TREE_Result_ToString(result));
        return NULL;
    }
    
    return Py_None;
}

static PyObject* PyTREE_Deinit()
{
    TREE_Free();    
    return Py_None;
}

static PyObject* PyTREE_Time_Now()
{
    return PyLong_FromLongLong(TREE_Time_Now());
}

static PyMethodDef MyMethods[] = {
    // {"add", py_add, METH_VARARGS, "Add two integers"},
    {"init", PyTREE_Init, METH_NOARGS, "Initialize the TREE library"},
    {"deinit", PyTREE_Deinit, METH_NOARGS, "Deinitialize the TREE library"},
    {"time_now", PyTREE_Time_Now, METH_NOARGS, "Get the current time in milliseconds since epoch"},
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

    // add constants
    // PyModule_AddIntConstant(module, "CONSTANT_ONE", 1);

    return module;
}