#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <memory>

#ifndef CONTAINER_BASE_H
#define CONTAINER_BASE_H

namespace mylib {

using size_t = std::size_t;
using out_of_range = std::out_of_range;
using bad_weak_ptr = std::bad_weak_ptr;
using length_error = std::length_error;
using runtime_error = std::runtime_error;


#define ALLOC_UNIT_INFO(msg) std::string("mylib::alloc_unit: ") + msg

template <class T> 
class alloc_unit {
  private:
    void _data_size_check() {
      if(data_size == 0) throw length_error(
          ALLOC_UNIT_INFO("_data_size_check()::allocation size cannot be zero")
        );
    }

  public:
    alloc_unit<T> *left_node, *right_node;
    size_t data_size;
    T *data;

    alloc_unit() { 
      left_node = right_node = data = nullptr;
      data_size = 0; 
    }
    alloc_unit(size_t _data_size, alloc_unit<T> *_left_node, alloc_unit<T> *_right_node) {
      data = new T[_data_size];
      data_size = _data_size;
      left_node = _left_node;
      right_node = _right_node;
      _data_size_check();
    }
    alloc_unit(const alloc_unit<T>& other) {
      data_size = other.data_size;
      data = new T[other.data_size];
      for(size_t i = 0; i < data_size; ++i) data[i] = other.data[i];
    }
    ~alloc_unit() { delete [] data; }
}; // struct alloc_unit

#undef ALLOC_UNIT_INFO


#define ALLOC_UNIT_POINTER_INFO(msg) std::string("mylib::alloc_unit_pointer: ") + msg

template<class T>
class alloc_unit_pointer { // will break if container been modified (so as subclasses)
  private:
    void _pointer_valid_check() {
      if(_ptr == nullptr) throw bad_weak_ptr();
      if(_data_index >= _ptr->data_size) throw bad_weak_ptr();
    }
    void _operate_valid_check(alloc_unit_pointer<T> other) { // rough check
      if(_container_head != other._container_head) throw runtime_error(
          ALLOC_UNIT_POINTER_INFO("_operate_valid_check()::two pointer cannot have operation if they are in different conainter or at different time zone")
        );
    }
    size_t _count_distance_to_container_head() {
      alloc_unit<T> *tmp = _ptr;
      if(tmp->left_node == nullptr) return _data_index;
      size_t return_size = _data_index;
      tmp = tmp->left_node;
      while(tmp != nullptr) {
        return_size += tmp->data_size;
        tmp = tmp->left_node;
      }
      return return_size;
    }
    alloc_unit<T>* _get_container_head() {
      alloc_unit<T> *return_ptr = _ptr;
      while(return_ptr->left_node != nullptr) return_ptr = return_ptr->left_node;
      return return_ptr;
    }

  protected:
    size_t _data_index, _distance_to_container_head;
    alloc_unit<T> *_ptr, *_container_head;

  public:
    alloc_unit_pointer(alloc_unit<T>& node, size_t __data_index) {
      _ptr = &node;
      _data_index = __data_index;
      _distance_to_container_head = _count_distance_to_container_head();
      _container_head = _get_container_head();
      _pointer_valid_check();
    }
    alloc_unit_pointer(const alloc_unit_pointer<T>& other) {
      _ptr = other._ptr;
      _data_index = other._data_index;
      _pointer_valid_check();
    }
    ~alloc_unit_pointer() { }
    alloc_unit_pointer _forward(size_t step) {
      _data_index += step;
      while(_data_index >= _ptr->data_size) {
        _data_index -= _ptr->data_size;
        _ptr = _ptr->right_node;
      }
      _pointer_valid_check();
      return *this;
    }
    alloc_unit_pointer _backward(size_t step) {
      while(_data_index < step) {
        step -= _data_index;
        _ptr = _ptr->left_node;
        _data_index = _ptr->data_size;
      }
      _data_index -= step;
      _pointer_valid_check();
      return *this;
    }
    T& _get_pointer_value() { return *(_ptr->data + _data_index); }
    bool _equal_to(const alloc_unit_pointer<T> auptr) {
      return (
        _data_index == auptr._data_index &&
        _ptr->data_size == auptr._ptr->data_size &&
        _ptr->data == auptr._ptr->data &&
        _ptr->left_node == auptr._ptr->left_node &&
        _ptr->right_node == auptr._ptr->right_node
      );
    }
    bool _not_equal_to(const alloc_unit_pointer<T> auptr) {
      return !(this->_equal_to(auptr));
    }
    long long _differ(const alloc_unit_pointer<T> auptr) {
      _operate_valid_check();
      return _distance_to_container_head - auptr._distance_to_container_head;
    }
};

#undef ALLOC_UNIT_POINTER_INFO


#define MEMEORY_ALLOCATOR_INFO(msg) std::string("mylib::memeory_allocator: ") + msg

template <class T>
class memory_allocator {
  private:
    size_t _malloc_size;

    void _alloc_range_check(size_t index) {
      if(index > _malloc_size) throw out_of_range(
          MEMEORY_ALLOCATOR_INFO("_alloc_range_check()::allocate out of range")
        );
    }
    void _delete_empty_check() {
      if(_malloc_size == 0) throw out_of_range(
          MEMEORY_ALLOCATOR_INFO("_delete_empty_check()::try to delete node in an empty mylib::memery_allocator")
        );
    }
    void _initial_alloc(size_t alloc_size) {
      _tail = new alloc_unit<T>(1, nullptr, nullptr);
      _head = new alloc_unit<T>(alloc_size, nullptr, nullptr);
      _tail->left_node = _head;
      _head->right_node = _tail;
      ++_malloc_size;
    }
    // to check is data empty. if data is empty, than call _initial_alloc() and return true. else return fales.
    bool _empty_alloc(size_t alloc_size) { 
      if(_malloc_size == 0) {
        _initial_alloc(alloc_size);
        return true;
      }
      return false;
    }
    void _initialize() { _head = _tail = nullptr; _malloc_size = 0; }

  protected:
    alloc_unit<T> *_head, *_tail;

  public:
    memory_allocator() { _initialize(); }
    size_t _get_malloc_size() { return _malloc_size; }
    void _alloc_at(size_t index, size_t alloc_size) {
      _alloc_range_check(index);
      if(_empty_alloc(alloc_size)) return;
      alloc_unit<T> *new_node, *tmp = _head;
      for(size_t i = 0; i < index; ++i) tmp = tmp->right_node;
      new_node = new alloc_unit<T>(alloc_size, tmp->left_node, tmp);
      if(index != 0) (tmp->left_node)->right_node = new_node;
      else _head = new_node;
      tmp->left_node = new_node;
      ++_malloc_size;
    }
    void _alloc_back(size_t alloc_size) {
      if(_empty_alloc(alloc_size)) return;
      alloc_unit<T> *new_node = new alloc_unit<T>(alloc_size, _tail->left_node, _tail);
      (_tail->left_node)->right_node = new_node;
      _tail->left_node = new_node;
      ++_malloc_size;
    }
    void _delete_back() {
      _delete_empty_check();
      if(_malloc_size == 1) {
        delete _tail;
        delete _head;
        --_malloc_size;
        return;
      }
      alloc_unit<T> *tmp = _tail->left_node;
      (tmp->left_node)->right_node = _tail;
      _tail->left_node = tmp->left_node;
      --_malloc_size;
    }
    void _free() {
      if(_malloc_size == 0) return;
      if(_malloc_size == 1) {
        delete _tail;
        delete _head;
        _malloc_size = 0;
        return;
      }
      alloc_unit<T> *tmp = _head->right_node;
      while(true) {
        if(tmp->right_node == nullptr) {
          delete tmp;
          break;
        }
        delete tmp->left_node;
        tmp = tmp->right_node;
      }
      _malloc_size = 0;
    }
    void _copy(const memory_allocator<T>& other) {
      _free();
      alloc_unit<T> *other_tmp = other._head;
      for(size_t i = 0; i < other._malloc_size; ++i) {
        const size_t alloc_size = other_tmp->data_size;
        _alloc_back(alloc_size);
        for(size_t j = 0; j < alloc_size; ++j) {
          (_tail->left_node)->data[j] = other_tmp->data[j];
        }
        other_tmp = other_tmp->right_node;
      }
    }
    ~memory_allocator() { _free(); }
    memory_allocator(const memory_allocator<T>& other) { _initialize(); _copy(other); }
};

#undef MEMEORY_ALLOCATOR_INFO

} // namespace mylib

#endif
