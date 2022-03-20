#include "common.h"

#include <cctype>
#include <sstream>
#include <regex>

const int LETTERS = 26; // юзаемые символы
const int MAX_POSITION_LENGTH = 17; // масимальная длина пользовательской позиции
const int MAX_POS_LETTER_COUNT = 3; // максимальное колличество букв в позиции 
const int INDEX_FIRST_CHAR_TWENTY_SIX_SISTEM = 65;

const Position Position::NONE = {-1, -1};

// Реализуйте методы:
bool Position::operator==(const Position rhs) const
{
	return this->col == rhs.col && this->row == rhs.row;
}

bool Position::operator<(const Position rhs) const
{
	return this->col < rhs.col && this->row < rhs.row;
}

bool Position::IsValid() const
{
	return (this->col <= 16383 && this->row <= 16383 && !(*this == Position::NONE));
}

std::string Position::ToString() const
{
	/*
	* последовательно делим col на 26 с остатком
	* остаток складываем в вектор интов
	* челую часть опять делим на основание пока целая часть больше 0
	* все числа кроме первого в веторе увеличиваем на 1
	* В строку запихивыем INDEX_FIRST_CHAR_TWENTY_SIX_SISTEM + число с последнего
	*/
	std:: 
	int ost = 0;
	int chel = col;
	while (chel > 0)
	{
		int ost = this->col % 26;
		int chel =/ 26;
	}
	return "";
}

Position Position::FromString(std::string_view str)
{
	//1-16383
	std::regex pos_reg("[A-Z]{1,3}[1-9][0-9]{0,4}");
	if (!std::regex_match(std::string(str), pos_reg))
	{
		return Position::NONE;
	}
	std::string col_str;
	std::string row_str;
	for (const char ch : str)
	{
		(std::isdigit(ch)) ? row_str += ch : col_str += ch;
	}
	std::reverse(col_str.begin(), col_str.end());
	int col = 0;
	for (size_t i = 0; i < col_str.size(); ++i)
	{
		col += (i > 0) ?
			((1 + col_str[i]) - INDEX_FIRST_CHAR_TWENTY_SIX_SISTEM) * std::pow(26, i) :
			(col_str[i] - INDEX_FIRST_CHAR_TWENTY_SIX_SISTEM) * std::pow(26, i);
	}
	return  Position{std::stoi(row_str) - 1, col};
}