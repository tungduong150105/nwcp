std::vector p(n, std::vector<std::pair<int, int>> (1, {-1, 0}));
// p[u] = {{-x, t1}, {-y, t2}, .. {z, tn}}
// p[u].first < 0 when u is leader and > 0 when u not is a leader and after that, p[u] don't push additional value
// p[u].second > t it means u was updated at time t, so < t u must be a leader
auto find = [&](auto && self, int u, int t) -> int {
    if (p[u].back().first >= 0 && p[u].back().second <= t) {
        return self(self, p[u].back().first, t);
    }
    return u;
};
int timer = 0;
auto merge = [&](int u, int v) {
    ++timer;
    u = find(find, u, timer), v = find(find, v, timer);
    if (u == v) {
        return false;
    }
    if (p[u].back().first > p[v].back().first) {
        std::swap(u, v);
    }
    p[u].push_back({p[u].back().first + p[v].back().first, timer});
    p[v].push_back({u, timer});
    return true;
};
auto size = [&](int u, int t) {
    u = find(find, u, t);
    int lo = 0, hi = int(p[u].size());
    while (hi - lo > 1) {
        int mid = (lo + hi) / 2;
        if (p[u][mid].second <= t) {
            lo = mid;
        } else {
            hi = mid;
        }
    }
    return -p[u][lo].first;
};
