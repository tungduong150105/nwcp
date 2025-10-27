#include "bits/stdc++.h"

int main() {
    std::ios_base::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, m; std::cin >> n >> m;
    std::vector<int> curr(10);
    std::iota(curr.begin(), curr.end(), 0);
    std::vector<std::vector<int>> pref;
    pref.push_back(curr);
    while (n--) {
        int l, r; std::cin >> l >> r;
        std::swap(curr[l], curr[r]);
        pref.push_back(curr);
    }
    while (m--) {
        int l, r; std::cin >> l >> r;
        std::vector<int> p(10);
        std::iota(p.begin(), p.end(), 0);
        std::sort(p.begin(), p.end(), [&](const int &lhs, const int &rhs) {
            return pref[l - 1][lhs] < pref[l - 1][rhs];
        });
        for (int v : pref[r]) {
            std::cout << p[v] << " \n"[v == pref[r].back()];
        }
    }
    return 0;
}
