#include "typestring/typestring.hh"

#include <cstdio>
#include <tuple>
#include <type_traits>

namespace LogImpl {

	// cause c++11 sucks

	// sequence of integers
	template <int... Ints>
	struct Seq { int length = sizeof...(Ints); };

	template <int K, int N, int... Acc>
	struct RangeImpl {
		using result_type = typename RangeImpl<K+1, N, Acc..., K>::result_type;
	};

	template <int N, int... Acc>
	struct RangeImpl<N, N, Acc...> { using result_type = Seq<Acc...>; };

	// sequence of N integers from 0 to N-1
	template <int N>
	using Range = typename RangeImpl<0, N>::result_type;

	using namespace std;

	template <bool B, class T = void>
	using enable_if_t = typename enable_if<B, T>::type;

	template <bool B, class T, class F>
	using conditional_t = typename conditional<B, T, F>::type;

	template <char... Cs>
	using TS = irqus::typestring<Cs...>;

	/*
	// Split
	// Quote, Params, AccStrs, AccStr, TS
	// Params are in reverse order
	template <char, typename, typename, typename, typename>
	struct SI;

	template <char A, typename B, typename C, typename D, typename E>
	using SIT = typename SI<A, B, C, D, E>::result_type;

	template <typename ResultType>
	struct SI<'\0', TS<>, ResultType, TS<>, TS<>> {
		using result_type = ResultType;
	};

	template <typename... Strs, typename LastString>
	struct SI<'\0', TS<>, tuple<Strs...>, LastString, TS<>> {
		using result_type = tuple<Strs..., LastString>;
	};

	template <char... Ps, typename Strs, char... Acc, char... Cs>
	struct SI<'\0', TS<'(', Ps...>, Strs, TS<Acc...>, TS<')', Cs...>> {
		using result_type = SIT<
			'\0',
			TS<Ps...>,
			Strs,
			TS<Acc..., ')'>,
			TS<Cs...>
		>;
	};

	template <char... Ps, typename Strs, char... Acc, char... Cs>
	struct SI<'\0', TS<'[', Ps...>, Strs, TS<Acc...>, TS<']', Cs...>> {
		using result_type = SIT<
			'\0',
			TS<Ps...>,
			Strs,
			TS<Acc..., ']'>,
			TS<Cs...>
		>;
	};

	template <char... Ps, typename Strs, char... Acc, char... Cs>
	struct SI<'\0', TS<'{', Ps...>, Strs, TS<Acc...>, TS<'}', Cs...>> {
		using result_type = SIT<
			'\0',
			TS<Ps...>,
			Strs,
			TS<Acc..., '}'>,
			TS<Cs...>
		>;
	};

	template <char Q, typename Ps, typename Strs, char... Acc, char QC, char... Cs>
	struct SI<Q, Ps, Strs, TS<Acc...>, TS<'\\', QC, Cs...>> {
		using result_type = enable_if_t<Q != '\0', SIT<
			Q,
			Ps,
			Strs,
			TS<Acc..., QC>,
			TS<Cs...>
		>>;
	};

	constexpr bool SIIsQuote(char c) {
		return c == '"' || c == '\'';
	}

	constexpr bool SIIsOpenParam(const char c) {
		return c == '(' || c == '[' || c == '{';
	}

	template <char... Ps, typename... Strs, char... Acc, char C, char... Cs>
	struct SI<'\0', TS<Ps...>, tuple<Strs...>, TS<Acc...>, TS<C, Cs...>> {
		using result_type =
			conditional_t<
				sizeof...(Ps) == 0 && C == ',',
				SIT<'\0', TS<>, tuple<Strs..., TS<Acc...>>, TS<>, TS<Cs...>>
			,
			conditional_t<
				SIIsQuote(C),
				SIT<  // opens a quote
					C,
					TS<Ps...>,
					Strs,
					TS<Acc..., C>,
					TS<Cs...>
				>,
				SIT<
					'\0',
					conditional_t<  // if the shit opens a param then push it to the param stack
						SIIsOpenParam(C),
						TS<C, Ps...>,
						TS<Ps...>
					>,
					tuple<Strs...>,
					TS<Acc..., C>,
					TS<Cs...>
					>
				>>;
	};

	template <char... Cs>
	auto Split(const TS<Cs...>) ->
		SIT<'\0', TS<>, tuple<>, TS<>, TS<Cs...>> {
		return {};
	}
*/

	// tuple<Strings...>, stack, acc, pending
	template <typename Strs>
	auto SI(Strs, TS<>, TS<>, TS<>)
		-> Strs;

	constexpr bool SIIsOpenParam(const char c) {
		return c == '(' || c == '[' || c == '{';
	}

	constexpr bool SIIsQuote(const char c) {
		return c == '"' || c == '\'';
	}

	constexpr bool SIIsNono(const char c) {
		return c == ',' || SIIsQuote(c) || SIIsOpenParam(c);
	}

	template <typename... Strs, typename Acc>
	auto SI(tuple<Strs...>, TS<>, Acc, TS<>)
		-> decltype(SI(tuple<Strs..., Acc>(), TS<>(), TS<>(), TS<>()));

	template <typename... Strs, typename Acc, char... Cs>
	auto SI(tuple<Strs...>, TS<>, Acc, TS<',', Cs...>)
		-> decltype(SI(tuple<Strs..., Acc>(), TS<>(), TS<>(), TS<Cs...>()));

	/*
	// Params
	template <typename Strs, char... Ss, char... Bs, char C, char... Cs,
		enable_if_t<SIIsOpenParam(C), int> = 0>
	auto SI(Strs, TS<Ss...>, TS<Bs...>, TS<C, Cs...>)
		-> decltype(SI(Strs(), TS<C, Ss...>(), TS<Bs..., C>(), TS<Cs...>()));

	template <typename Strs, char... Ss, char... Bs, char... Cs>
	auto SI(Strs, TS<'(', Ss...>, TS<Bs...>, TS<')', Cs...>)
		-> decltype(SI(Strs(), TS<Ss...>(), TS<Bs..., ')'>(), TS<Cs...>()));

	template <typename Strs, char... Ss, char... Bs, char... Cs>
	auto SI(Strs, TS<'[', Ss...>, TS<Bs...>, TS<']', Cs...>)
		-> decltype(SI(Strs(), TS<Ss...>(), TS<Bs..., ']'>(), TS<Cs...>()));

	template <typename Strs, char... Ss, char... Bs, char... Cs>
	auto SI(Strs, TS<'{', Ss...>, TS<Bs...>, TS<'}', Cs...>)
		-> decltype(SI(Strs(), TS<Ss...>(), TS<Bs..., '}'>(), TS<Cs...>()));
	// End params

	// Quote mode
	template <typename Strs, typename Stack, char... Acc, char C, char... Cs,
		enable_if_t<SIIsQuote(C), int> = 0>
	auto SI(Strs, Stack, TS<Acc...>, TS<C, Cs...>)
		-> decltype(SIQ(Strs(), TS<C>(), Stack(), TS<Acc..., C>(), TS<Cs...>()));

	template <typename Strs, char Q, typename Stack, char... Acc, char C, char... Cs>
	auto SIQ(Strs, TS<Q>, Stack, TS<Acc...>, TS<'\\', C, Cs...>)
		-> decltype(SIQ(Strs(), TS<Q>(), Stack(), TS<Acc..., C>(), TS<Cs...>()));

	template <typename Strs, typename Stack, char... Acc, char C, char... Cs,
		enable_if_t<SIIsQuote(C), int> = 0>
	auto SIQ(Strs, TS<C>, Stack, TS<Acc...>, TS<C, Cs...>)
		-> decltype(SI(Strs(), Stack(), TS<Acc..., C>(), TS<Cs...>()));

	template <typename Strs, char Q, typename Stack, char... Acc, char C, char... Cs>
	auto SIQ(Strs, TS<Q>, Stack, TS<Acc...>, TS<C, Cs...>)
		-> decltype(SIQ(Strs(), TS<Q>(), Stack(), TS<Acc..., C>(), TS<Cs...>()));

	// Quote mode end
*/

	template <typename Strs, typename Stack, char... Acc, char C, char... Cs,
		enable_if_t<!SIIsNono(C), int> = 0>
	auto SI(Strs, Stack, TS<Acc...>, TS<C, Cs...>)
		-> decltype(SI(Strs(), Stack(), TS<Acc..., C>(), TS<Cs...>()));


	template <char... Cs>
	auto Split(TS<Cs...>) ->
		decltype(SI(tuple<>(), TS<>(), TS<>(), TS<Cs...>())) {
		return {};
	}


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
	struct RPPI;

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
	struct SSI;

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
	struct SSI<Acc, Spaces, TS<>> {
		using result_type = Acc;
	};

	template <typename Str>
	auto StripSpaces(Str) -> typename SSI<TS<>, TS<>, Str>::result_type {
		return {};
	}


	struct PrintingTuple {};

	void Print(PrintingTuple, tuple<>) {
	}

	template <typename S, typename... Ss>
	void Print(PrintingTuple, tuple<S, Ss...>) {
		printf("\t%s,\n", S().data());
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
	int d___ = -1;

	LOG("WTF", a);
	LOG("WTF", a, b);
	/*
	LOG("WTF", a, b  , c[a]);
	LOG("WTF", a, b  , c[a], "ddd");
	LOG("WTF2", a, b, ((a))                , c[a], d___);
	*/
	puts(LogImpl::RemoveParamPairs(typestring_is("((aa()))")()).data());
	puts(LogImpl::StripSpaces(typestring_is("         ((aa()))  ")()).data());

	return 0;
}
