#include "PyInterface.h"

PythonInterface::PythonInterface()
{
    // Init python interperter
    Py_Initialize();
}

PythonInterface::~PythonInterface()
{
    // clean up
    for ( auto& module : this->m_Modules ) // free loaded modules
        Py_DECREF(module.second);
    for ( auto& loadedClass : this->m_Classes ) // free loaded classes
        Py_DECREF(loadedClass.second);
    
    Py_Finalize();
}

template <typename ...Args>
PyObject* PythonInterface::CallClassMethod(_IN_ const char* modName, _IN_ const char* className, _IN_ const char* methodName, _IN_ _OPTIONAL_ Args&&... args) {
    PyObject* loadedModule = LoadPythonModule(modName);
    PyObject* loadedClass = GetInlineClass(modName, className);
    PyObject* classMethod = PyObject_CallMethod(loadedClass, methodName, args...);

    return classMethod;
}

PyObject* PythonInterface::GetInlineClass(_IN_ const char* modName, _IN_ const char* className) {
    PyObject* globalDict = PyModule_GetDict(LoadPythonModule(modName));
    return PyDict_GetItemString(globalDict, className);    
}

PyObject* PythonInterface::GetClass(_IN_ const char* modName, _IN_ const char* className) {
    PyObject* loadedModule = LoadPythonModule(modName);
    if ( !loadedModule ) return nullptr;

    return PyObject_GetAttrString(loadedModule, className);
}

bool PythonInterface::IsModuleLoaded(_IN_ const char* modName) {
    return ( this->m_Modules.find(modName) != this->m_Modules.end() );
}

PyObject* PythonInterface::LoadPythonModule(_IN_ const char* modName) {
    PyObject* name = PyUnicode_DecodeFSDefault(modName);
    if ( !name )
        return nullptr;

    if ( IsModuleLoaded(modName) )
        return this->m_Modules.at(modName);

    PyObject* module = PyImport_Import(name);
    if ( !module )
        return nullptr;

    return module;
}