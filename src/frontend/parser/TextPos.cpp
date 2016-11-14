#include "TextPos.h"
#include <string>

namespace frontend{
namespace parser{
	TextPos::operator std::string() const {
		return "(" + std::to_string(row) + "," + std::to_string(col) + ")";
	}
} // namespace parser
} // namespace frontend
