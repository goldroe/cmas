#ifndef DARRAY_H
#define DARRAY_H

template <class T>
class DArray {
public:
    T *data;
    size_t length;
    size_t capacity;

    DArray() {
        data = nullptr;
        length = 0;
        capacity = 0;
    }

    void reserve(size_t num_elements) {
        data = (T *)malloc(num_elements * sizeof(T));
        length = capacity = num_elements;
    }
    
    void grow(size_t num_elements) {
        size_t new_cap = 0;
        while (new_cap < capacity + num_elements) {
            new_cap = 2 * new_cap + 1;
        }
        data = (T *)realloc(data, new_cap * sizeof(T));
        capacity = new_cap;
    }
    
    void push(T element) {
        if (length + 1 > capacity) {
            grow(1);
        }
        data[length] = element;
        length++;
    }

    T operator[](size_t index) {
        return data[index];
    }
};

#endif // DARRAY_H
