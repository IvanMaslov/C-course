//
// Created by maslov on 17.05.19.
//

#ifndef BIGINTEGER_VECTOR_OPT_H
#define BIGINTEGER_VECTOR_OPT_H

//#include "vector_opt.cpp"
#include <cstring>
#include <vector>


template<typename T>
class vector_opt {
    static const size_t max_short_size = 12;
    std::vector<T> *vec;
    size_t *vec_counter;
    size_t shortsize;
    T shortvector[max_short_size];

    void realloc_vector() {
        if (shortsize == max_short_size) { all_to_vec(); }
        if (vec != nullptr && vec->size() < max_short_size) { all_to_short(); }
    }

    void all_to_vec() {
        if (shortsize == 0)return;
        control_vec();
        if (vec == nullptr) {
            vec = new std::vector<T>();
            vec_counter = new size_t(1);
            for (size_t i = 0; i < shortsize; ++i)
                vec->push_back(shortvector[i]);
            shortsize = 0;
        }
    }

    void all_to_short() {
        if (vec == nullptr)return;
        control_vec();
        if (shortsize == 0 && vec->size() < max_short_size) {
            for (size_t i = 0; i < vec->size(); ++i) {
                shortvector[shortsize++] = vec->operator[](i);
            }
            if (--(*vec_counter) == 0) {
                delete vec;
                delete vec_counter;
            }
            vec = nullptr;
            vec_counter = nullptr;
        }
    }

    void control_vec() {
        if (vec == nullptr)
            return;
        if ((*vec_counter) > 1) {
            --(*vec_counter);
            vec = new std::vector<T>(*vec);
            vec_counter = new size_t(1);
        }
    }

public:
    vector_opt() {
        vec = nullptr;
        vec_counter = nullptr;
        shortsize = 0;
        memset(shortvector, 0, sizeof(shortvector));
    }

    vector_opt(vector_opt const &src) {
        vec = src.vec;
        vec_counter = src.vec_counter;
        shortsize = src.shortsize;
        memcpy(shortvector, src.shortvector, sizeof(shortvector));
        if (vec_counter != nullptr)
            ++(*vec_counter);
    }

    vector_opt(size_t size) {
        if (size < max_short_size) {
            vec = nullptr;
            vec_counter = nullptr;
            shortsize = size;
            memset(shortvector, 0, sizeof(shortvector));
        } else {
            vec = new std::vector<T>(size);
            vec_counter = new size_t(1);
            shortsize = 0;
            memset(shortvector, 0, sizeof(shortvector));
        }
    }

    size_t size() const {
        if (vec != nullptr)
            return vec->size();
        return shortsize;
    }

    void resize(size_t newsize) {
        resize(newsize, T());
    }

    void resize(size_t newsize, T const &elem) {
        control_vec();
        if (vec_counter != nullptr) {
            vec->resize(newsize, elem);
        } else {
            if (newsize < shortsize) {
                shortsize = newsize;
            } else {
                if (newsize >= max_short_size) {
                    all_to_vec();
                    if (vec == nullptr) {
                        vec = new std::vector<T>(newsize, elem);
                        vec_counter = new size_t(1);
                    } else {
                        vec->resize(newsize, elem);
                    }
                } else {
                    while (shortsize != newsize) {
                        shortvector[shortsize++] = elem;
                    }
                }
            }
        }
        realloc_vector();
    }

    T &operator[](size_t index) {
        control_vec();
        if (vec == nullptr && index < max_short_size) return shortvector[index];
        return vec->operator[](index);
    }

    const T &operator[](size_t index) const {
        if (vec == nullptr && index < max_short_size) return shortvector[index];
        return vec->operator[](index);
    }

    void assign(size_t size, T const &val) {
        /*control_vec();
        size_t old = this->size();
        resize(size, val);
        if (old < size)
            for (size_t i = 0; i < old; ++i)
                operator[](i) = val;*/
        resize(0);
        resize(size, val);
    }

    void push_back(T const &elem) { resize(size() + 1, elem); }

    void pop_back() { resize(size() - 1); }

    T &back() { return operator[](size() - 1); }

    const T &back() const { return operator[](size() - 1); }

    bool empty() const { return size() == 0; }

    vector_opt<T> &operator=(vector_opt<T> const &arg) {
        shortsize = arg.shortsize;
        memcpy(shortvector, arg.shortvector, sizeof(shortvector));
        if(vec_counter != nullptr){
            if(!(--(*vec_counter))){
                delete vec_counter;
                delete vec;
                vec = nullptr;
                vec_counter = nullptr;
            }
        }
        vec = arg.vec;
        vec_counter = arg.vec_counter;
        if (vec_counter != nullptr)
            ++(*vec_counter);
        return *this;
    }
    //vector_opt<T>&operator=(vector_opt<T> const& arg) = delete;

    ~vector_opt<T>() {
        if (vec_counter != nullptr && !(--(*vec_counter))) {
            delete vec_counter;
            delete vec;
        }
    }
};

#endif //BIGINTEGER_VECTOR_OPT_H

//
// Created by maslov on 17.05.19.
//

#define BIGINTEGER_VECTOR_OPT_Ht
#ifndef BIGINTEGER_VECTOR_OPT_Ht

#include <vector>


template<class T = unsigned char>
class vector_opt {
    std::vector<T> arg;

public:
    vector_opt() : arg() {}

    vector_opt(vector_opt const &src) : arg(src.arg) {}

    vector_opt(size_t size) : arg(size) {}

    size_t size() const { return arg.size(); }

    void resize(size_t newsize) { arg.resize(newsize); }

    void resize(size_t newsize, T const &elem) { arg.resize(newsize, elem); }

    T &operator[](size_t index) { return arg[index]; }

    const T &operator[](size_t index) const { return arg[index]; }

    void push_back(T const &elem) { arg.push_back(elem); }

    void pop_back(){ arg.pop_back();}

    T &back() { return arg.back(); }

    const T &back() const { return arg.back(); }

    void assign(size_t size, T const &val) { arg.assign(size, val); }

    bool empty() const{return arg.empty(); }

};

#endif //BIGINTEGER_VECTOR_OPT_Ht
