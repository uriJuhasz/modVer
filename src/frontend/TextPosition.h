/*
 * TextPosition.h
 *
 *  Created on: 29 Nov 2016
 *      Author: uri
 */

#ifndef SRC_FRONTEND_TEXTPOSITION_H_
#define SRC_FRONTEND_TEXTPOSITION_H_
#include <cassert>
#include <string>

namespace frontend {
class TextPosition {
public:
	typedef int Int;
	TextPosition() : row(-1),col(-1){}
	TextPosition(Int row,Int col) : row(row),col(col){}
	~TextPosition(){};
	Int row,col;

	operator std::string() const;

};
static const TextPosition NoPos(-1,-1);

} /* namespace frontend */

#endif /* SRC_FRONTEND_TEXTPOSITION_H_ */
