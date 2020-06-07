#include <cstdio>

namespace LogImpl {

	// cause c++11 sucks

	// sequence of integers
	template <int... Ints>
	struct Seq { int length = sizeof...(Ints); };

	template <int K, int N, int... Acc>
	struct RangeImpl { using result_type = typename RangeImpl<K+1, N, Acc..., K>::result_type; };

	template <int N, int... Acc>
	struct RangeImpl<N, N, Acc...> { using result_type = Seq<Acc...>; };

	// sequence of N integers from 0 to N-1
	template <int N>
	using Range = typename RangeImpl<0, N>::result_type;


	// madness
	template <int N>
	class SCS
	{
		const char str_[N + 1];

	public:
		template <int N2, int... N3S>
		constexpr SCS(const char(&str)[N2], const Seq<N3S...>&)
			: str_{str[N3S]..., '\0'}
		{}

		template <int N2>
		constexpr SCS(const char(&str)[N2]) : SCS(str, Range<N>{}) {}

		template <int N1, int... N1S, int... N2S>
		constexpr SCS(const char (&a)[N1], const char (&b)[N-N1], const Seq<N1S...>&, const Seq<N2S...>&)
			: str_{a[N1S]..., b[N2S]..., '\0'}
		{}

		template <int N1, int... N1S, int... N2S, int N2=N-N1>
		constexpr SCS(const char (&a)[N1], const char (&b)[N2])
			: SCS(a, b, Range<N1>{}, Range<N2>{})
		{}

		constexpr char operator[] (std::size_t n) const { return str_[n]; }
		constexpr std::size_t Length() const { return N; }

		constexpr operator const char*() const { return &str_[0]; }

		template <int N2>
		constexpr SCS<N+N2> operator+ (const SCS<N2> &o) const { return SCS<N+N2>(*this, o); }

		static constexpr int PopBackLength(const int n) { return n > 0 ? n-1 : 0; }
		constexpr SCS<PopBackLength(N)> PopBack() const {
			return SCS<PopBackLength(N)>(str_);
		}
	};

	template <int N>
	constexpr int RTULen(const SCS<N> &s) {
		return
			N <= 0 ? 0 :
			s[N-1] == '_' ? RTULen(s.PopBack()) :
			s.Length();
	}

	template <int N, int M>
	// constexpr auto RemoveTrailingUnderscores(const SCS<N> &s) -> SCS<RTULen(s)> {
		constexpr auto RemoveTrailingUnderscores(const SCS<N> &s) -> SCS<M> {
		return
			N <= 0 ? s :
			s[N-1] == '_' ? RemoveTrailingUnderscores(s.PopBack()) :
			s;
	}

	constexpr int StrLen(const char* str) {
		return str[0] == '\0' ? 0 : 1 + StrLen(str+1);
	}

	template <int MS, int FS, int AS, typename... Args>
	void Log(
			 const char(&msg_)[MS],
			 const char (&file_)[FS],
			 const int line,
			 const char (&args_str_)[AS],
			 Args&&... args
	 ) {
		constexpr SCS<MS-1> msg(msg_);
		constexpr SCS<FS-1> file(file_);
		constexpr SCS<AS-1> args_str(args_str_);

		printf((const char*)RemoveTrailingUnderscores<AS-1,RTULen(args_str)>(args_str));
	}
}

#define LOG(msg, ...) LogImpl::Log(msg, __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)

/*
#define LOG(msg, ...) \
	do { \
	constexpr char LOG_IMPL ## __FILE__ ## __LINE__ ## [] = msg; \
	LogImpl::Log((LOG_IMPL ## __FILE__ ## __LINE__ ## []), __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__) \
		} while (0)
*/


int main()
{
	int a;
	double b;
	int c[3] = {};
	int d___ = -1;

	constexpr char wtf[] = "WTF";
	LOG(wtf, a, b, ((a))                , c[a], d___);

	return 0;
}
