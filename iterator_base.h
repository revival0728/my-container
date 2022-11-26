#ifndef ITERATOR_BASE_H
#define ITERATOR_BASE_H

#include <iterator>
#include "container_base.h"

namespace mylib {


#define ITERATOR_BASE_INFO(msg) std::string("mylib::iterator_base: ") + msg

template<class T>
class iterator_base : private alloc_unit_pointer<T> {
  public:
    iterator_base() : alloc_unit_pointer<T>(new alloc_unit<T>(1, nullptr, nullptr), 0) {}
    iterator_base(alloc_unit<T>& node, size_t index) : alloc_unit_pointer<T>(node, index) {}
    iterator_base(const iterator_base<T>& other) : alloc_unit_pointer<T>(other) {}
    ~iterator_base() {}
    iterator_base<T>& move(long long step) { 
      try {
        return step >= 0 ? this->_forward(step) : this->_backward(-step);
      } catch(const bad_weak_ptr& err) {
        throw out_of_range(
          ITERATOR_BASE_INFO("move()::iterator out of range")
        );
      }
    }
    T& get_value() { return *this->_get_pointer_value(); }
    T* get_address() { return this->_get_pointer_value(); }
    long long differ(const iterator_base<T> other) { return this->_differ(other); }
    bool equal_to(const iterator_base<T> other) { return this->_equal_to(other); }
    bool not_equal_to(const iterator_base<T> other)  { return this->_not_equal_to(other); }
    bool greater_than(const iterator_base<T> other) { this->_greater_than(other); }
    bool greater_equal_to(const iterator_base<T> other) { this->_greater_equal_to(other); }
    bool less_than(const iterator_base<T> other) { this->_less_than(other); }
    bool less_equal_to(const iterator_base<T> other) { this->_less_equal_to(other); }
};

#undef ITERATOR_BASE_INFO

template<class T>
class random_access_iterator_base : protected iterator_base<T>, public std::iterator<std::random_access_iterator_tag, T> {
  public:
    random_access_iterator_base() : iterator_base<T>() {}
    random_access_iterator_base(alloc_unit<T>& node, size_t index) : iterator_base<T>(node, index) {}
    random_access_iterator_base(const random_access_iterator_base<T>& other) : iterator_base<T>(other) {}
    ~random_access_iterator_base() {}
};

}

#endif
