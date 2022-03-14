#pragma once

#include <iosfwd>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include <map>

namespace parse
{

	namespace token_type
	{
		struct Number
		{
			int value;
		};

		struct Id
		{
			std::string value;
		};

		struct Char
		{
			char value;
		};

		struct String
		{
			std::string value;
		};

		struct Class {};
		struct Return {};
		struct If {};
		struct Else {};
		struct Def {};
		struct Newline {};
		struct Print {};
		struct Indent {};
		struct Dedent {};
		struct Eof {};
		struct And {};
		struct Or {};
		struct Not {};
		struct Eq {};
		struct NotEq {};
		struct LessOrEq {};
		struct GreaterOrEq {};
		struct None {};
		struct True {};
		struct False {};

	}  // namespace token_type

	using TokenBase = std::variant<token_type::Number, token_type::Id,
		token_type::Char, token_type::String, token_type::Class, token_type::Return,
		token_type::If, token_type::Else, token_type::Def, token_type::Newline,
		token_type::Print, token_type::Indent, token_type::Dedent, token_type::And,
		token_type::Or, token_type::Not, token_type::Eq, token_type::NotEq,
		token_type::LessOrEq, token_type::GreaterOrEq, token_type::None,
		token_type::True, token_type::False, token_type::Eof>;

	struct Token : TokenBase
	{
		using TokenBase::TokenBase;

		template <typename T>
		[[nodiscard]] bool Is() const
		{
			return std::holds_alternative<T>(*this);
		}

		template <typename T>
		[[nodiscard]] const T& As() const
		{
			return std::get<T>(*this);
		}

		template <typename T>
		[[nodiscard]] const T* TryAs() const
		{
			return std::get_if<T>(this);
		}
	};

	bool operator==(const Token& lhs, const Token& rhs);
	bool operator!=(const Token& lhs, const Token& rhs);

	std::ostream& operator<<(std::ostream& os, const Token& rhs);

	class LexerError : public std::runtime_error
	{
	public:

		using std::runtime_error::runtime_error;
	};

	class Lexer
	{
	public:

		explicit Lexer(std::istream& input);

		[[nodiscard]] const Token& CurrentToken() const;

		Token NextToken();

		template <typename T>
		const T& Expect() const
		{
			using namespace std::literals;
			return (CurrentToken().Is<T>()) ? CurrentToken().As<T>() : throw LexerError("Not implemented"s);
		}

		template <typename T, typename U>
		void Expect(const U& value) const
		{
			using namespace std::literals;
			if (Expect<T>().value == value)
			{
				return;
			}
			throw LexerError("Not implemented"s);
		}

		template <typename T>
		const T& ExpectNext()
		{
			NextToken();
			return Expect<T>();
		}

		template <typename T, typename U>
		void ExpectNext(const U& value)
		{
			NextToken();
			Expect<T>(value);
		}

	private:

		std::map<std::string, Token> keywords_
		{
			{ "class",  token_type::Class{}       },
			{ "return", token_type::Return{}      },
			{ "if",     token_type::If{}          },
			{ "else",   token_type::Else{}        },
			{ "def",    token_type::Def{}         },
			{ "print",  token_type::Print{}       },
			{ "or",     token_type::Or{}          },
			{ "None",   token_type::None{}        },
			{ "and",    token_type::And{}         },
			{ "not",    token_type::Not{}         },
			{ "True",   token_type::True{}        },
			{ "False",  token_type::False{}       }
		};
		std::vector<Token> tokens_;
		int global_space_count = 0;
		std::vector<Token>::const_iterator current_token_;

		std::vector<Token>::const_iterator ParseTokens(std::istream& input);

		void ParseChars(std::istream& input);
		void ParseDoubleChars(std::istream& input);
		void ParseNewLine(std::istream& input);
		void ParseIndent(std::istream& input);
		void ParseStrings(std::istream& input);
		void ParseNumbers(std::istream& input);
		void ParseId(std::istream& input);

		void SkipSpaces(std::istream& input);
		void SkipComment(std::istream& input);
	};

}  // namespace parse