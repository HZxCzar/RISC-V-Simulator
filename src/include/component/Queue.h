#include "info/op_type.h"

namespace Czar {
template <class T, int SIZE> class Queue {
public:
  Queue() {
    size_ = SIZE;
    head_ = 0;
    tail_ = 0;
  }
  void push(T value) {
    if ((tail_ + 1) % size_ == head_) {
      throw("Queue is full");
    }
    data_[tail_] = value;
    tail_ = (tail_ + 1) % size_;
  }
  T pop() {
    if (head_ == tail_) {
      return T();
    }
    T value = data_[head_];
    head_ = (head_ + 1) % size_;
    return value;
  }
  bool empty() { return head_ == tail_; }
  bool full() { return (tail_ + 1) % size_ == head_; }
  T &front() { return data_[head_]; }
  T &operator[](int index) { return data_[(index) % size_]; }
  void clean() { head_ = tail_ = 0; }
  int size() { return (tail_ - head_ + size_) % size_; }
  int head() { return head_; }
  int tail() { return tail_; }

private:
  int size_ = 0;
  int head_ = 0;
  int tail_ = 0;
  T data_[SIZE];
};
} // namespace Czar