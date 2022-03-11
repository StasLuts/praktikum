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
        ParseTokens(input);
    }

    const Token& Lexer::CurrentToken() const
    {
        return (current_token_ == tokens_.end()) ? token_type::Eof{} : *current_token_;
    }

    Token Lexer::NextToken()
    {
        return ((current_token_ + 1) == tokens_.end()) ? token_type::Eof{} : *++current_token_;
    }

    //-------------------Lexer private----------------

    //посимвольно считывает поток и проверяет его на соответсвие одному из токенов
    //в метод считывания токена передает ОСТАТОК потока
    void Lexer::ParseTokens(std::istream& input)
    {
        char current_char;

        while (input.get(current_char)) // читаем из потока символ
        {
            input.putback(current_char); // загоняем обратно в поток
            
            //ParseString(input);
            ParseNumber(input);
            ParseIdentifer(input);
            SkippedSpace(input);
        }

        current_token_ = tokens_.begin();
    }

    void Lexer::ParseString(std::istream& input)
    {
        char current_char;
        input.get(current_char);
        if (current_char == '\"' || current_char == '\'')
        {
            std::string parse_str;
        }
        else
        {
            input.putback(current_char);
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
            tokens_.emplace_back((keywords_.find(parse_identifer) != keywords_.end()) ? keywords_[parse_identifer] : token_type::Id{ parse_identifer });
        }
    }

    //--------------------Skippeds----------------------------

    void Lexer::SkippedSpace(std::istream& input)
    {
        char current_char = input.peek();
        if (current_char == ' ')
        {
            input.get(current_char); 
        }
    }

}  // namespace parse