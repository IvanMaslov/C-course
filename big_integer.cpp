//
// Created by maslov on 14.04.19.
//
#include "big_integer.h"

const big_integer big_integer::TEN = big_integer(10);
const big_integer big_integer::ONE = big_integer(1);
const big_integer big_integer::ZERO = big_integer(0);

big_integer::big_integer() {
    v.push_back(0);
    sign = false;
}

big_integer::big_integer(big_integer const &other) : v(other.v), sign(other.sign) { ; }

big_integer::big_integer(int a) {
    sign = a < 0;
    v.push_back(static_cast<unsigned int>(a < 0 ? -static_cast<unsigned int>(a) : a));
}

big_integer::big_integer(std::string const &str) {
    sign = false;
    v.push_back(0);
    if (str.empty() || str == "-0" || str == "0") return;
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

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (sign == rhs.sign)
        abs_add(rhs.v);
    else {
        sign = !sign;
        bool cond = *this < rhs;
        if ((cond && sign) || (!cond && !sign)) {
            sign = !sign;
            abs_sub(rhs.v);
        } else {
            reversed_abs_sub(rhs.v);
        }
    }
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    sign = !sign;
    (*this) += rhs;
    sign = !sign;
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer ans;
    big_integer d(false, v);
    for (size_t i = 0; i < rhs.v.size(); ++i) {
        d.v = v;
        d.abs_mul(rhs.v[i]);
        ans.shifted_abs_add(d.v, static_cast<int>(i));
    }
    ans.sign = sign ^ rhs.sign;
    (*this = ans);
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool ans_sign = sign ^rhs.sign;
    sign = false;
    big_integer ans;

    big_integer &A = (*this);
    big_integer B = rhs;
    B.sign = false;

    if(A < B){
        *this = ZERO;
        return *this;
    }

    int cnt = 0;
    unsigned int d = rhs.v.back();
    while (d < 0x80000000) {
        ++cnt;
        d <<= 1;
    }
    B <<= cnt;
    A <<= cnt;
    int n = static_cast<int>(B.v.size());
    int m = static_cast<int>(A.v.size()) - static_cast<int>(B.v.size());
    ans.v.assign(static_cast<unsigned long>(m + 1), 0);
    big_integer c = B;
    c <<= (m * 32);
    if (c <= A) {
        ans.v[m] = 1;
        A -= c;
    } else {
        ans.v[m] = 0;
    }
    for (int j = m - 1; j >= 0; --j) {
        if(static_cast<int>(A.v.size()) <= j + n){
            ans.v[j] = static_cast<unsigned int>(0);
            continue;
        }

        unsigned long long rax = (static_cast<unsigned long long>(A.v[n + j]) << 32) + A.v[n + j - 1];
        rax /= B.v.back();
        if (rax > (0xffffffff)) rax = 0xffffffff;

        if (!rax) {
            ans.v[j] = static_cast<unsigned int>(rax);
            continue;
        }
        big_integer cB(B);
        cB.abs_mul(static_cast<unsigned int>(rax));

        while (A.shifted_abs_compare(cB, j) < 0) {
            cB.abs_sub(B.v);
            --rax;
        }
        A.shifted_abs_sub(cB.v, j);


        ans.v[j] = static_cast<unsigned int>(rax);
    }

    (*this) = ans;
    sign = ans_sign;
    clear_leading_zeroes();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    big_integer e(*this);
    (*this) /= rhs;
    (*this) *= rhs;
    e -= (*this);
    (*this) = e;
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    big_integer l = (*this).put_sign();
    big_integer r = rhs.put_sign();
    if (l.v.size() < r.v.size())
        l.v.resize(r.v.size(), (l.sign ? 0xffffffff : 0));

    for (size_t i = 0; i < r.v.size(); ++i)
        l.v[i] &= r.v[i];

    *this = l.drop_sign();

    return *this;
}


big_integer &big_integer::operator|=(big_integer const &rhs) {
    big_integer l = (*this).put_sign();
    big_integer r = rhs.put_sign();
    if (l.v.size() < r.v.size())
        l.v.resize(r.v.size(), (l.sign ? 0xffffffff : 0));

    for (size_t i = 0; i < r.v.size(); ++i)
        l.v[i] |= r.v[i];

    *this = l.drop_sign();

    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    big_integer l = (*this).put_sign();
    big_integer r = rhs.put_sign();
    if (l.v.size() < r.v.size())
        l.v.resize(r.v.size(), (l.sign ? 0xffffffff : 0));

    for (size_t i = 0; i < r.v.size(); ++i)
        l.v[i] ^= r.v[i];

    *this = l.drop_sign();

    return *this;
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


big_integer &big_integer::operator--() {
    if (*this == ZERO)
        sign = true;
    if (sign) abs_inc();
    else abs_dec();
    return *this;
}

big_integer &big_integer::operator++() {
    if (!sign) abs_inc();
    else abs_dec();
    return *this;
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


big_integer &big_integer::operator<<=(int rhs) {
    vector_opt<unsigned int> new_v;
    new_v.assign(rhs / 32, 0);
    unsigned long long s = 0;
    //for (auto i : v) {
    for(size_t j = 0; j < v.size(); ++j){
        unsigned int i = v[j];
        s += static_cast<unsigned long long>(i) << (rhs & 31);
        new_v.push_back(static_cast<unsigned int>(s & 0xffffffff));
        s >>= 32;
    }
    if (s)
        new_v.push_back(static_cast<unsigned int>(s & 0xffffffff));
    std::swap(new_v, v);
    clear_leading_zeroes();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    for (size_t i = static_cast<size_t>(rhs / 32); i != v.size(); ++i) {
        v[i - rhs / 32] = v[i];
    }
    while (rhs >= 32 && !v.empty()) {
        rhs -= 32;
        v.pop_back();
    }
    clear_leading_zeroes();
    if(rhs) {
        v[0] >>= rhs;
        for (size_t i = 1; i != v.size(); ++i) {
            v[i - 1] |= (v[i] & (static_cast<unsigned int>(1 << rhs) - 1)) << (32 - rhs);
            v[i] >>= rhs;
        }
    }
    clear_leading_zeroes();
    if (sign) abs_inc();
    return *this;
}

big_integer::big_integer(bool sign, vector_opt<unsigned int> const &v) : v(v), sign(sign) { ; }

void big_integer::clear_leading_zeroes() {
    while (!v.empty() && !v.back()) v.pop_back();
    if (v.empty()) v.push_back(0);
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

void big_integer::abs_add(vector_opt<unsigned int> const &arg) {
    shifted_abs_add(arg, 0);
}

void big_integer::abs_sub(vector_opt<unsigned int> const &arg) {
    shifted_abs_sub(arg, 0);
}

void big_integer::shifted_abs_add(vector_opt<unsigned int> const &arg, int offset) {
    if (v.size() < static_cast<size_t>(offset) + arg.size())
        v.resize(offset + arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = static_cast<size_t>(offset); t != v.size(); ++t) {
        if (offset + arg.size() > t)
            cl = arg[t - offset] + cl;
        else if (!cl) break;
        cl += v[t];
        v[t] = static_cast<unsigned int>(cl & 0xffffffff);
        cl = cl >> 32;
    }
    if (cl) v.push_back(static_cast<unsigned int>(cl));
    clear_leading_zeroes();
}

void big_integer::shifted_abs_sub(vector_opt<unsigned int> const &arg, int offset) {
    if (v.size() < offset + arg.size())
        v.resize(offset + arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = static_cast<size_t>(offset); t != v.size(); ++t) {
        cl = (cl >> 32) & 1;
        if (offset + arg.size() > t)
            cl = arg[t - offset] + cl;
        else if (!cl) break;
        cl = v[t] - cl;
        v[t] = static_cast<unsigned int>(cl & 0xffffffff);
    }
    if(!cl) clear_leading_zeroes();
}

void big_integer::abs_mul(unsigned int arg) {
    shifted_abs_mul(arg, 0);
}

big_integer big_integer::put_sign() const {
    big_integer ans(*this);
    if (!sign) {
        ans.v.push_back(0);
    } else {
        ans.sign = false;
        for (size_t i = 0; i < ans.v.size(); ++i)
            ans.v[i] = ~ans.v[i];
        //for (auto &i : ans.v)
        //    i = ~i;
        ans.abs_inc();
        ans.v.push_back(0xffffffff);
    }
    return ans;
}

big_integer big_integer::drop_sign() {
    if (v.back() & (1 << 31)) {
        sign = true;
        for (size_t i = 0; i < v.size(); ++i)
            v[i] = ~v[i];
        //for (auto &i : v)
        //    i = ~i;
        abs_inc();
    }
    clear_leading_zeroes();
    return (*this);
}

big_integer big_integer::valueOf(unsigned int e) {
    big_integer ans(static_cast<int>(e >> 2));
    ans.v[0] <<= 2;
    ans.v[0] += e & 3;
    return ans;
}

int big_integer::abs_compare(big_integer const &arg) const {
    return shifted_abs_compare(arg, 0);
}

int big_integer::compare(big_integer const &arg) const {
    if (sign != arg.sign) {
        if (v.size() == 1 && arg.v.size() == 1 && v[0] == 0 && arg.v[0] == 0)
            return 0;
        return arg.sign - sign;
    }
    int u = this->abs_compare(arg);
    if (u == 0)return u;
    if (sign)return -u;
    return u;
}

int big_integer::shifted_abs_compare(big_integer const &arg, int offset) const {
    if (v.size() != offset + arg.v.size())
        return static_cast<int>(v.size()) - static_cast<int>(arg.v.size()) - offset;
    for (int i = static_cast<int>(v.size()) - 1; i >= offset; --i)
        if (v[i] != arg.v[i - offset])
            return v[i] < arg.v[i - offset] ? -1 : 1;
    return 0;
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


bool operator==(big_integer const &a, big_integer const &b) {
    return a.compare(b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return a.compare(b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return a.compare(b) < 0;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return a.compare(b) > 0;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return a.compare(b) <= 0;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return a.compare(b) >= 0;
}

std::string to_string(big_integer const &a) {
    std::string ans;
    big_integer oae = a;
    while (big_integer::ZERO != oae) {
        ans.push_back(static_cast<char>((oae % big_integer::TEN).v[0] + '0'));
        oae /= 10;
    }
    if (ans.empty()) ans.push_back('0');
    if (a.sign) ans.push_back('-');
    reverse(ans.begin(), ans.end());
    return ans;
}

void big_integer::shifted_abs_mul(unsigned int arg, int offset) {
    unsigned long long t = 0;
    for (size_t i = static_cast<size_t>(offset); i < v.size(); ++i) {
        unsigned int d = static_cast<unsigned int>(t);
        t = v[i];
        t *= arg;
        t += d;
        v[i] = static_cast<unsigned int>(t & 0xffffffff);
        t >>= 32;
    }
    if (t) v.push_back(static_cast<unsigned int>(t));
}

void big_integer::reversed_abs_sub(vector_opt<unsigned int> const &arg) {
    if(v.size() < arg.size())
        v.resize(arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = 0; t != arg.size(); ++t) {
        cl = arg[t] + cl - v[t];
        v[t] = static_cast<unsigned int>(cl & 0xffffffff);
        cl = (cl >> 32) & 1;
    }
    clear_leading_zeroes();
}


std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    s << to_string(a);
    return s;
}

big_integer::~big_integer() = default;
