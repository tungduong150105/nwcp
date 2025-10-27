template <typename Info> struct SegmentTree {
    int n;
    std::vector<Info> info;
    SegmentTree() {}
    SegmentTree(int _n) {
        init(_n);
    }
    void init(int _n) {
        n = _n;
        info.resize(n << 1, Info());
    }
    void modify(int p, const Info &value) {
        for (info[p += n] = value; p >>= 1; ) {
            info[p] = info[p << 1] + info[p << 1 | 1];
        }
    }
    Info rangeQuery(int l, int r) {
        Info left_res = Info(), right_res = Info();
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                left_res = left_res + info[l++];
            }
            if (r & 1) {
                right_res = info[--r] + right_res;
            }
        }
        return left_res + right_res;
    }
    template<class F> int findFirst(int l, int r, F &&pred) {
        Info total = Info();
        std::vector<int> left_indices, right_indices;
        for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                left_indices.push_back(l++);
            }
            if (r & 1) {
                right_indices.push_back(--r);
            }
        }
        for (int i : left_indices) {
            if (pred(total + info[i])) {
                while (i < n) {
                    i <<= 1;
                    if (!pred(total + info[i])) {
                        total = total + info[i];
                        i |= 1;
                    }
                }
                return i - n;
            }
            total = total + info[i];
        }
        std::reverse(right_indices.begin(), right_indices.end());
        for (int i : right_indices) {
            if (pred(total + info[i])) {
                while (i < n) {
                    i <<= 1;
                    if (!pred(total + info[i])) {
                        total = total + info[i];
                        i |= 1;
                    }
                }
                return i - n;
            }
            total = total + info[i];
        }
        return -1;
    }
};