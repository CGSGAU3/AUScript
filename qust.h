#pragma once

/* Debug memory allocation support */
#ifndef NDEBUG 
# define _CRTDBG_MAP_ALLOC
# include <crtdbg.h>
# define SetDbgMemHooks() \
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | \
  _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))

/* Memory leak class */
static class __Dummy
{
public:
  /* Class constructor */
  __Dummy( )
  {
    SetDbgMemHooks();
  } /* End of '__Dummy' constructor */
} __ooppss;
#endif /* _DEBUG */ 

#ifdef _DEBUG 
# ifdef _CRTDBG_MAP_ALLOC 
#   define new new(_NORMAL_BLOCK, __FILE__, __LINE__) 
# endif /* _CRTDBG_MAP_ALLOC */ 
#endif /* _DEBUG */

#include <iostream>

/* Queue forward declaration */
template<typename Type>
class Queue;

/* Stack forward declaration */
template<typename Type>
class Stack;

/* Entry structure */
template<typename Type>
class Entry
{
  Type data;
  Entry *next;

  Entry( const Type &newData, Entry *newNext = nullptr ) :
    data(newData), next(newNext)
  {
  }

  friend class Queue<Type>;
  friend class Stack<Type>;
};

/* Queue "FIFO" class */
template<typename Type>
class Queue
{
  Entry<Type> *head, *tail;

public:

  /* Constructor */
  Queue( void ) : head(nullptr), tail(nullptr)
  {
  }

  /* Copy constructor */
  Queue( const Queue &other )
  {
    Entry<Type> *ptr = other.head;

    head = tail = nullptr;
    try
    {
      while (ptr != nullptr)
        put(ptr->data), ptr = ptr->next;
    }
    catch (...)
    {
      clear();
      throw std::exception("Unable to create copy queue!");
    }
  }

  /* Move-constructor */
  Queue( Queue &&other ) noexcept: 
    head(other.head), tail(other.tail)
  {
    other.head = other.tail = nullptr;
  }

  /* Assignment operator */
  Queue & operator =( const Queue &other )
  {
    if (this != &other)
    {
      Queue temp(other);
      std::swap(*this, temp);
    }
    return *this;
  }

  /* Move-assignment */
  Queue & operator =( Queue &&other ) noexcept
  {
    if (this != &other)
    {
      head = other.head;
      tail = other.tail;

      other.head = other.tail = nullptr;
    }
    return *this;
  }

  /* Put in the end (it doesn't even matter) of the queue */
  void put( const Type &data )
  {
    if (head == nullptr)
      head = tail = new Entry<Type>(data);
    else
      tail = tail->next = new Entry<Type>(data);
  }

  /* Get from the beginning of the queue */
  bool get( Type *oldData = nullptr )
  {
    if (head == nullptr)
      return false;

    Entry<Type> *old = head;

    if (oldData != nullptr)
      *oldData = old->data;
    head = head->next;
    delete old;
    return true;
  }

  /* Output queue to the cmdline */
  void display( void )
  {
    Entry<Type> *ptr = head;

    if (ptr == nullptr)
      std::cout << "<Empty queue>" << std::endl;

    while (ptr != nullptr)
    {
      std::cout << ptr->data
                << (ptr->next == nullptr ? '\n' : ' ');
      ptr = ptr->next;
    }

    std::cout.flush();
  }

  /* Get front element of the queue */
  const Type & front( void ) const
  {
    return head->data;
  }

  /* Check if queue is empty */
  bool empty( void ) const
  {
    return head == nullptr;
  }

  /* Clear method */
  void clear( void )
  {
    while (get());
  }

  /* Destructor */
  ~Queue( void )
  {
    clear();
  }
};

/* Stack "LIFO" class */
template<typename Type>
class Stack
{
  Entry<Type> *top;

public:

  /* Constructor */
  Stack( void ) : top(nullptr)
  {
  }

  /* Copy constructor */
  Stack( const Stack &other )
  {
    Entry<Type> *ptr = other.top;
    Entry<Type> **rePtr = &top;

    top = nullptr;
    try
    {
      while (ptr != nullptr)
      {
        *rePtr = new Entry<Type>(ptr->data);
        rePtr = &(*rePtr)->next;
        ptr = ptr->next;
      }
    }
    catch (...)
    {
      clear();
      throw std::exception("Unable to create copy stack!");
    }
  }

  /* Move-constructor */
  Stack( Stack &&other ) noexcept :
    top(other.top)
  {
    other.top = nullptr;
  }

  /* Assignment operator */
  Stack & operator =( const Stack &other )
  {
    if (this != &other)
    {
      Stack temp(other);
      std::swap(*this, temp);
    }
    return *this;
  }

  /* Move-assignment */
  Stack & operator =( Stack &&other ) noexcept
  {
    if (this != &other)
    {
      top = other.top;
      other.top = nullptr;
    }
    return *this;
  }

  /* Push to the beginning */
  void push( const Type &data )
  {
    top = new Entry<Type>(data, top);
  }

  /* Pop FROM the beginning */
  bool pop( Type *oldData = nullptr )
  {
    if (top == nullptr)
      return false;

    Entry<Type> *old = top;

    if (oldData != nullptr)
      *oldData = old->data;
    top = top->next;
    delete old;
    return true;
  }

  /* Same with queue method */
  void display( void )
  {
    Entry<Type> *ptr = top;

    if (ptr == nullptr)
      std::cout << "<Empty stack>" << std::endl;

    while (ptr != nullptr)
    {
      std::cout << ptr->data
                << (ptr->next == nullptr ? '\n' : ' ');
      ptr = ptr->next;
    }

    std::cout.flush();
  }

  /* Check if stack is empty */
  bool empty( void ) const
  {
    return top == nullptr;
  }

  /* Get front element of the stack */
  const Type & front( void ) const
  {
    return top->data;
  }

  /* Clear method */
  void clear( void )
  {
    while (pop());
  }

  /* Destructor */
  ~Stack( void )
  {
    clear();
  }
};