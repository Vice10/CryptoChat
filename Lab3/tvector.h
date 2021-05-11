#ifndef _AVECTOR_H
#define _AVECTOR_H



template <class itemType>
class tvector
{
public:

    // constructors/destructor
    tvector();                        // default constructor (size==0)
    explicit tvector(int size);      // initial size of vector is size
    tvector(int size, const itemType& fillValue); // all entries == fillValue
    tvector(const tvector& vec);   // copy constructor
    ~tvector();                       // destructor

  // assignment
    const tvector& operator = (const tvector& vec);

    // accessors
    int  length()   const;        // support for old programs, deprecated
    int  capacity() const;        // use in place of length()
    int  size()     const;        // # elements constructed/stored

  // indexing
    itemType& operator [ ] (int index);       // indexing with range checking
    const itemType& operator [ ] (int index) const; // indexing with range checking

  // modifiers
    void resize(int newSize);             // change size dynamically;
                                            // can result in losing values
    void reserve(int size);                 // reserve size capacity
    void push_back(const itemType& t);
    void pop_back();
    void clear();                           // size == 0, capacity unchanged

private:

    int  mySize;                            // # elements in array
    int  myCapacity;                        // capacity of array
    itemType* myList;                      // array used for storage
};

#include "tvector.cpp"
#endif

