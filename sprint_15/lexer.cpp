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
        if (current_token_ == tokens_.end())
        {
            return token_type::Eof{};
        }
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
        char current_char = input.peek();
        if (current_char == '\n')
        {
            input.get(current_char);
        }

        while (input.get(current_char))
        {
            input.putback(current_char);

            ParseString(input);
            ParseNumber(input);
            ParseIdentifer(input);
            ParseChar(input);
            ParseDent(input);

            SkippedSpace(input);
        }

        return tokens_.begin();
    }

    void Lexer::ParseString(std::istream& input)
    {
        char first_char; // ??????????? ??????
        input.get(first_char); // ?????????
        if (first_char == '\"' || first_char == '\'') // ???? ????????? ? ??????? ??
        {
            char current_char;
            std::string parse_str;
            while (input.get(current_char))
            {
                if (first_char == '\"' && current_char == '\"' || first_char == '\'' && current_char == '\'')
                {
                    break;
                }
                else
                {
                    parse_str.push_back(current_char);
                }
            }
            tokens_.emplace_back(token_type::String{ parse_str });
        }
        else // ???? ??? ??
        {
            input.putback(first_char); // ??????? ?????? ??????? ? ?????
        }
    }

    void Lexer::ParseNumber(std::istream& input)
    {
        char current_char = input.peek();
        if (std::isdigit(current_char))
        {
            std::string parse_num;
            while (input.get(current_char))
            {
                if (std::isdigit(current_char))
                {
                    parse_num.push_back(current_char);
                }
                else
                {
                    input.putback(current_char);
                    break;
                }
            }
            int num = std::stoi(parse_num);
            tokens_.emplace_back(token_type::Number{ num });
        }
    }

    void Lexer::ParseIdentifer(std::istream& input)
    {
        char current_char = input.peek();
        if (std::isalpha(current_char) || current_char == '_')
        {
            std::string parse_identifer;
            while (input.get(current_char))
            {
                if (std::isalnum(current_char) || current_char == '_')
                {
                    parse_identifer.push_back(current_char);
                }
                else
                {
                    input.putback(current_char);
                    break;
                }
            }
            tokens_.emplace_back((keywords_.find(parse_identifer) != keywords_.end()) ? keywords_.at(parse_identifer) : token_type::Id{ parse_identifer });
        }
    }

    void Lexer::ParseChar(std::istream& input)
    {
        char current_char = input.peek();
        if (current_char == '\n')
        {
            input.get(current_char);
            if (tokens_.back() != token_type::Newline{})
            {
                tokens_.emplace_back(token_type::Newline{});
            }
        }
        else if (std::ispunct(current_char))
        {
            string parse_comparison_operand;
            input.get(current_char);
            parse_comparison_operand.push_back(current_char);
            input.get(current_char);
            if (std::ispunct(current_char))
            {
                parse_comparison_operand.push_back(current_char);
                if (keywords_.find(parse_comparison_operand) != keywords_.end())
                {
                    tokens_.emplace_back(keywords_.at(parse_comparison_operand));
                    return;
                }
            }
            input.putback(current_char);
            tokens_.emplace_back(token_type::Char{ parse_comparison_operand.front() });
        }
    }

    void Lexer::ParseDent(std::istream& input)
    {
        char current_char = input.peek();
        if (tokens_.back() == token_type::Newline{}) // ???? ?????????? ?????? ? ?????????? ????? \n
        {
            int current_space_count = 0; // ??????? ?????????? ????????
            while (input.get(current_char)) // ???? ?????
            {
                if (current_char == ' ') // ???? ??????
                {
                    ++current_space_count; // ?? ???????? ??????????? ??????? ???????
                }
                else
                {
                    input.putback(current_char);
                    break;
                }
            }
            input.get(current_char);
            if (current_char != '\n')
            {
                if (current_space_count > global_space_count && current_space_count % 2 == 0) // ???? ??????? ??????? ??? ?????????? ?? 2 ?? 
                {
                    for (int i = current_space_count; i > global_space_count; i -= 2)
                    {
                        tokens_.emplace_back(token_type::Indent{});
                    }
                    global_space_count = current_space_count; // ???????????
                }
                else if (current_space_count < global_space_count && current_space_count % 2 == 0) // ???? ??????? ??????? ??? ?????????? ?? 2 ?? 
                {
                    for (int i = current_space_count; i < global_space_count; i += 2)
                    {
                        tokens_.emplace_back(token_type::Dedent{});
                    }
                    global_space_count = current_space_count; // ???????????
                }
                input.putback(current_char);
            }
        }
    }

    //--------------------Skippeds----------------------------

    void Lexer::SkippedSpace(std::istream& input)
    {
        char current_char = input.peek();
        if (current_char == ' ' && tokens_.back() != token_type::Newline{})
        {
            input.get(current_char);
        }
    }

}  // namespace parse