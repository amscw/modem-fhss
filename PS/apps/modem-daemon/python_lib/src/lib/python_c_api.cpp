#include "python_c_api.h"

#include <stdexcept>

python_c_api::python_c_api() : pName_(nullptr), pModule_(nullptr), pInstance_(nullptr), pTuple_(nullptr)
{
	Py_Initialize();
}

python_c_api::~python_c_api()
{
	if (pTuple_) { Py_DECREF(pTuple_); }
	if (pInstance_) { Py_DECREF(pInstance_); }
	if (pModule_) { Py_DECREF(pModule_); }
	if (pName_) { Py_DECREF(pName_); }

	//Py_Finalize();
}

void python_c_api::py_set_module_path(const std::string& module_path) noexcept
{
	// Импортирование модуля sys
	PyObject * sys = PyImport_ImportModule("sys");
	// Получение ссылки на path
	PyObject * path = PyObject_GetAttrString(sys, "path");
	// Добавление пути в конец списка path
	PyList_Append(path, PyUnicode_FromString(module_path.c_str()));

	if (sys) { Py_DECREF(sys); }
	if (path) { Py_DECREF(path); }
}

void python_c_api::py_import_module(const std::string& module_name)
{
	// Построить объект имени
	pName_ = PyUnicode_FromString(module_name.c_str());

	// Загрузить объект модуля
	pModule_ = PyImport_Import(pName_);

	if(!pModule_)
	{
		throw std::runtime_error("Can not import module");
	}
}

void python_c_api::py_function_call(const std::string& function_name)
{
	PyObject * pFunc = PyObject_GetAttrString(pModule_, function_name.c_str());
	PyObject * pValue = PyObject_CallObject(pFunc, nullptr);

	if (pFunc) { Py_DECREF(pFunc); }
	if (pValue) { Py_DECREF(pValue); }
}

void python_c_api::py_class_instance(const std::string& class_name)
{
	assert(pModule_);
	assert(!class_name.empty());

	// Получение словаря с именами модуля
	PyObject * pDict = PyModule_GetDict(pModule_);

	// Построить имя вызываемого класса
	PyObject * pClass = PyDict_GetItemString(pDict, class_name.c_str());

	if(PyCallable_Check(pClass))
	{
		pInstance_ = PyObject_CallObject(pClass, nullptr);
	}
	else
	{
		throw std::logic_error("Can not instance class: " + class_name);
	}

	if (pDict) { Py_DECREF(pDict); }
	if (pClass) { Py_DECREF(pClass); }
}

void python_c_api::py_class_method_call(const std::string& method_name)
{
	PyObject * pValue = PyObject_CallMethod(pInstance_, method_name.c_str(), nullptr);

	if (pValue) { Py_DECREF(pValue); }
}
