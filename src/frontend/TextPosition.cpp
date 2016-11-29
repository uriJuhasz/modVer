/*
 * TextPosition.cpp
 *
 *  Created on: 29 Nov 2016
 *      Author: uri
 */

#include <frontend/TextPosition.h>
#include <string>

namespace frontend {
	TextPosition TextPosition::NoPos = TextPosition(-1,-1);
	TextPosition::operator std::string() const {
		return "(" + std::to_string(row) + "," + std::to_string(col) + ")";
	}

} /* namespace frontend */
