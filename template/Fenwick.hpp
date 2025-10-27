template <class T = int64_t>
struct Fenwick {
    int n;
    std::vector<T> bit;
    Fenwick() : n(0) {}
    explicit Fenwick(int _n) { init(_n); }
    void init(int _n) {
        n = _n;
        bit.assign(n, T{});
    }
    void add(int idx, T delta) {
        assert(0 <= idx && idx < n);
        for (int i = idx + 1; i <= n; i += i & -i) {
            bit[i - 1] = bit[i - 1] + delta;
        }
    }
    T sum_prefix(int r) const {
        assert(0 <= r && r < n);
        T res{};
        for (int i = r; i > 0; i -= i & -i) {
            res = res + bit[i - 1];
        }
        return res;
    }
    T sum(int l, int r) const {
        assert(0 <= l && l <= r && r < n);
        return sum_prefix(r) - sum_prefix(l);
    }
};
