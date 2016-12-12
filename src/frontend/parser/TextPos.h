#ifndef PARSER_TEXTPOS_H
#define PARSER_TEXTPOS_H

#include <string>

namespace frontend{
namespace parser{

class TextPos{
    public:
        typedef int Int;
        TextPos(Int _row,Int _col) : row(_row), col(_col){}
        TextPos(const TextPos& other) : row(other.row), col(other.col){}
        ~TextPos(){};
        Int row;
        Int col;

        operator std::string() const;
    
    protected:

    private:
};

}} // namespace parser

#endif // PARSER_TEXTPOS_H
