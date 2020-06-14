#include "typestring/typestring.hh"

#include <cstdio>
#include <tuple>
#include <string>
#include <type_traits>

namespace LogImpl {

	using namespace std;

	template <bool B, class T = void>
	using enable_if_t = typename enable_if<B, T>::type;

	template <bool B, class T, class F>
	using conditional_t = typename conditional<B, T, F>::type;

	template <char... Cs>
	using TS = irqus::typestring<Cs...>;


	// Poor man's interger_sequence or sth
	template <int... Nums>
	struct Seq {};


	template <int UB, int Step>
	struct RangeImpl {
		static constexpr bool IsEnd(int lb) {
			return
				Step > 0 ? lb >= UB :
				Step < 0 ? lb <= UB :
				true;
		}

		template <int, bool>
		struct QAQ {};

		template <int LB>
		struct QAQ<LB, true> {
			template <typename Acc>
			static auto QAQAQ(Acc) -> Acc;
		};

		template <int LB>
		struct QAQ<LB, false> {
			static constexpr int Next = LB + Step;

			template <int... Acc>
			static auto QAQAQ(Seq<Acc...>)
				-> decltype(QAQ<Next, IsEnd(Next)>::QAQAQ(Seq<Acc..., LB>{}));
		};

		template <int LB>
		static auto Yo(Seq<LB>) -> decltype(QAQ<LB, IsEnd(LB)>::QAQAQ(Seq<>{}));
	};

	template <int LB, int UB, int Step=1>
	using Range = decltype(RangeImpl<UB, Step>::Yo(Seq<LB>{}));


	// StripTrailingUnderscores
	template <typename, typename, typename>
	struct StripTrailingUnderscoresImpl {};

	template <char... As, char... Bs>
	struct StripTrailingUnderscoresImpl<TS<As...>, TS<Bs...>, TS<>> {
		using result_type = TS<As...>;
	};

	template <char... As, char... Bs, char... Cs>
	struct StripTrailingUnderscoresImpl<TS<As...>, TS<Bs...>, TS<'_', Cs...>> {
		using result_type = typename StripTrailingUnderscoresImpl<
			TS<As...>,
			TS<Bs..., '_'>,
			TS<Cs...>
		>::result_type;
	};

	template <char... As, char... Bs, char C, char... Cs>
	struct StripTrailingUnderscoresImpl<TS<As...>, TS<Bs...>, TS<C, Cs...>> {
		using result_type = typename StripTrailingUnderscoresImpl<
			TS<As..., Bs..., C>,
			TS<>,
			TS<Cs...>
		>::result_type;
	};

	template <char... Cs>
	auto StripTrailingUnderscores(TS<Cs...>) ->
		typename StripTrailingUnderscoresImpl<TS<>, TS<>, TS<Cs...>>::result_type {
		return {};
	}

	// RemoveParamPairs
	template <typename, typename, typename, typename>
	struct RPPI {};

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
	struct SI {};

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
	struct SIQ {};

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

	enum class WTFAction {
		kEndQuote,
		kEscape,
		kSlurp,
	};

	template <WTFAction, char, typename, typename>
	struct WTFDispatch {};

	template <char, typename, typename>
	struct WTF {};

	template <char Q, char... Acc, char C, char... Cs>
	struct WTFDispatch<WTFAction::kEndQuote, Q, TS<Acc...>, TS<C, Cs...>> {
		using result_type = WTFResult<TS<Acc..., C>, TS<Cs...>>;
	};

	template <char Q, char... Acc, char C, char... Cs>
	struct WTFDispatch<WTFAction::kEscape, Q, TS<Acc...>, TS<C, Cs...>> {
		using result_type = typename WTF<Q, TS<Acc..., '\\', C>, TS<Cs...>>::result_type;
	};

	template <char Q, char... Acc, char C, char... Cs>
	struct WTFDispatch<WTFAction::kSlurp, Q, TS<Acc...>, TS<C, Cs...>> {
		using result_type = typename WTF<Q, TS<Acc..., C>, TS<Cs...>>::result_type;
	};

	template <char Q, typename Acc, char C, char... Cs>
	struct WTF<Q, Acc, TS<C, Cs...>> {
		static constexpr WTFAction GetAction() {
			return
				C == Q ? WTFAction::kEndQuote :
				C == '\\' ? WTFAction::kEscape :
				WTFAction::kSlurp;
		}

		using result_type = typename WTFDispatch<GetAction(), Q, Acc, TS<C, Cs...>>::result_type;
	};


	template <typename, typename, typename, typename>
	struct SIP {};

	template <typename A, typename B, typename C, typename D>
	using SIPT = typename SIP<A, B ,C, D>::result_type;

	template <typename Result, typename Acc, typename Rem>
	struct SIP<Result, TS<>, Acc, Rem> {
		using result_type = SIT<Result, Acc, Rem>;
	};

	enum class SIPAction {
		kCloseStack,
		kOpenQuote,
		kOpenParam,
		kSlurp,
	};

	template <SIPAction, typename, typename, typename, typename>
	struct SIPDispatch {};

	template <typename Result, char S, char... Ss, char... Acc, char C, char... Cs>
	struct SIPDispatch<SIPAction::kCloseStack, Result, TS<S, Ss...>, TS<Acc...>, TS<C, Cs...>> {
		using result_type = SIPT<Result, TS<Ss...>, TS<Acc..., C>, TS<Cs...>>;
	};

	template <typename Result, typename Stack, char... Acc, char C, char... Cs>
	struct SIPDispatch<SIPAction::kOpenQuote, Result, Stack, TS<Acc...>, TS<C, Cs...>> {
		using WTFRes = typename WTF<C, TS<Acc..., C>, TS<Cs...>>::result_type;

		using result_type = SIPT<Result, Stack,
			typename WTFRes::acc, typename WTFRes::rem>;
	};

	template <typename Result, char... Ss, char... Acc, char C, char... Cs>
	struct SIPDispatch<SIPAction::kOpenParam, Result, TS<Ss...>, TS<Acc...>, TS<C, Cs...>> {
		using result_type = SIPT<Result, TS<SIMapParam(C), Ss...>, TS<Acc..., C>, TS<Cs...>>;
	};

	template <typename Result, typename Stack, char... Acc, char C, char... Cs>
	struct SIPDispatch<SIPAction::kSlurp, Result, Stack, TS<Acc...>, TS<C, Cs...>> {
		using result_type = SIPT<Result, Stack, TS<Acc..., C>, TS<Cs...>>;
	};

	template <typename Result, char S, char... Ss, typename Acc, char C, char... Cs>
	struct SIP<Result, TS<S, Ss...>, Acc, TS<C, Cs...>> {
		static constexpr SIPAction GetAction() {
			return
				C == S ? SIPAction::kCloseStack :
				SIIsQuote(C) ? SIPAction::kOpenQuote :
				SIIsOpenParam(C) ? SIPAction::kOpenParam :
				SIPAction::kSlurp;
		}

		using result_type =
			typename SIPDispatch<GetAction(), Result, TS<S, Ss...>, Acc, TS<C, Cs...>>::result_type;
	};


	enum class SIAction {
		kNormal,
		kComma,
		kQuote,
		kOpenParam,
	};

	template <SIAction, typename Strs, typename Acc, typename Rem>
	struct SIDispatch {};

	template <typename... Strs, char... Acc, char C, char... Cs>
	struct SIDispatch<SIAction::kNormal, tuple<Strs...>, TS<Acc...>, TS<C, Cs...>> {
		using result_type = SIT<tuple<Strs...>, TS<Acc..., C>, TS<Cs...>>;
	};

	template <typename... Strs, char... Acc, char C, char... Cs>
	struct SIDispatch<SIAction::kComma, tuple<Strs...>, TS<Acc...>, TS<C, Cs...>> {
		using result_type = SIT<
			tuple<Strs..., decltype(StripSpaces(TS<Acc...>()))>,
			TS<>,
			TS<Cs...>
		>;
	};

	template <typename... Strs, char... Acc, char C, char... Cs>
	struct SIDispatch<SIAction::kQuote, tuple<Strs...>, TS<Acc...>, TS<C, Cs...>> {
		using WTFRes = typename WTF<C, TS<Acc..., C>, TS<Cs...>>::result_type;

		using result_type = SIT<
			tuple<Strs...>,
			typename WTFRes::acc,
			typename WTFRes::rem
		>;
	};

	template <typename... Strs, char... Acc, char C, char... Cs>
	struct SIDispatch<SIAction::kOpenParam, tuple<Strs...>, TS<Acc...>, TS<C, Cs...>> {
		using result_type = SIPT<tuple<Strs...>, TS<SIMapParam(C)>, TS<Acc..., C>, TS<Cs...>>;
	};


	template <typename Strs, typename Acc, char C, char... Cs>
	struct SI<Strs, Acc, TS<C, Cs...>> {
		static constexpr SIAction GetAction() {
			return
				C == ',' ? SIAction::kComma :
				SIIsQuote(C) ? SIAction::kQuote :
				SIIsOpenParam(C) ? SIAction::kOpenParam :
				SIAction::kNormal;
		};

		using result_type = typename SIDispatch<
			GetAction(),
			Strs,
			Acc,
			TS<C, Cs...>
		>::result_type;
	};

	template <char... Cs>
	auto Split(TS<Cs...>) -> SIT<tuple<>, TS<>, TS<Cs...>> {
		return {};
	}


	template <typename Name, typename Value>
	struct Variable {
		using name = Name;
		using value_type = Value;
		Value value;
	};



	template <typename... Names, typename... Vars, int... Idxs>
	auto ToVariablesImpl(tuple<Names...>, const tuple<const Vars&...> &vars, Seq<Idxs...>)
		-> tuple<Variable<Names, const Vars&>...> {
		return { Variable<Names, const Vars&>{ get<Idxs>(vars) }... };
	}

	template <typename... Names, typename... Vars>
	auto ToVariables(tuple<Names...> names, const tuple<const Vars&...> &vars)
		-> tuple<Variable<Names, const Vars&>...> {
		return ToVariablesImpl(names, vars, Range<0, sizeof...(Names), 1>{});
	}


	auto GetFormat(char) -> typestring_is("%c");
	auto GetFormat(int) -> typestring_is("%d");
	auto GetFormat(float) -> typestring_is("%f");
	auto GetFormat(double) -> typestring_is("%lf");
	auto GetFormat(const char*) -> typestring_is("%s");
	auto GetFormat(const std::string&) -> typestring_is("%s");

	template <typename Var, int Idx>
	struct CreateFormatImpl {
		using result = irqus::tycat<
			conditional_t<
				Idx == 0,
				typestring_is(" ("),
				typestring_is(", ")
			>,
			decltype(RemoveParamPairs(typename Var::name{})),
			typestring_is(": '"),
			decltype(GetFormat(declval<typename Var::value_type>())),
			typestring_is("'")
		>;
	};

	template <typename File, typename Msg, typename... Vars, int... Idxs>
	const char* CreateFormat(const tuple<Vars...> &, Seq<Idxs...>) {
		return irqus::tycat<
			File,
			typestring_is(" (%4d): "),
			Msg,
			typename CreateFormatImpl<Vars, Idxs>::result...,
			typestring_is(")\n")
		>::data();
	}

	template <typename File, typename Msg>
	const char* CreateFormat(const tuple<> &, Seq<>) {
		return irqus::tycat<
			File,
			typestring_is(" (%4d): "),
			Msg,
			typestring_is(" (nothing lulululu)\n")
		>::data();
	}


	const char* GetFormatted(const char* str) { return str; }
	const char* GetFormatted(const std::string &str) { return str.c_str(); }

	template <typename T, enable_if_t<is_integral<T>::value, int> = 0>
	T GetFormatted(const T v) { return v; }

	template <typename T, enable_if_t<is_floating_point<T>::value, int> = 0>
	T GetFormatted(const T v) { return v; }

	template <typename File, typename Msg, typename... Vars, int... Idx>
	void PrintVars(const int line, const tuple<Vars...> &vars, Seq<Idx...>) {
		printf(
			CreateFormat<File, Msg>(vars, Range<0, sizeof...(Vars)>{}),
			line,
			GetFormatted(get<Idx>(vars).value)...
		);
	}

	// Log
	template <typename Msg, typename File, typename ArgStr, typename... Args>
	void Log(const int line, const Args&... args) {
		auto&& split_arg_names = Split(ArgStr{});
		auto&& args_tuple = tuple<const Args&...>(args...);
		auto&& var_pack = ToVariables(split_arg_names, args_tuple);
		PrintVars<File, Msg>(line, var_pack, Range<0, sizeof...(Args)>{});
	}

	// I don't know what's wrong with typestring_is("") so I'm doing this:
	template <typename T>
	auto FixEmptyTSImpl(T) -> T;

	auto FixEmptyTSImpl(TS<'\0'>) -> TS<>;

	template <typename T>
	using FixEmptyTS = decltype(FixEmptyTSImpl(T{}));
}

#define LOG(msg, ...) do											\
		{															\
			using namespace LogImpl;								\
			using Msg = typestring_is(msg);							\
			using File = typestring_is(__FILE__);					\
			using ArgStr = typestring_is(#__VA_ARGS__);				\
			using RealArgStr = FixEmptyTS<ArgStr>;					\
			Log<Msg, File, RealArgStr>(__LINE__, ## __VA_ARGS__);	\
		} while (0)

int main()
{
	int a = 1;
	int aaaaaaaaaaa = 1;
	double b = 128.322;
	int c[3] = {2, 1, 0};
	int d = -1;

	std::string my_str = "hoho haha";

	LOG("WTF");
	LOG("WTF", a);
	LOG("WTF", a, b);
	LOG("WTF", a, b, "c");
	LOG("WTF", (d));
	LOG("WTF", (d), a);
	LOG("WTF", a, (d));
	LOG("WTF", a, b, "c'", (d));
	LOG("WTF", '\\');
	LOG("WTF", a, b, c[a]);
	LOG("QQ", a);
	LOG("QQ", aaaaaaaaaaa);
	LOG("WTF2", a, b, ((a)), c[a], d);
	LOG("str", my_str, "OHYEAH");

	return 0;
}
