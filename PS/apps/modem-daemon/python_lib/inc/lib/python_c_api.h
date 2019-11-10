/**
 * Флайл:
 *
 * Описание:
*/

#ifndef _PYTHON_C_API_H_
#define _PYTHON_C_API_H_

#include <string>
#include <python3.6m/Python.h>

class PyObjectBase;

class python_c_api
{
public:
	python_c_api();
	~python_c_api();

	/**
	 * @brief установка и настройка директорий до python модулей
	 *
	 * @param module_path директория до python модуля
	 */
	void py_set_module_path(const std::string& module_path) noexcept;

	/**
	 * @brief импорт используемого python модуля
	 *
	 * @param module_name имя импортируемого модуля
	 *
	 * @throw
	 */
	void py_import_module(const std::string& module_name);

	/**
	 * @brief вызов функции из модуля python
	 *
	 * @param function_name имя функции из модуля python
	 *
	 */
	void py_function_call(const std::string& function_name);

	/**
	 * @brief вызов функции из модуля python
	 *
	 * @param function_name имя функции из модуля python, args параметры вызываемой функции
	 */
	template <class... Args>
	void py_function_call(const std::string& function_name, Args&&... args);

	/**
	 * @brief вызов функции из модуля python
	 *
	 * @param function_name имя функции из модуля python
	 *
	 * @return результат выполнения функции
	 */
	template <typename T>
	T py_ret_function_call(const std::string& function_name);

	/**
	 * @brief вызов функции из модуля python
	 *
	 * @param function_name имя функции из модуля python, args аргументы функции function_name
	 *
	 * @return результат выполнения функции
	 */
	template <typename T, typename... Args>
	T py_ret_function_call(const std::string& function_name, Args&&... args);

	/**
	 *  @brief создания экземпляра класса из модуля python
	 *
	 *  @param class_name имя класса из модуля python
	 *
	 *  @throw std::logic_error
	 */
	void py_class_instance(const std::string& class_name);

	/**
	 *  @brief вызов метода класса
	 *
	 *  @param method_name имя метода класса
	 *
	 *  @throw std::logic_error
	 */
	void py_class_method_call(const std::string& method_name);

	/**
	 * @brief вызов функции из модуля python
	 *
	 * @param method_name имя функции из модуля python
	 */
	template <class... Args>
 	void py_class_method_call(const std::string& method_name, Args&&... args);

	python_c_api(const python_c_api& copy) = delete;
	python_c_api& operator=(const python_c_api& copy) = delete;

private:
	template<typename... Args>
	PyObject * py_make_args(Args&&... args);

	template<typename HEAD, typename...Args>
	void convert_args(HEAD&& Head, Args&&... args);

	template<typename Arg>
	void convert_args(Arg&& arg);

	template<typename Arg>
	void set_arg(Arg&& arg, size_t index);

private:
	//
	PyObject * pName_;

	//
	PyObject * pModule_;

	//
	PyObject * pInstance_;

	//
	PyObject * pTuple_;

	// Количество переданных аргументов
	size_t py_tuple_size_;

	// Указатель на массив PyObject*
	PyObjectBase * pPyObjectBase_;
};

// Обертка над PyObject* (аргументами функций python), обеспечивающая его перемещение
class PyObjectBase
{
public:
	PyObjectBase(PyObject * object) : rhs_(object)
	{

	}

	PyObjectBase() : PyObjectBase(nullptr)
	{

	}

	// Коструктор копирования
	PyObjectBase(const PyObjectBase& object) = delete;

	// Конструктор перемещения
	PyObjectBase(PyObjectBase&& object)
	{
 		rhs_ = object.rhs_;
		object.rhs_ = nullptr;
	}

	// Оператор присваивания
	PyObjectBase& operator=(const PyObjectBase& object) = delete;

	// Оператор перемещения
	PyObjectBase& operator=(PyObjectBase&& object)
	{
		rhs_ = object.rhs_;
		object.rhs_ = nullptr;
		return *this;
	}

	// Оператор присывивания
	PyObjectBase& operator=(PyObject* object)
	{
		rhs_ = object;
		return *this;
	}

	// Функтор
	operator PyObject*()
	{
		return rhs_;
	}

	~PyObjectBase()
	{
		if (rhs_) { Py_DECREF(rhs_); }
	}

public:
	//
	PyObject * rhs_;
};

template <class... Args>
void python_c_api::py_function_call(const std::string& function_name, Args&&... args)
{
	PyObject * pFunc = PyObject_GetAttrString(pModule_, function_name.c_str());
	PyObject * pValue = PyObject_CallFunction(pFunc, "O", py_make_args(std::forward<Args>(args)...));

	if(pFunc) { Py_DECREF(pFunc); }
	if(pValue) { Py_DECREF(pValue); }

	// Удаление указателей после выполнения функции
	delete [] pPyObjectBase_;
}

template <class... Args>
void python_c_api::py_class_method_call(const std::string& method_name, Args&&... args)
{
	if (pInstance_)
	{
		PyObject * pValue = PyObject_CallMethod(pInstance_, method_name.c_str(), "O", py_make_args(std::forward<Args>(args)...));
	}

	// Удаление указателей после выполнения функции
	delete [] pPyObjectBase_;
}

template<typename... Args>
PyObject * python_c_api::py_make_args(Args&&... args)
{
	py_tuple_size_ = sizeof...(args);

	pTuple_ = PyTuple_New(py_tuple_size_);

	// Создание объектов PyObject* равное количеству переданных аргументов python-функции
	pPyObjectBase_ = new PyObjectBase[py_tuple_size_];

	this->convert_args(std::forward<Args>(args)...);

	return pTuple_;
}

template<typename HEAD, typename...Args>
void python_c_api::convert_args(HEAD&& Head, Args&&... args)
{
	this->set_arg<HEAD>(std::forward<HEAD>(Head), py_tuple_size_ - sizeof...(Args) - 1);
	this->convert_args(std::forward<Args>(args)...);
}

template<typename Arg>
void python_c_api::convert_args(Arg&& arg)
{
	this->set_arg<Arg>(std::forward<Arg>(arg), py_tuple_size_ - 1);
}

template<typename T>
struct PyConverterArgs;

template<typename Arg>
void python_c_api::set_arg(Arg&& arg, size_t index)
{
	PyConverterArgs<Arg> convert;

	// Конвертация аргумента в формат данных python'а
	pPyObjectBase_[index] = convert.from(arg);

	PyTuple_SetItem(pTuple_, index, pPyObjectBase_[index]);
}

template<>
struct PyConverterArgs<std::int32_t>
{
	PyObject * from(const std::int32_t& value)
	{
		return PyLong_FromSize_t(value);
	}
};

template<>
struct PyConverterArgs<std::uint32_t>
{
	PyObject * from(const std::uint32_t& value)
	{
		return PyLong_FromSsize_t(value);
	}
};

template<>
struct PyConverterArgs<double>
{
	PyObject * from(const double& value)
	{
		return PyFloat_FromDouble(value);
	}
};

template<>
struct PyConverterArgs<std::string>
{
	PyObject * from(const std::string& value)
	{
		return PyUnicode_FromString(value.c_str());
	}
};

template<typename T>
struct PyConverterRet;

template <typename T>
T python_c_api::py_ret_function_call(const std::string& function_name)
{
	PyObject * pFunc = PyObject_GetAttrString(pModule_, function_name.c_str());
	PyObject * pValue = PyObject_CallFunction(pFunc, "");

	PyConverterRet<T> convert;

	T res = convert.from(pValue);

	if(pFunc) { Py_DECREF(pFunc); }
	if(pValue) { Py_DECREF(pValue); }

	return res;
}

template <typename T, typename... Args>
T python_c_api::py_ret_function_call(const std::string& function_name, Args&&... args)
{
	PyObject * pFunc = PyObject_GetAttrString(pModule_, function_name.c_str());
	PyObject * pValue = PyObject_CallFunction(pFunc, "O", py_make_args(std::forward<Args>(args)...));

	PyConverterRet<T> convert;

	T res = convert.from(pValue);

	if(pFunc) { Py_DECREF(pFunc); }
	if(pValue) { Py_DECREF(pValue); }

	// Удаление указателей после выполнения функции
	delete [] pPyObjectBase_;

	return res;
}

template<>
struct PyConverterRet<std::uint32_t>
{
	std::uint32_t from(PyObject * pValue)
	{
		return PyLong_AsSsize_t(pValue);
	}
};

template<>
struct PyConverterRet<std::int32_t>
{
	std::int32_t from(PyObject * pValue)
	{
		return PyLong_AsSize_t(pValue);
	}
};

template<>
struct PyConverterRet<double>
{
	double from(PyObject * pValue)
	{
		return PyLong_AsDouble(pValue);
	}
};

template<>
struct PyConverterRet<std::string>
{
	std::string from(PyObject * pValue)
	{
		return PyBytes_AsString(PyUnicode_AsUTF8String(pValue));
	}
};

#endif //_PYTHON_C_API_H_
