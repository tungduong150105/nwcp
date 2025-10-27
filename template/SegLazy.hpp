template <class Info, class Tag> struct SegLazy {
    int n;
    std::vector<Info> tree;
    std::vector<Tag> lazy;
    SegLazy() {}
    SegLazy(int _n) {
        init(_n);
    }
    void init(int _n) {
        n = _n;
        tree.assign(4 * n, Info());
        lazy.assign(4 * n, Tag());
    }
    void apply(int v, int l, int r, const Tag &x) {
        tree[v].apply(x, l, r);
        lazy[v].apply(x);
    }
    void push_down(int v, int l, int r) {
        int m = (l + r) >> 1;
        apply(v << 1, l, m, lazy[v]);
        apply(v << 1 | 1, m, r, lazy[v]);
        lazy[v] = Tag();
    }
    void modify(int v, int l, int r, int idx, const Info &x) {
        if (r - l == 1) {
            tree[v] = tree[v] + x;
            return;
        }
        int m = (l + r) / 2;
        push_down(v, l, r);
        if (idx < m) {
            modify(v << 1, l, m, idx, x);
        } else {
            modify(v << 1 | 1, m, r, idx, x);
        }
        tree[v] = tree[v << 1] + tree[v << 1 | 1];
    }
    void range_update(int v, int l, int r, int lhs, int rhs, const Tag &x) {
        if (rhs <= l || lhs >= r) return;
        if (lhs <= l && r <= rhs) {
            apply(v, l, r, x);
        } else {
            push_down(v, l, r);
            int m = (l + r) >> 1;
            range_update(v << 1, l, m, lhs, rhs, x);
            range_update(v << 1 | 1, m, r, lhs, rhs, x);
            tree[v] = tree[v << 1] + tree[v << 1 | 1];
        }
    }
    Info range_query(int v, int l, int r, int lhs, int rhs) {
        if (rhs <= l || lhs >= r) return Info();
        if (lhs <= l && r <= rhs) return tree[v];
        push_down(v, l, r);
        int m = (l + r) >> 1;
        return range_query(v << 1, l, m, lhs, rhs) + range_query(v << 1 | 1, m, r, lhs, rhs);
    }
    void modify(int idx, const Info &x) {
        modify(1, 0, n, idx, x);
    }
    void range_update(int l, int r, const Tag &x) {
        range_update(1, 0, n, l, r, x);
    }
    Info range_query(int l, int r) {
        return range_query(1, 0, n, l, r);
    }
};
 
struct Tag {
    Z val = 1;
    void apply(const Tag &t) {
        val *= t.val;
    }
};
 
struct Info {
    Z val = 0;
    void apply(const Tag &t, int l, int r) {
        val *= t.val;
    }
};
 
Info operator+(const Info &lhs, const Info &rhs) {
    return {lhs.val + rhs.val};
}