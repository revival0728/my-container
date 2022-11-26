#ifndef MVECTOR_H
#define MVECTOR_H

#include "container_base.h"
#include "iterator_base.h"

namespace mylib {

  namespace _use_memeory_allocator {

  template<class T, size_t default_alloc_size, size_t max_alloc_size>
  class _mvector : private memory_allocator<T> { // ( values... )
    private:
      size_t _mvector_size;

      void _constructor() 
        { _mvector_size = 0; this->_alloc_back(default_alloc_size); }
      void _copy_constructor(const _mvector<T, default_alloc_size, max_alloc_size>& other) 
        { _mvector_size = other._mvector_size(); this->_copy(other); }
      void _deconstructor() 
        { _mvector_size = 0; this->_free(); }

    public:
      class iterator : private random_access_iterator_base<T> {
        private:
          iterator(alloc_unit<T>& node, size_t index) : random_access_iterator_base<T>(node, index) {}

        public:
          iterator() : random_access_iterator_base<T>() {}
          iterator(const iterator& other) : random_access_iterator_base<T>(other) {}
          iterator operator=(const iterator other) { *this = iterator(other); }
          bool operator==(const iterator other) { this->equal_to(other); }
          bool operator!=(const iterator other) { this->not_equal_to(other); }
          T& operator*() { return this->get_value(); }
          T* operator->() { return this->get_address(); }
          iterator& operator++() { this->move(1); return *this; }
          iterator operator++(int) { iterator r(*this); this->move(1); return r; }
          iterator& operator--() { this->move(-1); return *this; }
          iterator operator--(int) { iterator r(*this); this->move(-1); return r; }
          iterator operator+(int n) { iterator r(*this); r.move(n); return r; }
          friend iterator operator+(int n, iterator it) { return (it + n); }
          iterator operator-(int n) { iterator r(*this); r.move(n); return r; }
          long long operator-(const iterator it) { this->differ(it); }
          bool operator<(const iterator it) { this->less_than(it); }
          bool operator>(const iterator it) { this->greater_than(it); }
          bool operator<=(const iterator it) { this->less_equal_to(it); }
          bool operator>=(const iterator it) { this->greater_equal_to(it); }
          iterator& operator+=(int n) { this->move(n); return *this; }
          iterator& operator-=(int n) { this->move(n); return *this; }
          T& operator[](int n) { iterator r(*this); r.move(n); return r.get_value(); }
      };
      iterator begin() { return iterator(this->_head, 1); }
      iterator end() { return iterator(this->_head, 0) + _mvector_size; }
      iterator rbegin() { return std::make_reverse_iterator(begin()); }
      iterator rend() { return std::make_reverse_iterator(end()); }
      _mvector() { _constructor(); }
      _mvector(const _mvector<T, default_alloc_size, max_alloc_size>& other) { _copy_constructor(other); }
      ~_mvector() { _deconstructor(); }

    private:
      iterator _tail_iterator() { return iterator(this->_tail, 0); }
      iterator _head_iterator() { return iterator(this->_head, 0); }
      void _ensure_alloc_size() {
        if(end() == _tail_iterator()) {
          this->_alloc_back(default_alloc_size);
        }
      }

    public:
      void push_back(const T value) {
        _ensure_alloc_size();
        *end() = value;
        ++_mvector_size;
      }
      void pop_back()  {--_mvector_size; }
      void insert(iterator it, const T value) {
        _ensure_alloc_size();
        for(iterator tmp = it+1; tmp != end(); ++tmp) *tmp = *(tmp-1);
        *it = value;
        ++_mvector_size;
      }
      void erase(iterator it) {
        for(; it != end(); ++it) *it = *(it+1);
        --_mvector_size;
      }
      size_t size() { return _mvector_size; }
  };

  }

}

#endif
