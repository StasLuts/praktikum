#include "lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>
#include <cctype>

#include <iostream>

using namespace std;

namespace parse
{

    //----------------Functions-------------------------

    bool operator==(const Token& lhs, const Token& rhs)
    {
        using namespace token_type;

        if (lhs.index() != rhs.index())
        {
            return false;
        }
        if (lhs.Is<Char>())
        {
            return lhs.As<Char>().value == rhs.As<Char>().value;
        }
        if (lhs.Is<Number>())
        {
            return lhs.As<Number>().value == rhs.As<Number>().value;
        }
        if (lhs.Is<String>())
        {
            return lhs.As<String>().value == rhs.As<String>().value;
        }
        if (lhs.Is<Id>())
        {
            return lhs.As<Id>().value == rhs.As<Id>().value;
        }
        return true;
    }

    bool operator!=(const Token& lhs, const Token& rhs)
    {
        return !(lhs == rhs);
    }

    std::ostream& operator<<(std::ostream& os, const Token& rhs)
    {
        using namespace token_type;

#define VALUED_OUTPUT(type) \
        if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

        VALUED_OUTPUT(Number);
        VALUED_OUTPUT(Id);
        VALUED_OUTPUT(String);
        VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
        if (rhs.Is<type>()) return os << #type;

        UNVALUED_OUTPUT(Class);
        UNVALUED_OUTPUT(Return);
        UNVALUED_OUTPUT(If);
        UNVALUED_OUTPUT(Else);
        UNVALUED_OUTPUT(Def);
        UNVALUED_OUTPUT(Newline);
        UNVALUED_OUTPUT(Print);
        UNVALUED_OUTPUT(Indent);
        UNVALUED_OUTPUT(Dedent);
        UNVALUED_OUTPUT(And);
        UNVALUED_OUTPUT(Or);
        UNVALUED_OUTPUT(Not);
        UNVALUED_OUTPUT(Eq);
        UNVALUED_OUTPUT(NotEq);
        UNVALUED_OUTPUT(LessOrEq);
        UNVALUED_OUTPUT(GreaterOrEq);
        UNVALUED_OUTPUT(None);
        UNVALUED_OUTPUT(True);
        UNVALUED_OUTPUT(False);
        UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

        return os << "Unknown token :("sv;
    }

    //-----------------Lexer---------------------------

    Lexer::Lexer(std::istream& input)
    {
        current_token_ = ParseTokens(input);
    }

    const Token& Lexer::CurrentToken() const
    {
        return *current_token_;
    }

    Token Lexer::NextToken()
    {
        if ((current_token_ + 1) == tokens_.end())
        {
            return token_type::Eof{};
        }
        return *++current_token_;
    }

    //-------------------Lexer private----------------

    std::vector<Token>::const_iterator Lexer::ParseTokens(std::istream& input)
    {
        char current_char;
        while (input.get(current_char))
        {
            input.putback(current_char);

            ParseNewLine(input);
            ParseStrings(input);
            ParseId(input);
            ParseDoubleChars(input);
            ParseChars(input);
            ParseNumbers(input);

            SkipSpaces(input);
        }
        if (!tokens_.empty() && tokens_.back() != token_type::Newline{} && tokens_.back() != token_type::Dedent{})
        {
            tokens_.emplace_back(token_type::Newline{});
        }
        tokens_.emplace_back(token_type::Eof{});
        return tokens_.begin();
    }

    void Lexer::ParseNewLine(std::istream& input)
    {
        char current_char;
        input.get(current_char);

        if (current_char == '\n')
        {
            if (!tokens_.empty() && tokens_.back() != token_type::Newline{})
            {
                tokens_.emplace_back(token_type::Newline{});
            }
            ParseIndent(input);
        }
        else
        {
            input.putback(current_char);
        }
    }

    void Lexer::ParseIndent(std::istream& input)
    {
        constexpr int INDENT_SPACES_COUNT = 2;
        int spaces_counts = 0;
        char current_char;
        while (input.get(current_char) && current_char == ' ')
        {
            ++spaces_counts;
        }
        if (input.rdstate() != std::ios_base::eofbit)
        {
            input.putback(current_char);
        }
        char next_char = input.peek();
        if (next_char == '\n' || next_char == '#')
        {
            return;
        }
        int indents_count = (spaces_counts - global_space_count) / INDENT_SPACES_COUNT;
        global_space_count = spaces_counts;
        Token indent_type;
        if (indents_count > 0)
        {
            indent_type = token_type::Indent{};
        }
        else if (indents_count < 0)
        {
            indent_type = token_type::Dedent{};
        }
        for (int i = 0; i < std::abs(indents_count); ++i)
        {
            tokens_.emplace_back(indent_type);
        }
    }

    void Lexer::ParseStrings(std::istream& input)
    {
        char quote_type;
        input.get(quote_type);
        if (quote_type == '\'' || quote_type == '\"')
        {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string str;
            while (it != end)
            {
                const char current_char = *it;
                if (current_char == quote_type)
                {
                    ++it;
                    break;
                }
                else if (current_char == '\\')
                {
                    ++it;
                    if (it == end)
                    {
                        throw std::logic_error("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        str.push_back('\n');
                        break;
                    case 't':
                        str.push_back('\t');
                        break;
                    case 'r':
                        str.push_back('\r');
                        break;
                    case '"':
                        str.push_back('"');
                        break;
                    case '\'':
                        str.push_back('\'');
                        break;
                    case '\\':
                        str.push_back('\\');
                        break;
                    default:
                        throw std::logic_error("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (current_char == '\n' || current_char == '\r')
                {
                    throw std::logic_error("Unexpected end of line"s);
                }
                else
                {
                    str.push_back(current_char);
                }
                ++it;
            }
            tokens_.emplace_back(token_type::String{ str });
        }
        else
        {
            input.putback(quote_type);
        }
    }

    void Lexer::ParseId(std::istream& input)
    {
        char current_char;
        current_char = input.peek();
        if (std::isalpha(current_char) || current_char == '_')
        {
            std::string parsed_id;
            while (input.get(current_char))
            {
                if (std::isalnum(current_char) || current_char == '_')
                {
                    parsed_id.push_back(current_char);
                }
                else
                {
                    input.putback(current_char);
                    break;
                }
            }
            if (keywords_.find(parsed_id) != keywords_.end())
            {
                tokens_.push_back(keywords_.at(parsed_id));
            }
            else
            {
                tokens_.emplace_back(token_type::Id{ parsed_id });
            }
        }
    }

    void Lexer::ParseDoubleChars(std::istream& input)
    {
        char current_char;
        input.get(current_char);
        char next_char = input.peek();
        if (current_char == '=' && next_char == '=')
        {
            input.get();
            tokens_.emplace_back(token_type::Eq{});
        }
        else if (current_char == '!' && next_char == '=')
        {
            input.get();
            tokens_.emplace_back(token_type::NotEq{});
        }
        else if (current_char == '>' && next_char == '=')
        {
            input.get();
            tokens_.emplace_back(token_type::GreaterOrEq{});
        }
        else if (current_char == '<' && next_char == '=')
        {
            input.get();
            tokens_.emplace_back(token_type::LessOrEq{});
        }
        else
        {
            input.putback(current_char);
        }
    }

    void Lexer::ParseChars(std::istream& input)
    {
        char current_char;
        current_char = input.peek();
        if (std::ispunct(current_char))
        {
            if (current_char == '#')
            {
                SkipComment(input);
                return;
            }
            input.get(current_char);
            tokens_.emplace_back(token_type::Char{ current_char });
        }
    }

    void Lexer::ParseNumbers(std::istream& input)
    {
        char current_char;
        current_char = input.peek();
        if (std::isdigit(current_char))
        {
            string parsed_num;
            while (input.get(current_char))
            {
                if (std::isdigit(current_char))
                {
                    parsed_num.push_back(current_char);
                }
                else
                {
                    input.putback(current_char);
                    break;
                }
            }
            int num = std::stoi(parsed_num);
            tokens_.emplace_back(token_type::Number{ num });
        }
    }

    //--------------------Skippeds----------------------------

    void Lexer::SkipSpaces(std::istream& input)
    {
        while (input.peek() == ' ')
        {
            input.get();
        }
    }

    void Lexer::SkipComment(std::istream& input)
    {
        char current_char;
        input.get(current_char);
        if (current_char == '#')
        {
            string comment;
            getline(input, comment, '\n');
            if (input.rdstate() != std::ios_base::eofbit)
            {
                input.putback('\n');
            }
        }
        else
        {
            input.putback(current_char);
        }
    }

}  // namespace parse