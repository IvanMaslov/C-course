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
    v.push_back(static_cast<unsigned int>(a < 0 ? -a : a));
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
        sign = true;
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
        if ((cond && sign) || (!cond && !sign)) {
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
    shifted_abs_add(arg, 0);
}


void big_integer::abs_sub(std::vector<unsigned int> const &arg) {
    shifted_abs_sub(arg, 0);
}

void big_integer::shifted_abs_add(std::vector<unsigned int> const &arg, int offset) {
    if (v.size() < offset + arg.size())
        v.resize(offset + arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = offset; t != v.size(); ++t) {
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

void big_integer::shifted_abs_sub(std::vector<unsigned int> const &arg, int offset) {
    if (v.size() < offset + arg.size())
        v.resize(offset + arg.size(), 0);
    unsigned long long cl = 0;
    for (size_t t = static_cast<size_t>(offset); t != v.size(); ++t) {
        if (offset + arg.size() > t)
            cl = arg[t - offset] + cl;
        else if (!cl) break;
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
    return *this;
}


big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer ans;
    for (size_t i = 0; i < rhs.v.size(); ++i) {
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
    for (size_t i = 0; i < v.size(); ++i) {
        unsigned int d = static_cast<unsigned int>(t);
        t = v[i];
        t *= arg;
        t += d;
        v[i] = static_cast<unsigned int>(t & 0xffffffff);
        t >>= 32;
    }
    if (t) v.push_back(static_cast<unsigned int>(t));
}

big_integer &big_integer::operator<<=(int rhs) {
    std::vector<unsigned int> new_v;
    new_v.assign(rhs / 32, 0);
    unsigned long long s = 0;
    for (auto i : v) {
        s += static_cast<unsigned long long>(i) << (rhs & 31);
        new_v.push_back(static_cast<unsigned int>(s & 0xffffffff));
        s >>= 32;
    }
    if (s)
        new_v.push_back(static_cast<unsigned int>(s & 0xffffffff));
    swap(new_v, v);
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
    v[0] >>= rhs;
    for (size_t i = 1; i != v.size(); ++i) {
        v[i - 1] |= (v[i] & ((1 << rhs) - 1)) << (32 - rhs);
        v[i] >>= rhs;
    }
    clear_leading_zeroes();
    if (sign) abs_inc();
    return *this;
}

big_integer big_integer::put_sign() const {
    big_integer ans(*this);
    if (!sign) {
        ans.v.push_back(0);
    } else {
        ans.sign = false;
        for (auto &i : ans.v)
            i = ~i;
        ans.abs_inc();
        ans.v.push_back(0xffffffff);
    }
    return ans;
}

big_integer big_integer::drop_sign() {
    if (v.back() & (1 << 31)) {
        sign = true;
        for (auto &i : v)
            i = ~i;
        abs_inc();
    }
    clear_leading_zeroes();
    return (*this);
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

/*
big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool ans_sign = sign ^ rhs.sign;
    sign = false;
    big_integer ans;

    while (this->abs_compare(rhs) >= 0) {
        unsigned long long rax;
        int offset;
        if (v.size() > rhs.v.size()) {
            rax = (1ull << 32) * v.back() + v[v.size() - 2];
            offset = static_cast<int>(v.size() - rhs.v.size() - 1);
        } else {
            rax = v.back();
            offset = 0;
        }
        rax /= (rhs.v.back());
        if (rax >= (1ull << 32)) {
            ++offset;
            rax = v.back() / (rhs.v.back());
        }
        if (rax <= 2) rax = 1;
        else rax -= 2;

        big_integer e(false, rhs.v);
        e *= valueOf(rax);
        unsigned long outofsize = e.v.size();
        while(true) {
            if(outofsize < e.v.size())goto end;
            for (int u = static_cast<int>(e.v.size()) - 1; u >= 0; --u)
                if (v[v.size() + u - e.v.size()] < e.v[u])
                    goto end;
                else if(v[v.size() + u - e.v.size()] > e.v[u]) break;
            e.abs_add(rhs.v);
            ++rax;
        }
        end:
        --rax;
        e.abs_sub(rhs.v);
        this->shifted_abs_sub(e.v, offset);

        ans.shifted_abs_add(valueOf(rax).v, offset);
    }

    (*this) = ans;
    sign = ans_sign;
    return *this;
}
*/

/*
big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool ans_sign = sign ^ rhs.sign;
    sign = false;
    big_integer ans;

    while (this->abs_compare(rhs) >= 0) {
        unsigned long long rax;
        int offset;
        if (v.size() > rhs.v.size()) {
            rax = (1ull << 32) * v.back() + v[v.size() - 2];
            offset = static_cast<int>(v.size() - rhs.v.size() - 1);
        } else {
            rax = v.back();
            offset = 0;
        }
        rax /= (rhs.v.back());
        if (rax >= (1ull << 32)) {
            ++offset;
            rax = v.back() / (rhs.v.back());
        }
        if (rax <= 2) rax = 1;
        else rax -= 2;

        big_integer e(false, rhs.v);
        e *= valueOf(rax);
        unsigned long outofsize = e.v.size();
        while(true) {
            if(outofsize < e.v.size())goto end;
            for (int u = static_cast<int>(e.v.size()) - 1; u >= 0; --u)
                if (v[v.size() + u - e.v.size()] < e.v[u])
                    goto end;
                else if(v[v.size() + u - e.v.size()] > e.v[u]) break;
            e.abs_add(rhs.v);
            ++rax;
        }
        end:
        --rax;
        e.abs_sub(rhs.v);
        this->shifted_abs_sub(e.v, offset);

        ans.shifted_abs_add(valueOf(rax).v, offset);
    }

    (*this) = ans;
    sign = ans_sign;
    return *this;
}
*/



big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool ans_sign = sign ^ rhs.sign;
    sign = false;
    big_integer ans;

    big_integer& A = (*this);
    big_integer B = rhs;
    B.sign = false;

    int cnt = 0;
    unsigned int d = rhs.v.back();
    while(d < 0x80000000){
        ++cnt;
        d <<= 1;
    }
    B <<= cnt; A <<= cnt;
    int n = static_cast<int>(B.v.size());
    int m = static_cast<int>(A.v.size()) - static_cast<int>(B.v.size());
    ans.v.assign(static_cast<unsigned long>(m + 1), 0);

    big_integer c = B;
    c <<= (m * 32);
    if(c <= A) {
        ans.v[m] = 1;
        A -= c;
    } else {
        ans.v[m] = 0;
    }
    for(int j = m - 1; j >= 0; --j){

        unsigned long long rax = (static_cast<unsigned long long>(A.v[n + j]) << 32) + A.v[n + j - 1];
        rax /= B.v.back();
        if(rax > (0xffffffff)) rax = 0xffffffff;



        /*
        big_integer cB = B;
        cB *= valueOf(rax);

        cB <<= j * 32;
        B <<= j * 32;
        A -= cB;
        //A.shifted_abs_sub(cB.v, j);
        while(A < ZERO){
            //A.shifted_abs_add(B.v, j);
            A += B;
            --rax;
        }
        B >>= j * 32;
        */

        if(!rax){
            ans.v[j] = static_cast<unsigned int>(rax);
            continue;
        }
        big_integer cB = B;
        cB *= valueOf(rax);

        while(A.shifted_abs_compare(cB, j) < 0){
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
    big_integer e = (*this);
    (*this) /= rhs;
    (*this) *= rhs;
    e -= (*this);
    (*this) = e;
    return *this;
}

std::string to_string(big_integer const &a) {
    std::string ans;
    big_integer oae = a;
    while (big_integer::ZERO != oae) {
        ans.push_back(static_cast<char>((oae % big_integer::TEN).v[0] % 10 + '0'));
        oae /= 10;
    }
    if (ans.empty()) ans.push_back('0');
    if (a.sign) ans.push_back('-');
    reverse(ans.begin(), ans.end());
    return ans;
}

big_integer big_integer::valueOf(unsigned int e) {
    big_integer ans;
    ans += (e & 0xffff0000) >> 16;
    ans <<= 16;
    ans += (e & 0x0000ffff);
    return ans;
}

/*
int big_integer::compare(big_integer const &arg) const {
    if (sign != arg.sign) {
        if (v.size() == 1 && arg.v.size() == 1 && v[0] == 0 && arg.v[0] == 0)
            return 0;
        return arg.sign - sign;
    }
    if (v.size() != arg.v.size())
        return ((v.size() < arg.v.size()) ^ sign) ? -1 : 1;
    for (int i = static_cast<int>(v.size()) - 1; i >= 0; --i)
        if (v[i] != arg.v[i])
            return ((v[i] < arg.v[i]) ^ sign) ? -1 : 1;
    return 0;
}
*/



int big_integer::abs_compare(big_integer const &arg) const {
    if (v.size() != arg.v.size())
        return static_cast<int>(v.size()) - static_cast<int>(arg.v.size());
    for (int i = static_cast<int>(v.size()) - 1; i >= 0; --i)
        if (v[i] != arg.v[i])
            return v[i] < arg.v[i] ? -1 : 1;
    return 0;
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


