#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template <class T, int size = 0>
class Pointer
{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise.
    */
    bool isArray;
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    static bool first; // true when first Pointer is created
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer()
    {
        Pointer(NULL);
    }
    Pointer(T*);
    // Copy constructor.
    Pointer(const Pointer &);
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*()
    {
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->()
    {
        return addr;
    }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i)
    {
        return addr[i];
    }
    // Conversion function to T *.
    operator T *()
    {
        return addr;
    }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin()
    {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end()
    {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize()
    {
        return refContainer.size();
    }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T,int size>
Pointer<T,size>::Pointer(T *t)
{
    std::cout<<"inside Pointer Constructor class"<<std::endl;
    typename std::list<PtrDetails<T> >::iterator p;
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    isArray = false;
    arraySize = 0;
    if(size > 1)
    {
        isArray = true;
        arraySize = size;
    }

    addr = t;
    std::cout<<"addr is: "<<addr<<" : "<<*addr<<"Is Array: "<<isArray<<std::endl ;
    //showlist();
    p = findPtrInfo(addr);
    if (p != refContainer.end())
    {
        p->refcount++;
    }
    else
    {
        if(arraySize)
        {
            p->isArray = true;
            p->arraySize = size;
        }
        //refContainer.insert(p,addr);
        PtrDetails<T> temp_ptr(addr, size);
        refContainer.push_back(temp_ptr);

    }
    showlist();
    std::cout<<"Array size: "<<p->arraySize<<"is array:"<<p->isArray<<"Addr:"<<p->memPtr<<std::endl;
    //<<"value: "<<*p<<std::endl;

}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob)
{

    std::cout<<"inside ptr copy constructor"<<std::endl;
    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab
    typename std::list<PtrDetails<T> >::iterator p;
    p = findPtrInfo(ob.addr);

    // TODO: Implement copy constructor
    isArray = ob.isArray;
    arraySize = ob.arraySize;
    addr = ob.addr;
    // increment ref count
    p->refcount++;

    // decide whether it is an array
    if(arraySize)
    {
        p->isArray = true;
        p->arraySize = size;
    }


}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer()
{

    // TODO: Implement Pointer destructor
    // Lab: New and Delete Project Lab

    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);

    // TODO: Finalize Pointer destructor
    // decrement ref count

    p->refcount--;
    std::cout<<"decrementing the ref count for"<<p->memPtr<<" Is Array: "<<p->isArray<<" "<<std::endl;

    // Collect garbage when a pointer goes out of scope.
    collect();

    // For real use, you might want to collect unused memory less frequently,
    // such as after refContainer has reached a certain size, after a certain number of Pointers have gone out of scope,
    // or when memory is low.
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect()
{

    // TODO: Implement collect function
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    bool memFreed = false;
    typename std::list<PtrDetails<T>>:: iterator p;

    do
    {
        for(p = refContainer.begin(); p != refContainer.end(); p++)
        {
            if (p->refcount != 0)
            {
                continue;
            }
            memFreed = true;
            refContainer.remove(*p);
            //refContainer.remove(p);

            if (p->memPtr)
            {
                if(p->isArray)
                {
                    delete [] p->memPtr;
                }
                else
                {
                    delete p->memPtr;
                }
            }
            break;
        }
    }
    while(p !=refContainer.end());

    // TODO: Implement collect function
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    return memFreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t)
{

    typename std::list<PtrDetails<T> >::iterator p;
    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab
    //std::cout<<"addr is: "<<addr<<" "<<*addr<<"and size is: "<<size<<std::endl;
    p = findPtrInfo(addr);
    if (p != refContainer.end())
    {
        std::cout<<"inside copy operator overload ="<<std::endl;
        p->refcount--;
    }
    addr = t;
    isArray = false;
    if(size > 1)
    {
        isArray = true;
    }
    p = findPtrInfo(addr);
    if (p != refContainer.end())
    {
        p->refcount++;
    }
    else
    {
        refContainer.insert(p,addr);
    }

    showlist();


    return t;

}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv)
{
    //std::cout<<"inside operator overload =ref"<<std::endl;
    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab
    typename std::list<PtrDetails<T> >::iterator p;
    // TODO: Implement assignment
    // First, decrement the reference count
    // for the memory currently being pointed to.
    //showlist();
    p = findPtrInfo(addr);
    if (p != refContainer.end())
    {
        std::cout<<"inside copy constructor using operator overload: "<<p->memPtr<<" "<<*p->memPtr<<std::endl;
        p->refcount--;
    }

    //showlist();

    // Then, ncrement the reference count of
    // the new address.
    p = findPtrInfo(rv.addr);
    p->refcount++;
    showlist();

    // increment ref count
    // store the address.
    //addr = rv.addr;
    //std::cout<<"addr is"<<addr<<std::endl;
    // return
    return rv ;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist()
{
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
                  << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr)
{
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown()
{
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}
