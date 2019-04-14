//
// Created by maslov on 14.04.19.
//
#include "big_integer.h"

/*
const big_integer big_integer::TEN = big_integer(10);
const big_integer big_integer::ONE = big_integer(1);
const big_integer big_integer::ZERO = big_integer(0);


big_integer::big_integer() {
    v.push_back(0);
    sign = 0;
};

big_integer::big_integer(big_integer const &other) : v(other.v), sign(other.sign) { ; }

big_integer::big_integer(int a) {
    sign = 0;
    if (a < 0) {
        sign = 1;
        a = -a;
    }
    v.push_back(a);
}

big_integer::big_integer(std::string const &str) {
    sign = 0;
    if (str.empty()) {
        v.push_back(0);
        return;
    }
    size_t t = 0;
    if (str[t] == '-') {
        sign = 1;
        t = 1;
    }
    while (t != str.size()) {
        this->operator*=(TEN);
        this->operator+=(big_integer(str[t] - '0'));
        ++t;
    }
}

big_integer::~big_integer() = default;


big_integer &big_integer::operator=(big_integer const &other) {
    sign = other.sign;
    v = other.v;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (v.size() < rhs.v.size())
        v.resize(rhs.v.size(), 0);
    if (sign == rhs.sign) {
        unsigned long long cl = 0;
        for (size_t t = 0; t != v.size(); ++t) {
            if (rhs.v.size() > t)
                cl = rhs.v[t] + cl;
            cl += v[t];
            v[t] = static_cast<unsigned int>(cl & 0xffffffff);
            cl = cl >> 32;
        }
        if (cl) v.push_back(static_cast<unsigned int>(cl));
    } else {
        unsigned long long cl = 0;
        for (size_t t = 0; t != v.size(); ++t) {
            if (rhs.v.size() > t)
                cl = rhs.v[t] + cl;
            cl = v[t] - cl;
            v[t] = static_cast<unsigned int>(cl & 0xffffffff);
            cl = (cl >> 32) & 1;
        }
        if (cl) v.push_back(static_cast<unsigned int>(cl));
    }
    clear_leading_zeroes();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    sign = !sign;
    operator+=(rhs);
    sign = !sign;
    return *this;
}

void big_integer::clear_leading_zeroes() {
    while(v.back() == 0) v.pop_back();
    if(v.empty()) v.push_back(0);
}

std::string to_string(big_integer const &a) {
    std::string ans;
    if(a.sign)ans.push_back('-');
    unsigned int mod10 = 0;
    unsigned int mod10shift = 1;
    const unsigned int step_10_shitf = (1ll << 32) % 10;
    for(auto i : a.v){
        unsigned int u = i;
        while(u){
            mod10 += (mod10shift * (u % 10));
            u /= 10;
        }
    }

}
*/

const big_integer big_integer::TEN = big_integer(10);
const big_integer big_integer::ONE = big_integer(1);
const big_integer big_integer::ZERO = big_integer(0);


big_integer::big_integer() { v.push_back(0); sign = false; }

big_integer::big_integer(big_integer const &other) : v(other.v), sign(other.sign) { ; }

big_integer::big_integer(int a) {
    sign = a < 0;
    v.push_back(static_cast<unsigned int>(a < 0 ? -a : a));
}

big_integer::big_integer(std::string const &str) {
    sign = false;
    v.push_back(0);
    if (str.empty() || str == "-0") return;
    size_t t = 0;
    if (str[t] == '-') {
        sign = true;
        t = 1;
    }
    while (t != str.size()) {
        this->operator*=(TEN);
        this->operator+=(big_integer(str[t] - '0'));
        ++t;
    }
    if (str[0] == '-') sign = true;
}

big_integer &big_integer::operator=(big_integer const &other) {
    v = other.v;
    sign = other.sign;
}

big_integer big_integer::operator+() const {
    return big_integer(*this);
}

big_integer big_integer::operator-() const {
    return big_integer(!sign, v);
}

big_integer big_integer::operator~() const {
    big_integer res(*this);
    res.sign = !res.sign;
    --res;
    return res;
}

big_integer::big_integer(bool sign, std::vector<unsigned int> const &v) : v(v), sign(sign) { ; }

void big_integer::clear_leading_zeroes() {
    while (!v.empty() && !v.back()) v.pop_back();
    if (v.empty()) v.push_back(0);
}

big_integer &big_integer::operator--() {
    if (*this == ZERO)
        sign = 1;
    if (sign) abs_inc();
    else abs_dec();
    return *this;
}

void big_integer::abs_inc() {
    unsigned int t = 1;
    size_t i = 0;
    while (i != v.size() && t) {
        t = t + v[i];
        v[i] = t;
        if (!t) t = 1;
        else t = 0;
        ++i;
    }
    if (t) v.push_back(t);
}

void big_integer::abs_dec() {
    unsigned int t = 1;
    size_t i = 0;
    while (i != v.size() && t) {
        t = v[i] - t;
        v[i] = t;
        if (!(t + 1)) t = 1;
        else t = 0;
        ++i;
    }
    clear_leading_zeroes();
}

big_integer &big_integer::operator++() {
    if (!sign) abs_inc();
    else abs_dec();
    return *this;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.v.size() != b.v.size()) return false;
    for (size_t i = 0; i != a.v.size(); ++i)
        if (a.v[i] != b.v[i]) return false;
    return (a.sign == b.sign) || (a.v.size() == 1 && a.v[0] == 0);
}

big_integer big_integer::operator--(int) {
    big_integer c(*this);
    --*this;
    return c;
}

big_integer big_integer::operator++(int) {
    big_integer c(*this);
    ++*this;
    return c;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (sign == rhs.sign)
        abs_add(rhs.v);
    else {
        sign = !sign;
        bool cond = *this < rhs;
        if (cond && sign || !cond && !sign) {
            sign = !sign;
            abs_sub(rhs.v);
        } else {
            sign = !sign;
            big_integer ans = rhs;
            ans += *this;
            (*this) = ans;
        }
    }
    return *this;
}


void big_integer::abs_add(std::vector<unsigned int> const &arg) {
    if (v.size() < arg.size())
        v.resize(arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = 0; t != v.size(); ++t) {
        if (arg.size() > t)
            cl = arg[t] + cl;
        cl += v[t];
        v[t] = static_cast<unsigned int>(cl & 0xffffffff);
        cl = cl >> 32;
    }
    if (cl) v.push_back(static_cast<unsigned int>(cl));
    clear_leading_zeroes();
}


void big_integer::abs_sub(std::vector<unsigned int> const &arg) {
    if (v.size() < arg.size())
        v.resize(arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = 0; t != v.size(); ++t) {
        if (arg.size() > t)
            cl = arg[t] + cl;
        cl = v[t] - cl;
        v[t] = static_cast<unsigned int>(cl & 0xffffffff);
        cl = (cl >> 32) & 1;
    }
    clear_leading_zeroes();
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    sign = !sign;
    (*this) += rhs;
    sign = !sign;
}

bool operator<(big_integer const &a, big_integer const &b) {
    if(a.sign != b.sign)
        return b.sign < a.sign;
    if(a.v.size() != b.v.size())return a.v.size() < b.v.size();
    for (int i = static_cast<int>(a.v.size()) - 1; i >= 0; --i)
        if (a.v[i] != b.v[i]) return (a.v[i] < b.v[i]) ^ a.sign;
    return false;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer ans;
    for(size_t i = 0; i < rhs.v.size(); ++i){
        big_integer d(false, v);
        d.abs_mul(rhs.v[i]);
        d <<= 32 * i;
        ans += d;
    }
    ans.sign = sign ^ rhs.sign;
    (*this = ans);
    return *this;
}

void big_integer::abs_mul(unsigned int arg) {
    unsigned long long t = 0;
    for(size_t i = 0; i < v.size(); ++i){
        unsigned int d = static_cast<unsigned int>(t);
        t = v[i];
        t *= arg;
        t += d;
        v[i] = static_cast<unsigned int>(t & 0xffffffff);
        t >>= 32;
    }
    if(t) v.push_back(static_cast<unsigned int>(t));
}

big_integer &big_integer::operator<<=(int rhs) {
    std::vector<unsigned int> new_v;
    new_v.assign(rhs / 32, 0);
    unsigned long long s = 0;
    for(auto i : v){
        s += static_cast<unsigned long long>(i) << (rhs & 31);
        new_v.push_back(static_cast<unsigned int>(s & 0xffffffff));
        s >>= 32;
    }
    if(s)
        new_v.push_back(static_cast<unsigned int>(s & 0xffffffff));
    swap(new_v, v);
    clear_leading_zeroes();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    for(size_t i = static_cast<size_t>(rhs / 32); i != v.size(); ++i){
        v[i - rhs / 32] = v[i];
    }
    while(rhs > 32 && !v.empty()){
        rhs -= 32;
        v.pop_back();
    }
    clear_leading_zeroes();
    v[0] >>= rhs;
    for(size_t i = 1; i != v.size(); ++i){
        v[i - 1] |= (v[i] & ((1 << rhs) - 1)) << (32 - rhs);
        v[i] >>= rhs;
    }
    clear_leading_zeroes();
    if(sign) abs_inc();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    if (v.size() < rhs.v.size())
        v.resize(rhs.v.size(), 0);
    for(size_t i = 0; i < rhs.v.size(); ++i)
        v[i] ^= rhs.v[i];
    clear_leading_zeroes();
    sign ^= rhs.sign;
    return *this;
}


big_integer &big_integer::operator&=(big_integer const &rhs) {
    if (v.size() < rhs.v.size())
        v.resize(rhs.v.size(), 0);
    for(size_t i = 0; i < rhs.v.size(); ++i)
        v[i] &= rhs.v[i];
    clear_leading_zeroes();
    sign &= rhs.sign;
    return *this;
}


big_integer &big_integer::operator|=(big_integer const &rhs) {
    if (v.size() < rhs.v.size())
        v.resize(rhs.v.size(), 0);
    for(size_t i = 0; i < rhs.v.size(); ++i)
        v[i] |= rhs.v[i];
    clear_leading_zeroes();
    sign |= rhs.sign;
    return *this;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b <= a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return (a == b) || (a < b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return b <= a;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool ans_sign = sign ^ rhs.sign;
    sign = 0;
    int shift = (v.size() - rhs.v.size()) * 32 + 32;
    big_integer ans;
    while(shift >= 0){
        big_integer e = rhs;
        e.sign = 0;
        e <<= shift;
        if((*this) >= e){
            ans += (big_integer(ONE) <<= shift);
            (*this) -= e;
        }
        --shift;
    }
    (*this) = ans;
    sign = ans_sign;
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    bool ans_sign = sign;
    sign = 0;
    int shift = (v.size() - rhs.v.size()) * 32 + 32;
    while(shift >= 0){
        big_integer e = rhs;
        e.sign = 0;
        e <<= shift;
        if((*this) >= e){
            (*this) -= e;
        }
        --shift;
    }
    sign = ans_sign;
    return *this;
}

std::string to_string(big_integer const &a) {
    std::string ans;
    big_integer oae = a;
    while(big_integer::ZERO != oae){
        ans.push_back(static_cast<char>((oae % big_integer::TEN).v[0] % 10 + '0'));
        oae /= 10;
    }
    if(ans.empty()) ans.push_back('0');
    if(a.sign) ans.push_back('-');
    reverse(ans.begin(), ans.end());
    return ans;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    s << to_string(a);
    return s;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}


big_integer::~big_integer() = default;


