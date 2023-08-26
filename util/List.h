//===---------------------------------------------------------===
//
// A highly efficient list implemented using an expandable
// array.
//
//===---------------------------------------------------------===

#include "mem.h"

#ifndef SSC_LIST_H
#define SSC_LIST_H

#include <memory>
#include "core_types.h"
#include "sys.h"

namespace ssc {

// Forward declaring because mem.h relies on List.h
ulen next_pow_of2(ulen);

template<typename T, typename Allocator = DynAllocator>
class List {
private:
    ulen capacity;
    ulen csize;
    T*   buckets;
    
    Allocator allocator;

    void free_buckets(T* old_buckets) {
        allocator.free(old_buckets);
    }

    void alloc_buckets(ulen new_capacity) {
        buckets = (T*) allocator.alloc(new_capacity * sizeof(T));
        capacity = new_capacity;
    }
        
      // Deallocates the existing buckets and creates new
      // buckets without copying over the old elements.
    void alloc_new_buckets(ulen new_capacity) {
        if (capacity >= new_capacity)
            return;
        T* old_buckets = buckets;
        alloc_buckets(new_capacity);
        free_buckets(old_buckets);
    }

    void grow() {
         if (capacity == 0) {
            // first allocation!
            alloc_buckets(1);
        } else {
            T* old_buckets=buckets;
            alloc_buckets(capacity << 1);
            // Should be safe to use memcpy here since we are now in control
            // of the memory of the buckets.
            memcpy(buckets, old_buckets, csize * sizeof(T));
            free_buckets(old_buckets);
        }

    }
    
    void destroy_range(T* p, T* e) {
        while (p != e)
            p->~T(), ++p;
    }
    
    void check_bounds(const T* itr) const {
        if (itr < buckets || itr >= buckets+csize)
            DBG_PANIC("list out of bounds");
    }

public:

    ~List() {
        if constexpr (!std::is_trivially_destructible_v<T>)
            destroy_range(begin(), end());
        free_buckets(buckets);
    }
    List(Allocator&& allocator = {}) :
        capacity(0), csize(0), buckets(nullptr),
        allocator(allocator)
    {}
    List(std::initializer_list<T> il) {
        alloc_buckets(next_pow_of2(il.size()));
        if constexpr (std::is_trivially_copyable_v<T>)
            memcpy(begin(), il.begin(), il.size() * sizeof(T));
        else
            std::uninitialized_copy(il.begin(), il.end(), begin());
        csize = il.size();
    }
      // copy constructor
    List(const List& rhs) {
        if (!rhs.empty()) {
            alloc_buckets(next_pow_of2(rhs.size()));
            if constexpr (std::is_trivially_copyable_v<T>)
                memcpy(begin(), rhs.begin(), rhs.csize * sizeof(T));
            else
                std::uninitialized_copy(rhs.begin(), rhs.end(), begin());
        }
        csize = rhs.csize;
    }
      // move constructor
    List(List&& rhs) noexcept :
        csize(std::exchange(rhs.csize, 0)),
        capacity(std::exchange(rhs.capacity, 0)),
        buckets(std::exchange(rhs.buckets, nullptr)),
        allocator(std::exchange(rhs.allocator, {}))
    {}
     // copy assignment
    List& operator=(const List& rhs) {
          // avoid self-assignment.
        if (this == &rhs) return *this;

        if constexpr (std::is_trivially_copyable_v<T>) {
            // Trivially copyable so we can use memcpy.
            // 
            // NOTE: No need to call destructors here because
            // trivially copyable types are also trivially
            // destructible.

            // Expand the memory size if we need to.
            if (capacity < rhs.size())
               alloc_new_buckets(next_pow_of2(rhs.size()));
            
            memcpy(begin(), rhs.begin(), rhs.size() * sizeof(T));
            csize = rhs.size();
            return *this;
        }

        // Unfortunate, got to use copy constructors!

        // Checking if there is already enough pre-created elements
        // such that the elements can just be copied into the existing
        // elements.
        if (csize >= rhs.size()) {
            T* new_end = rhs.size() > 0
                       ? std::copy(rhs.begin(), rhs.end(), begin())
                       : begin();
            // Destroy the excess elements.
            destroy_range(new_end, end());

            csize = rhs.size();
            return *this;
        }

        // Checking to see if we need to expand our memory capacity.
        if (capacity < rhs.size())
            alloc_new_buckets(next_pow_of2(rhs.size()));
        else
            // Otherwise, going to copy to the existing elements.
            std::copy(rhs.begin(), rhs.begin()+csize, begin());

        // Now going to copy over the elements for the uninitialized memory.
        std::uninitialized_copy(rhs.begin()+csize, rhs.end(), begin()+csize);

        csize = rhs.size();
        return *this;
    }
      // move assignment
    List& operator=(List&& rhs) noexcept {
          // Avoid self-assignment.
        if (this == &rhs) return *this;

        // Checking if there is already enough pre-created elements
        // such that the memory can just be moved into the existing
        // elements.
        if (csize >= rhs.csize) {
            T* new_end = rhs.csize > 0
                       ? std::move(rhs.begin(), rhs.end(), begin())
                       : begin();
            // Destroy the excess elements.
            destroy_range(new_end, end());

            csize = rhs.size();
            rhs.clear();
            return *this;
        }
        
        // Checking to see if we need to expand our memory capacity.
        if (capacity < rhs.size())
            alloc_new_buckets(next_pow_of2(rhs.size()));
        else
            // Otherwise, going to copy to the existing elements.
            std::move(rhs.begin(), rhs.begin()+csize, begin());
        
        // Now going to move over the elements for the uninitialized memory.
        std::uninitialized_move(rhs.begin()+csize, rhs.end(), begin()+csize);
        
        csize = rhs.csize;
        return *this;
    }

    /// Get the number of elements in this list.
    ulen size() const { return csize; }

    /// Does this list contain no elements.
    bool empty() const { return csize == 0; }

      // begin/end iterators
    T* begin() { return buckets;         }
    T* end()   { return buckets + csize; }
      // begin/end const iterators
    const T* begin() const { return buckets;         }
    const T* end()   const { return buckets + csize; }
    
    /// Get the first element in the list.
    ///
    T& front() {
        DBG_ASSERT(!empty(), "cannot call front() on empty list");
        return *buckets;
    }
    const T& front() const {
        DBG_ASSERT(!empty(), "cannot call front() on empty list");
        return *buckets;
    }

    /// Get the last element in the list.
    ///
    T& back() {
        DBG_ASSERT(!empty(), "cannot call back() on empty list");
        return *(buckets + csize - 1);
    }
    const T& back() const {
        DBG_ASSERT(!empty(), "cannot call back() on empty list");
        return *(buckets + csize - 1);
    }

    /// Append the element to the end of the list and
    /// allocate memory is needed.
    ///
    void add(T&& elm) {
       if (csize == capacity) grow();
       ::new (end()) T(std::move(elm));
       ++csize;
    }

    /// Append the element to the end of the list and
    /// allocate memory is needed.
    ///
    void add(const T& elm) {
        if (csize == capacity) grow();
        ::new (end()) T(elm);
        ++csize;
    }

    /// Try and find a matching element in the list.
    /// 
    /// \return this->end() if no element is found.
    ///
    T* find(const T& elm) {
        return std::find(begin(), end(), elm);
    }
    template<class P>
    T* find_if(P predicate) {
        return std::find_if(begin(), end(), predicate);
    }
    
    T& operator[](ulen idx) {
        check_bounds(begin() + idx);
        return buckets[idx];
    }
    const T& operator[](ulen idx) const {
        check_bounds(begin() + idx);
        return buckets[idx];
    }

    bool operator==(const List& rhs) const {
        if (csize != rhs.csize) return false;
        return std::equal(begin(), end(), rhs.begin());
    }
    bool operator!=(const List& rhs) const = default;

    /// Removes the element at the given index.
    ///
    void remove_by_index(ulen idx) {
        remove(begin() + idx);
    }
    
    /// Removes the element given the iterator to
    /// an element.
    ///
    void remove(T* itr) {
        check_bounds(itr);
        if constexpr (!std::is_trivially_destructible_v<T>)
            itr->~T();
        if (itr != (end()-1))
            // Shift all the elements down to fill the whole
            // of the removed element.
            memcpy(itr, itr+1, ((end()-1)-itr) * sizeof(T));
        --csize;
    }

    /// Removes the last element of the list.
    ///
    void pop_back() {
        DBG_ASSERT(!empty(), "cannot pop an empty list");
        if constexpr (!std::is_trivially_destructible_v<T>)
            (end()-1)->~T();
        --csize;
    }

    /// Removes `n` elements from the end of the list.
    ///
    void pop_back_n(ulen n) {
        check_bounds(end() - n);
        if constexpr (!std::is_trivially_destructible_v<T>)
            destroy_range(end()-n, end());
        csize -= n;
    }

    /// Removes the first element of the list.
    ///
    void pop_front() {
        remove_by_index(0);
    }

    /// Removes `n` elements from the front of the list.
    ///
    void pop_front_n(ulen n) {
        check_bounds(begin()+n-1);
        destroy_range(begin(), begin()+n);
        memcpy(begin(), begin()+n, (csize-n) * sizeof(T));
        csize -= n;
    }

    /// Reserves memory of at least `size`.
    ///
    /// NOTE: This does not increase the size of the list.
    /// If you intend to access the elements after increasing
    /// the size, use resize().
    ///
    void reserve(ulen size) {
        if (capacity < size) {
            T* old_buckets = buckets;
            alloc_buckets(next_pow_of2(size));
            memcpy(buckets, old_buckets, csize*sizeof(T));
            free_buckets(old_buckets);
        }
    }
    
    /// Reserves memory and increases the size of this list.
    ///
    void resize(ulen size) {
        reserve(size);
        if (size > csize) {
            ulen diff=size-csize;
            // Call default constructors.
            for (T* p=end(), *e=end()+diff; p != e; ++p)
                ::new (p) T();
        } else if (size < csize) {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                ulen diff=csize-size;
                destroy_range(end(), end()+diff);
            }
        }
        csize = size;
    }

    /// Empties the list of all the elements but does not
    /// modify the amount of memory allocated.
    ///
    void clear() {
        destroy_range(begin(), end());
        csize = 0;
    }

    /// Trims the amount of allocated memory to
    /// accommodate for the size of the list.
    ///
    /// NOTE: This has the overhead of having to
    /// copy the new memory over and free the old.
    ///
    void trim() {
        ulen new_capacity = next_pow_of2(csize);
        if (new_capacity != capacity) {
            T* old_buckets = buckets;
            alloc_buckets(new_capacity);
            memcpy(buckets, old_buckets, csize*sizeof(T));
            free_buckets(old_buckets);
        }
    }
};
}

#endif
