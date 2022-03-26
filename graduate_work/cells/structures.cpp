#include "common.h"

#include <cctype>
#include <sstream>
#include <regex>
#include <cmath>

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
	return (this->col >= 0 && this->col < MAX_COLS &&
		this->row >= 0 && this->row < MAX_ROWS);
}

std::string Position::ToString() const
{
	if (IsValid())
	{
		std::string userest_num;
		int integer_part = col;
		while (integer_part >= 0)
		{
			userest_num.insert(userest_num.begin(), 'A' + integer_part % LETTERS);
			integer_part = integer_part / LETTERS - 1;
		}
		return userest_num += std::to_string(row + 1);
	}
	return "";
}

Position Position::FromString(std::string_view str)
{
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
			((1 + col_str[i]) - INDEX_FIRST_CHAR_TWENTY_SIX_SISTEM) * std::pow(LETTERS, i) :
			(col_str[i] - INDEX_FIRST_CHAR_TWENTY_SIX_SISTEM) * std::pow(LETTERS, i);
	}
	return  Position{std::stoi(row_str) - 1, col};
}