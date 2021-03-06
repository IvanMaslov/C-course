//
// Created by maslov on 14.04.19.
//

#ifndef BIGINTEGER_BIG_INTEGER_H
#define BIGINTEGER_BIG_INTEGER_H

#include <string>
#include <algorithm>


#include "vector_opt.h"

struct big_integer
{
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

    static const big_integer TEN;
    static const big_integer ONE;
    static const big_integer ZERO;
private:
    big_integer(bool sign, vector_opt<unsigned int> const& v);

    void clear_leading_zeroes();
    void abs_inc();
    void abs_dec();
    void abs_add(vector_opt<unsigned int> const& arg);
    void abs_sub(vector_opt<unsigned int> const& arg);
    void abs_mul(unsigned int arg);
    void shifted_abs_add(vector_opt<unsigned int> const& arg, int offset);
    void shifted_abs_sub(vector_opt<unsigned int> const& arg, int offset);
    void shifted_abs_mul(unsigned int arg, int offset);
    void reversed_abs_sub(vector_opt<unsigned int> const& arg);

    int compare(big_integer const& arg) const;
    int abs_compare(big_integer const& arg) const;
    int shifted_abs_compare(big_integer const& arg, int offset) const;

    big_integer put_sign() const;
    big_integer drop_sign();

    static big_integer valueOf(unsigned int e);

    vector_opt<unsigned int> v;
    bool sign;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);


#endif //BIGINTEGER_BIG_INTEGER_H
