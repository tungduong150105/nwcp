template <typename T> void debug_mat(const std::vector<std::vector<T>> &value) {
    for (int i = 0; i < (int) (value.size()); ++i) {
        for (int j = 0; j < (int) (value[0].size()); ++j) {
            std::cout << value[i][j] << " \n"[j == (int) (value[0].size() - 1)];
        }
    }
    std::cout << "---\n";
}

template <typename T> std::vector<std::vector<T>> mtmul(const std::vector<std::vector<T>> &lhs, const std::vector<std::vector<T>> &rhs) {
    std::vector res((int) (lhs.size()), std::vector<T> ((int) (rhs[0].size())));
    for (int i = 0; i < (int) (lhs.size()); ++i) {
        for (int k = 0; k < (int) (rhs.size()); ++k) {
            for (int j = 0; j < (int) (rhs[0].size()); ++j) {
                res[i][j] = res[i][j] + (lhs[i][k] * rhs[k][j]);
            }
        }
    }
    return res;
}

template <typename T> std::vector<std::vector<T>> mtpow(std::vector<std::vector<T>> lhs, int64_t exp) {
    std::vector res((int) (lhs.size()), std::vector<T> ((int) (lhs.size())));
    for (int i = 0; i < (int) (lhs.size()); ++i) {
        res[i][i] = 1;
    }
    for (; exp > 0; exp /= 2, lhs = mtmul(lhs, lhs)) {
        if (exp & 1) {
            res = mtmul(res, lhs);
        }
    }
    return res;
}