template <int MOD> int mod_inv_int(int a) {
    int b = MOD, u = 1, w = 0;
    while (b != 0) {
        int t = a / b;
        a -= t * b; std::swap(a, b);
        u -= t * w; std::swap(u, w);
    }
    assert(a == 1);
    if (u < 0) u += MOD;
    return u;
}

template <typename T> T mod_inv(T x) {
    assert(x.v != 0);
    T r;
    r.v = mod_inv_int<T::mod()>(x.v);
    return r;
}

template <typename T> T pow(T base, int64_t exp) {
    T res(1);
    while (exp > 0) {
        if (exp & 1) res *= base;
        base *= base;
        exp >>= 1;
    }
    return res;
}

template <int MOD> struct ModNum {
    int v;
    constexpr ModNum() : v(0) {}
    template <class T> constexpr ModNum(T x) : v(norm(x)) {}
    static constexpr int mod() { return MOD; }
    template <class T> static constexpr int norm(T x) {
        int64_t m = static_cast<int64_t>(MOD);
        int64_t y = static_cast<int64_t>(x % m);
        if (y < 0) y += m;
        return static_cast<int>(y);
    }
    ModNum& operator+=(const ModNum& rhs) {
        v += rhs.v;
        if (v >= MOD) v -= MOD;
        return *this;
    }
    ModNum& operator-=(const ModNum& rhs) {
        v -= rhs.v;
        if (v < 0) v += MOD;
        return *this;
    }
    ModNum& operator*=(const ModNum& rhs) {
        v = static_cast<int>((static_cast<__int128>(v) * rhs.v) % MOD);
        return *this;
    }
    ModNum& operator/=(const ModNum& rhs) {
        return *this *= mod_inv(rhs);
    }
    ModNum operator+() const {
        return *this;
    }
    ModNum operator-() const {
        return ModNum(v == 0 ? 0 : MOD - v);
    }
    friend ModNum operator+(ModNum lhs, const ModNum& rhs) {
        return lhs += rhs;
    }
    friend ModNum operator-(ModNum lhs, const ModNum& rhs) {
        return lhs -= rhs;
    }
    friend ModNum operator*(ModNum lhs, const ModNum& rhs) {
        return lhs *= rhs;
    }
    friend ModNum operator/(ModNum lhs, const ModNum& rhs) {
        return lhs /= rhs;
    }
    friend bool operator==(const ModNum& a, const ModNum& b) {
        return a.v == b.v;
    }
    friend bool operator!=(const ModNum& a, const ModNum& b) {
        return a.v != b.v;
    }
    friend std::istream& operator>>(std::istream& is, ModNum& x) {
        int64_t t; is >> t; x = ModNum(t); return is;
    }
    friend std::ostream& operator<<(std::ostream& os, const ModNum& x) {
        return os << x.v;
    }
};