#include "typestring/typestring.hh"

#include <cstdio>
#include <tuple>
#include <type_traits>

namespace LogImpl {

	using namespace std;

	template <bool B, class T = void>
	using enable_if_t = typename enable_if<B, T>::type;

	template <char... Cs>
	using TS = irqus::typestring<Cs...>;


	// OhNo
	struct OhNo { using result_type = typestring_is(("OhNo")); };


	// StripTrailingUnderscores
	template <typename, typename, typename>
	struct STUI {};

	template <char... As, char... Bs>
	struct STUI<TS<As...>, TS<Bs...>, TS<>> {
		using result_type = TS<As...>;
	};

	template <char... As, char... Bs, char... Cs>
	struct STUI<TS<As...>, TS<Bs...>, TS<'_', Cs...>> {
		using result_type = typename STUI<
			TS<As...>,
			TS<Bs..., '_'>,
			TS<Cs...>
		>::result_type;
	};

	template <char... As, char... Bs, char C, char... Cs>
	struct STUI<TS<As...>, TS<Bs...>, TS<C, Cs...>> {
		using result_type = typename STUI<
			TS<As..., Bs..., C>,
			TS<>,
			TS<Cs...>
		>::result_type;
	};

	template <char... Cs>
	auto StripTrailingUnderscores(TS<Cs...>) ->
		typename STUI<TS<>, TS<>, TS<Cs...>>::result_type {
		return {};
	}

	// RemoveParamPairs
	template <typename, typename, typename, typename>
	struct RPPI { using result_type = OhNo; };

	template <char... As, char... Bs>
	struct RPPI<TS<As...>, TS<>, TS<>, TS<'(', Bs...>> {
		using result_type = typename RPPI<
			TS<As..., '('>,
			TS<>,
			TS<>,
			TS<Bs...>
		>::result_type;
	};

	template <char... As, char... Bs, char... Cs, char... Ds>
	struct RPPI<TS<As...>, TS<Bs...>, TS<Cs...>, TS<')', Ds...>> {
		using result_type = typename RPPI<
			TS<As...>,
			TS<Bs...>,
			TS<Cs..., ')'>,
			TS<Ds...>
		>::result_type;
	};

	template <char... As, char... Bs, char... Cs, char D, char... Ds>
	struct RPPI<TS<As...>, TS<Bs...>, TS<Cs...>, TS<D, Ds...>> {
		using result_type = typename RPPI<
			TS<As...>,
			TS<Bs..., Cs..., D>,
			TS<>,
			TS<Ds...>
		>::result_type;
	};

	template <typename, typename, typename>
	struct RPPIQQ;

	template <char... As, char... Bs, char... Cs>
	struct RPPIQQ<TS<'(', As...>, TS<Bs...>, TS<')', Cs...>> {
		using result_type = typename RPPIQQ<
			TS<As...>,
			TS<Bs...>,
			TS<Cs...>
		>::result_type;
	};

	template <char... As, char... Bs, char... Cs>
	struct RPPIQQ<TS<As...>, TS<Bs...>, TS<Cs...>> {
		using result_type = TS<As..., Bs..., Cs...>;
	};

	template <char... As, char... Bs, char... Cs>
	struct RPPI<TS<As...>, TS<Bs...>, TS<Cs...>, TS<>> {
		using result_type = typename RPPIQQ<
			TS<As...>,
			TS<Bs...>,
			TS<Cs...>
		>::result_type;
	};

	template <char... Cs>
	auto RemoveParamPairs(TS<Cs...>) ->
		typename RPPI<TS<>, TS<>, TS<>, TS<Cs...>>::result_type {
		return {};
	}


	// Strip
	template <typename, typename, typename>
	struct SSI {};

	template <char... Cs>
	struct SSI<TS<>, TS<>, TS<' ', Cs...>> {
		using result_type = typename SSI<
			TS<>,
			TS<>,
			TS<Cs...>
		>::result_type;
	};

	template <char... As, char... Bs, char... Cs>
	struct SSI<TS<As...>, TS<Bs...>, TS<' ', Cs...>> {
		using result_type = typename SSI<
			TS<As...>,
			TS<Bs..., ' '>,
			TS<Cs...>
		>::result_type;
	};

	template <char... As, char... Bs, char C, char... Cs>
	struct SSI<TS<As...>, TS<Bs...>, TS<C, Cs...>> {
		using result_type = enable_if_t<C != ' ', typename SSI<
			TS<As..., Bs..., C>,
			TS<>,
			TS<Cs...>
		>::result_type>;
	};

	template <typename Acc, typename Spaces>
	struct SSI<Acc, Spaces, TS<>> { using result_type = Acc; };

	template <typename Str>
	auto StripSpaces(Str) -> typename SSI<TS<>, TS<>, Str>::result_type {
		return {};
	}




	constexpr bool SIIsOpenParam(const char c) {
		return c == '(' || c == '[' || c == '{';
	}

	constexpr char SIMapParam(const char c) {
		return
			c == '(' ? ')' :
			c == '[' ? ']' :
			c == '{' ? '}' :
			'\0';
	}

	constexpr bool SIIsQuote(const char c) {
		return c == '"' || c == '\'';
	}

	template <typename, typename, typename>
	struct SI { using result_type = OhNo; };

	template <typename A, typename B, typename C>
	using SIT = typename SI<A, B, C>::result_type;


	template <typename Strs, typename Rem>
	struct SI<Strs, Rem, TS<>> {
		template <typename... IStrs, char C, char... Cs>
		static auto FuckMyLife(tuple<IStrs...>, TS<C, Cs...>)
			-> tuple<IStrs..., decltype(StripSpaces(TS<C, Cs...>()))>;

		static auto FuckMyLife(Strs, TS<>)
			-> Strs;

		using result_type = decltype(FuckMyLife(Strs(), Rem()));
	};

	template <typename, char, typename, typename>
	struct SIQ { using result_type = OhNo; };

	template <typename A, char B, typename C, typename D>
	using SIQT = typename SIQ<A, B, C, D>::result_type;

	template <char Q, typename A, char... Acc, char C, char... Cs>
	auto SIQSlurp(A, TS<Acc...>, TS<C, Cs...>)
		-> typename SIQ<A, Q, TS<Acc..., C>, TS<Cs...>>::result_type;


	template <typename Acc, typename Rem>
	struct WTFResult {
		using acc = Acc;
		using rem = Rem;
	};

	template <char, typename, typename>
	struct WTF { using result_type = WTFResult<OhNo, OhNo>; };

	template <char Q, char... Acc, char C, char... Cs>
	struct WTF<Q, TS<Acc...>, TS<C, Cs...>> {
		static constexpr bool Else(char d) {
			return !(d == Q || d == '\\');
		}

		template <char D, typename Ds, enable_if_t<D == Q, int> = 0>
		static auto WTFDaYo(TS<D>, Ds)
			-> WTFResult<TS<Acc..., C>, TS<Cs...>>;

		template <char D, char E, char... Es>
		static auto WTFDaYo(TS<'\\'>, TS<E, Es...>)
			-> typename WTF<Q, TS<Acc..., E>, TS<Es...>>::result_type;

		template <char D, typename Es, enable_if_t<Else(D), int> = 0>
		static auto WTFDaYo(TS<D>, Es)
			-> typename WTF<Q, TS<Acc..., C>, TS<Cs...>>::result_type;

		using result_type = decltype(WTFDaYo(TS<C>(), TS<Cs...>()));
	};


	template <typename, typename, typename, typename>
	struct SIP { using result_type = OhNo; };

	template <typename A, typename B, typename C, typename D>
	using SIPT = typename SIP<A, B ,C, D>::result_type;

	template <typename Result, typename Acc, typename Rem>
	struct SIP<Result, TS<>, Acc, Rem> {
		using result_type = SIT<Result, Acc, Rem>;
	};

	template <typename Result, char S, char... Ss, char... Acc, char C, char... Cs>
	struct SIP<Result, TS<S, Ss...>, TS<Acc...>, TS<C, Cs...>> {
		static constexpr bool Else(char d) {
			return !(S == d || SIIsQuote(d) || SIIsOpenParam(d));
		}

		template <char D, enable_if_t<S == D, int> = 0>
		static auto WTFDaYo(TS<D>)
			-> SIPT<Result, TS<Ss...>, TS<Acc..., D>, TS<Cs...>>;

		template <char D, enable_if_t<SIIsQuote(D), int> = 0,
			typename WTFRes = typename WTF<C, TS<Acc..., D>, TS<Cs...>>::result_type>
		static auto WTFDaYo(TS<D>)
			-> SIPT<Result, TS<S, Ss...>,
					typename WTFRes::acc, typename WTFRes::rem>;

		template <char D, enable_if_t<SIIsOpenParam(D), int> = 0>
		static auto WTFDaYo(TS<D>)
			-> SIPT<Result, TS<SIMapParam(C), S, Ss...>, TS<Acc..., D>, TS<Cs...>>;

		template <char D, enable_if_t<Else(D), int> = 0>
		static auto WTFDaYo(TS<D>)
			-> SIPT<Result, TS<S, Ss...>, TS<Acc..., D>, TS<Cs...>>;

		using result_type = decltype(WTFDaYo(TS<C>()));
	};

	template <typename... Strs, char... Acc, char C, char... Cs>
	struct SI<tuple<Strs...>, TS<Acc...>, TS<C, Cs...>> {
		static constexpr bool IsNormalChar(char c) {
			return !(c == ',' || SIIsQuote(c) || SIIsOpenParam(c));
		}

		template <char D, enable_if_t<IsNormalChar(D), int> = 0>
		static auto FML(TS<D>)
			-> SIT<tuple<Strs...>, TS<Acc..., D>, TS<Cs...>>;

		static auto FML(TS<','>)
			-> SIT<
				tuple<Strs..., decltype(StripSpaces(TS<Acc...>()))>,
				TS<>,
				TS<Cs...>
			>;

		template <char D, enable_if_t<SIIsQuote(D), int> = 0,
			typename WTFRes = typename WTF<C, TS<Acc..., D>, TS<Cs...>>::result_type>
		static auto FML(TS<D>)
			-> SIT<
				tuple<Strs...>,
				typename WTFRes::acc,
				typename WTFRes::rem
			>;

		template <char D, enable_if_t<SIIsOpenParam(D), int> = 0>
		static auto FML(TS<D>)
			-> SIPT<tuple<Strs...>, TS<SIMapParam(D)>, TS<Acc..., D>, TS<Cs...>>;

		using result_type = decltype(FML(TS<C>()));
	};


	template <char... Cs>
	auto Split(TS<Cs...>) -> SIT<tuple<>, TS<>, TS<Cs...>> {
		return {};
	}
	struct PrintingTuple {};

	void Print(PrintingTuple, tuple<>) {
	}

	template <typename S, typename... Ss>
	void Print(PrintingTuple, tuple<S, Ss...>) {
		printf("\t'%s',\n", S().data());
		Print(PrintingTuple{}, tuple<Ss...>());
	}

	template <typename Ss>
	void Print(Ss) {
		puts("[");
		Print(PrintingTuple{}, Ss());
		puts("]");
	}

	// Log
	template <typename Msg, typename File, typename ArgStr, typename... Args>
	void Log(const int line, Args&&... args) {
		Print(Split(ArgStr()));
	}
}

#define LOG(msg, ...) do											\
		{															\
			using Msg = typestring_is(msg);							\
			using File = typestring_is(__FILE__);					\
			using ArgStr = typestring_is(#__VA_ARGS__);				\
			LogImpl::Log<Msg, File, ArgStr>(__LINE__, __VA_ARGS__); \
		} while (0)

int main()
{
	int a;
	double b;
	int c[3] = {};
	int d = -1;

	LOG("WTF", a);
	LOG("WTF", a, b);
	LOG("WTF", a, b, "c");
	LOG("WTF", (d));
	LOG("WTF", (d), a);
	LOG("WTF", a, (d));
	LOG("WTF", a, b, "c", (d));
	LOG("WTF", a, b  , c[a]);
	/*
	LOG("WTF2", a, b, ((a))                , c[a], d);
	*/

	puts(LogImpl::RemoveParamPairs(typestring_is("((aa()))")()).data());
	puts(LogImpl::StripSpaces(typestring_is("         ((aa()))  ")()).data());

	return 0;
}
