template <typename T> T mod_inv_in_range(T a, T m) {
	// assert(0 <= a && a < m);
	T x = a, y = m;
	// coeff of a in x and y
	T vx = 1, vy = 0;
	while (x) {
		T k = y / x;
		y %= x;
		vy -= k * vx;
		std::swap(x, y);
		std::swap(vx, vy);
	}
	assert(y == 1);
	return vy < 0 ? m + vy : vy;
}

template <typename T> T power(T a, int64_t b) {
	assert(b >= 0);
	T r = 1;
	while (b) {
		if (b & 1) r *= a; b >>= 1; a *= a;
	}
	return r;
}

template <typename tag> struct Mod {
private:
	static int _MOD;
	static uint64_t BARRETT_M;
public:
	static constexpr int const &MOD = _MOD;
	static void set_mod(int mod) {
		assert(mod > 0);
		_MOD = mod;
		BARRETT_M = (uint64_t(-1) / MOD);
	}
	static uint64_t barrett_reduce_partical(uint64_t a) {
		return uint32_t(a - uint64_t((__uint128_t(BARRETT_M) * a) >> 64) * MOD);
	}
	static int barrett_reduce(uint64_t a) {
		int32_t res = int32_t(barrett_reduce_partical(a) - MOD);
		return (res < 0 ? res + MOD : res);
	}
	struct mod_reader {
		friend std::istream &operator >> (std::istream &i, mod_reader) {
			int mod; i >> mod;
			Mod::set_mod(mod);
			return i;
		}
	};
	static mod_reader MOD_READER() {
		return mod_reader();
	}
private:
	int v;
public:
	Mod() : v(0) {}
	Mod(int64_t v_) : v(int(v_ % MOD)) {
		if (v < 0) v += MOD;
	}
	explicit operator int() const {
		return v;
	}
	friend std::ostream& operator << (std::ostream& out, const Mod& n) {
		return out << int(n);
	}
	friend std::istream& operator >> (std::istream& in, Mod& n) {
		int64_t v_; in >> v_; n = Mod(v_);
		return in;
	}
	friend bool operator == (const Mod& a, const Mod& b) {
		return a.v == b.v;
	}
	friend bool operator != (const Mod& a, const Mod& b) {
		return a.v != b.v;
	}

	Mod inv() const {
		Mod res;
		res.v = mod_inv_in_range(v, MOD);
		return res;
	}
	friend Mod inv(const Mod& m) {
		return m.inv();
	}
	Mod neg() const {
		Mod res;
		res.v = v ? MOD-v : 0;
		return res;
	}
	friend Mod neg(const Mod& m) {
		return m.neg();
	}
	Mod operator- () const {
		return neg();
	}
	Mod operator+ () const {
		return Mod(*this);
	}
	Mod& operator ++ () {
		v ++;
		if (v == MOD) v = 0;
		return *this;
	}
	Mod& operator -- () {
		if (v == 0) v = MOD;
		v --;
		return *this;
	}
	Mod& operator += (const Mod& o) {
		v -= MOD-o.v;
		v = (v < 0) ? v + MOD : v;
		return *this;
	}
	Mod& operator -= (const Mod& o) {
		v -= o.v;
		v = (v < 0) ? v + MOD : v;
		return *this;
	}
	Mod& operator *= (const Mod& o) {
		v = barrett_reduce(int64_t(v) * int64_t(o.v));
		return *this;
	}
	Mod& operator /= (const Mod& o) {
		return *this *= o.inv();
	}
	friend Mod operator ++ (Mod& a, int) {
		Mod r = a; ++a;
		return r;
	}
	friend Mod operator -- (Mod& a, int) {
		Mod r = a; --a;
		return r;
	}
	friend Mod operator + (const Mod& a, const Mod& b) {
		return Mod(a) += b;
	}
	friend Mod operator - (const Mod& a, const Mod& b) {
		return Mod(a) -= b;
	}
	friend Mod operator * (const Mod& a, const Mod& b) {
		return Mod(a) *= b;
	}
	friend Mod operator / (const Mod& a, const Mod& b) {
		return Mod(a) /= b;
	}
};
template <typename tag> int Mod<tag>::_MOD = 0;
template <typename tag> uint64_t Mod<tag>::BARRETT_M = 0;