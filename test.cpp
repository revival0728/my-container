#include <iostream>
#include "container_base.h"
using namespace std;

template<class T>
class test : protected mylib::memory_allocator<T> {
  private:
    const size_t alloc_size = 5;
  public:
    void travel_from_head() {
      mylib::alloc_unit<T> *tmp = this->_head;
      int count = 0;
      while(tmp != nullptr) {
        ++count;
        // cout << "alloc_unit<T> data size::" << tmp->data_size << '\n';
        tmp = tmp->right_node;
      }
      cout << "travel_from_head()::" << count << '\n';
    }
    void travel_from_tail() {
      mylib::alloc_unit<T> *tmp = this->_tail;
      int count = 0;
      while(tmp != nullptr) {
        ++count;
        // cout << "alloc_unit<T> data size::" << tmp->data_size << '\n';
        tmp = tmp->left_node;
      }
      cout << "travel_from_tail()::" << count << '\n';
    }
    void travel() {
      cout << "traveling..." << '\n';
      travel_from_head();
      travel_from_tail();
      cout << "_get_malloc_size()::" << this->_get_malloc_size() << '\n';
    }
    void set_value() {
      cout << "seting value..." << '\n';
      for(size_t i = 0; i < this->_head->data_size; ++i) {
        this->_head->data[i] = i;
      }
    }
    void custom() {
      this->_head->data[3] = -1;
    }
    void print_value() {
      cout << "printing value..." << '\n';
      for(size_t i = 0; i < this->_head->data_size; ++i) {
        cout << this->_head->data[i] << ' ';
      }
      cout << '\n';
    }
    void alloc_at(size_t index) {
      cout << "allocating at " << index << "...\n";
      this->_alloc_at(index, alloc_size);
    }
    void check_pointer() {
      cout << "traveling with pointer..." << '\n';
      cout << "head to tail: ";
      for(mylib::alloc_unit_pointer<T> begin(*this->_head, 0), end(*this->_tail, 0); begin._not_equal_to(end); begin._forward(1)) {
        cout << *begin._get_pointer_value() << ' ';
      }
      cout << '\n';
      cout << "tail to head: ";
      for(mylib::alloc_unit_pointer<T> begin(*this->_tail, 0), end(*this->_head, 0); begin._not_equal_to(end); begin._backward(1)) {
        cout << *begin._get_pointer_value() << ' ';
      }
      cout << '\n';
    }
    test() {
      cout << "allocating..." << '\n';
      this->_alloc_at(0, alloc_size);
      this->_alloc_at(1, alloc_size);
      this->_alloc_back(alloc_size);
      this->_alloc_back(alloc_size);
      this->_alloc_back(alloc_size);
      this->_delete_back();
      this->_alloc_at(1, alloc_size);
      travel();
      set_value();
      print_value();
      check_pointer();
    }
    test(const test<T>& other) : mylib::memory_allocator<T>(other) {
      cout << "testing copy..." << '\n';
      travel();
    }
    test<T> operator=(test<T> other) {
      this->_copy(other);
      return *this;
    }
};

int main() {
  test<int> a;
  test<int> b(a);
  b.custom();
  b.print_value();
  a.print_value();
  b.alloc_at(0);
  b.print_value();
  b.travel();
}
