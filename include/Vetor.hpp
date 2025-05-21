#ifndef VETOR_HPP
#define VETOR_HPP

#include <cstddef>
#include <stdexcept>

template<typename T>
class Vetor {
private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;

    void resize(std::size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
    Vetor() : data_(nullptr), size_(0), capacity_(0) {}

    explicit Vetor(std::size_t init_capacity)
        : data_(new T[init_capacity]), size_(0), capacity_(init_capacity) {}

    ~Vetor() {
        delete[] data_;
    }

    Vetor(const Vetor& other)
        : data_(new T[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    Vetor& operator=(const Vetor& other) {
        if (this != &other) {
            delete[] data_;
            data_ = new T[other.capacity_];
            size_ = other.size_;
            capacity_ = other.capacity_;
            for (std::size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            resize(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_] = value;
        ++size_;
    }

    void pop_back() {
        if (size_ == 0) {
            throw std::out_of_range("pop_back(): vetor vazio");
        }
        --size_;
    }

    T& operator[](std::size_t index) {
        if (index >= size_) {
            throw std::out_of_range("operator[]: índice fora dos limites");
        }
        return data_[index];
    }

    const T& operator[](std::size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("operator[]: índice fora dos limites");
        }
        return data_[index];
    }

    std::size_t size() const { return size_; }

    std::size_t capacity() const { return capacity_; }

    bool empty() const { return size_ == 0; }

    void clear() {
        size_ = 0;
    }
};

#endif // VETOR_H
