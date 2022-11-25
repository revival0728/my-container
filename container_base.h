#include <cstddef>
#include <stdexcept>
#include <cstring>

#ifndef CONTAINER_BASE_H
#define CONTAINER_BASE_H

namespace mylib {

using size_t = std::size_t;
using out_of_range = std::out_of_range;


template <class T> 
struct alloc_unit {
  alloc_unit<T> *left_node, *right_node;
  size_t data_size;
  T *data;

  alloc_unit() { left_node = right_node = data = nullptr; data_size = 0; }
  alloc_unit(size_t _data_size, alloc_unit<T> *_left_node, alloc_unit<T> *_right_node) {
    data = new T[_data_size];
    data_size = _data_size;
    left_node = _left_node;
    right_node = _right_node;
  }
  alloc_unit(const alloc_unit<T>& other) {
    data_size = other.data_size;
    data = new T[other.data_size];
    for(size_t i = 0; i < data_size; ++i) data[i] = other.data[i];
  }
  ~alloc_unit() { delete [] data; }
}; // struct alloc_unit


#define MEMEORY_ALLOCATOR_INFO(msg) std::string("mylib::memeory_allocator: ") + msg

template <class T>
class memory_allocator {
  private:
    size_t _malloc_size;

    void _alloc_range_check(size_t index) {
      if(index >= _malloc_size) throw out_of_range(
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
      if(_empty_alloc(alloc_size)) return;
      _alloc_range_check(index);
      alloc_unit<T> *new_node, *tmp = _head;
      for(size_t i = 0; i < index; ++i) tmp = tmp->right_node;
      new_node = new alloc_unit<T>(alloc_size, tmp->left_node, tmp);
      (tmp->left_node)->right_node = new_node;
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
      if(_malloc_size == 1) {
        delete _tail;
        delete _head;
        --_malloc_size;
        return;
      }
      _delete_empty_check();
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
