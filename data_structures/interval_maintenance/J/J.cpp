#include "bits/stdc++.h"

int main() {
    std::ios_base::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    int ans = 0, prv = 0;
    for (int i = 0; i < n; ++i) {
        int h; std::cin >> h;
        ans += std::max(0, h - prv);
        prv = h;
    }
    std::cout << ans << "\n";
    return 0;
}
