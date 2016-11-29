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
	TextPosition(Int _row,Int _col) : row(_row),col(_col){assert(_row>0 && _col>0);};
	~TextPosition(){};
	const Int row,col;

	operator std::string() const;

	static class TextPosition NoPos;
};

} /* namespace frontend */

#endif /* SRC_FRONTEND_TEXTPOSITION_H_ */
