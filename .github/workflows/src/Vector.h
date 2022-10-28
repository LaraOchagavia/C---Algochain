#ifndef VECTOR_H_INCLUDED_
#define VECTOR_H_INCLUDED_ 
#include <iostream>
#include <string>

#define DEFECT_SIZE 100

using namespace std;

template <typename T>
class Vector
{
private:
	int size; // Cuanto va a tener de largo el Array
	T *ptr; // Puntero al vector dinamico que contiene los datos de tipo T

public:
	Vector();
	Vector(int); 
	Vector(const Vector <T> &);
	Vector(int, const Vector <T> & );
	~Vector(); 

	Vector<T>& operator=(const Vector <T> &); 
	bool operator==(const Vector <T> &);
	T & operator[](int) const;

	int get_size() const;
	int search(const T &);
	int position(const T &);
	void clear();
};

template <typename T>
Vector<T>::Vector()
{
	size = DEFECT_SIZE;
	ptr = new T[size];
}

template <typename T>
Vector<T>::Vector(int x)
{
	size = x;

	if(x!=0)
		ptr = new T[size];
	else 
		ptr = 0;
}

template <typename T>
Vector<T>::Vector(const Vector <T> & v)
{
	size = v.size;

	ptr = new T[size];

	for (int i=0; i < size ; i++)
		ptr[i] = v.ptr[i];	
}

template <typename T>
Vector<T>::Vector(int len,const Vector <T> & v)
{
	size=len;

	if(len !=0)
	{
		ptr = new T[size];
		
		for (int i=0; i < v.size ; i++)
			ptr[i] = v.ptr[i];
	}
}

template <typename T>
Vector<T>::~Vector()
{	
	if (ptr) //si el puntero NO apunta a NULL-> libera la memoria pedida
		delete [] ptr;
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T> & v2) // v1 = v2
{
	if (this != &v2 )
	{
		if (size != v2.size)
		{
			T * aux;
			size = v2.size;
			delete [] ptr;
			aux = new T[size];
			ptr = aux;
		}
		for (int i = 0 ; i < size; i++)
			ptr[i] = v2.ptr[i];
		return *this; //devuelve el vector de <T> de la clase Vector por referencia
	}
	return *this;
}

template <typename T>
bool Vector<T>::operator==(const Vector<T> & v2)//v1 == v2
{
	if (size != v2.size)
		return false;
	else
	{
		for (int i = 0 ; i < size ; i++)
		{
			if (ptr[i] != v2.ptr[i])
				return false;
		}
		return true;
	}
}

template <typename T>
T & Vector<T>::operator[](int pos) const// v[pos]
{
	if( pos < size)
		return ptr[pos];
	cout << "Out of range"<< endl;
	exit(1);
}

template <typename T>
int Vector<T>::get_size() const
{
	return size;
}

template <typename T>
int Vector<T>::search(const T & data)
{
	int j = 0;
	for (int i = 0 ; i < size ; i++)
	{
		if (ptr[i] == data)
			j++;
	}
	return j;
}

template <typename T>
int Vector<T>::position(const T & data)
{
	for (int i = 0 ; i < size ; i++)
	{
		if (ptr[i] == data)
			return i;
	}
	return -1;
}

template <typename T>
void Vector<T>::clear()
{
	if(ptr)
		delete [] ptr;
	size = 0;
} 

#endif //VECTOR_H_INCLUDED_