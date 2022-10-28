#ifndef _LIST_H_INCLUDED_
#define _LIST_H_INCLUDED_

#include <cstdlib>
#include "Block.h"


using namespace std;

template<typename T>
class list
{
	class node
	{
		friend class iterator;
		friend class list;
		node *next_;
		node *prev_;
		T data_;

	public:
		node(T const&);
		~node();
	};
	node *first_;
	node *last_;
	size_t siz_;

public:
	class iterator
	{
		friend class list;
		
		node *actual_;
		iterator(node*);

	public:
		iterator();
		iterator(list<T> const &);
		iterator(iterator const &);
		~iterator();

		T& data();
		T const &data() const;
		iterator &go_forward();
		iterator &move_backwards();
		bool end() const;

		bool operator==(const iterator &) const;
		bool operator!=(const iterator &) const;
		iterator const &operator=(iterator const &);
	};

	typedef T t_data;
	typedef node t_node;
	typedef iterator t_iter;

	list();
	list(const list &);
	~list();

	//Metodos de list.
	size_t size() const;
	bool contains(const T &) const;
	bool empty() const;
	void insert(const T &);
	void insert_before(const T &, iterator const &);
	void insert_after(const T &, iterator const &);
	void erase(const T &);
	void destroy(iterator);
	list const &operator=(list const &);

	iterator first() const;
	iterator last() const;
	
};

template<typename T>
list<T>::iterator::iterator() : actual_(0)
{
}

template<typename T>
list<T>::iterator::iterator(node *actual) : actual_(actual)
{

}

template<typename T>
list<T>::iterator::iterator(list<T> const &l) : actual_(l.first_)
{
}

template<typename T>
list<T>::iterator::iterator(iterator const &it) : actual_(it.actual_)
{
}

template<typename T>
list<T>::iterator::~iterator()
{
}

template<typename T>
T & list<T>::iterator::data()
{
	return actual_->data_;
}

template<typename T>
T const &list<T>::iterator::data() const
{
	return actual_->data_;
}

template<typename T>
typename list<T>::iterator &list<T>::iterator::go_forward()
{
	if (actual_)
		actual_ = actual_->next_;
	return *this;
}

template<typename T>
typename list<T>::iterator &list<T>::iterator::move_backwards()
{
	if (actual_)
		actual_ = actual_->prev_;
	return *this;
}

template<typename T>
bool list<T>::iterator::end() const
{
	return actual_ == 0 ? true : false;
}


template<typename T>
bool list<T>::iterator::operator==(const typename list<T>::iterator &it2) const
{
	return actual_ == it2.actual_;
}

template<typename T>
bool list<T>::iterator::operator!=(const typename list<T>::iterator &it2) const
{
        return actual_ != it2.actual_;
}

template<typename T>
typename list<T>::iterator const &list<T>::iterator::operator=(iterator const &orig)
{
        if (this != &orig)
		actual_ = orig.actual_;
	return *this;
}

template<typename T>
list<T>::node::node(const T &t) : next_(0), prev_(0), data_(t)
{
}

template<typename T>
list<T>::node::~node()
{
}

template<typename T>
list<T>::list() : first_(0), last_(0), siz_(0)
{
}

template<typename T>
list<T>::list(const list &orig) : first_(0), last_(0), siz_(orig.siz_)
{
	node *iter;
	node *ant;

	for (iter = orig.first_, ant = 0; iter != 0; iter = iter->next_)
	{
		node *nuevo = new node(iter->data_);
		nuevo->prev_ = ant;
		nuevo->next_ = 0;
		if (ant != 0)
			ant->next_ = nuevo;

		if (first_ == 0)
			first_ = nuevo;
		ant = nuevo;
	}
	
	last_ = ant;
}

template<typename T>
list<T>::~list()
{
	for (node *p = first_; p; )
	{
		node *q = p->next_;
		delete p;
		p = q;
	}
}

template<typename T>
size_t list<T>::size() const
{
	return siz_;
}

template<typename T>
bool list<T>::contains(const T &elem) const
{
        node *iter;
        
	for (iter = first_; iter != 0; iter = iter->next_)
		if (elem == iter->data_)
			return true;
	return false;
}

template<typename T>
bool list<T>::empty() const
{
	return first_ ? false : true;
}

template<typename T>
void list<T>::insert(const T &t)
{
	node *p = new node(t);
	p->next_ = first_;
	p->prev_ = 0;

	if (first_)
		first_->prev_ = p;
	first_ = p;
	if (!last_)
		last_ = p;

	siz_++;
}

template<typename T>
void list<T>::insert_after(const T &t, iterator const &it)
{
	node *nuevo = new node(t);
	node *actual = it.actual_;

	if (actual == 0)
	{
		if (first_ != 0)
			std::abort();
		first_ = nuevo;
		last_ = nuevo;
	}
	else
	{
		if (actual->next_ != 0)
			actual->next_->prev_ = nuevo;
		nuevo->next_ = actual->next_;
		nuevo->prev_ = actual;
		actual->next_ = nuevo;
		if (last_ == actual)
			last_ = nuevo;
	}

	siz_++;
}

template<typename T>
void list<T>::insert_before(const T &t, iterator const &it)
{
        node *nuevo = new node(t);
        node *actual = it.actual_;

        if (actual == 0)
        {
                if (first_ != 0)
                        std::abort();
                first_ = nuevo;
                last_ = nuevo;
        }
        else
        {
                if (actual->prev_ != 0)
                        actual->prev_->next_ = nuevo;
                nuevo->next_ = actual;
                nuevo->prev_ = actual->prev_;
                actual->prev_ = nuevo;
                if (first_ == actual)
                        first_ = nuevo;
        }

	siz_++;
}

template<typename T>
void list<T>::erase(const T &t)
{

        node *iter, *sig=0;

        for (iter = first_; iter != 0; iter = sig)
	{
		sig = iter->next_;
		if (t == iter->data_)
		{
			node *ant = iter->prev_;
			if (ant == 0)
				first_ = sig;
			else
				ant->next_ = sig;
			if (sig == 0)
				last_ = ant;
			else
				sig->prev_ = ant;
			delete iter;

			siz_--;
		}
	}
}

template<typename T>
void list<T>::destroy(iterator q)
{
   if (q.actual_)
	{
		node* paux = q.actual_;
		q = q.go_forward();
		delete paux;
		destroy (q);
    }
}


template<typename T>
typename list<T>::iterator list<T>::last() const
{
	return typename list<T>::iterator(last_);
}

template<typename T>
typename list<T>::iterator list<T>::first() const
{
        return typename list<T>::iterator(first_);
}

template<typename T>
list<T> const &list<T>::operator=(list const &orig)
{
	node *iter;
	node *sig;
	node *ant;

	if (this != &orig)
	{
		for (iter = first_; iter != 0; )
		{
			sig = iter->next_;
			delete iter;
			iter = sig;	
		}

		first_ = 0;
		last_ = 0;

		for (iter = orig.first_, ant = 0; iter != 0; iter = iter->next_)
		{
			node *nuevo = new node(iter->data_);
			nuevo->prev_ = ant;
			nuevo->next_ = 0;
			if (ant != 0)
				ant->next_ = nuevo;
			if (first_ == 0)
				first_ = nuevo;
			ant = nuevo;
		}
		last_ = ant;
		siz_ = orig.siz_;
	}

	return *this;
}

#endif
