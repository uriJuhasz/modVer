#ifndef PARSER_TEXTPOS_H
#define PARSER_TEXTPOS_H

namespace parser{

class TextPos{
    public:
        TextPos(Int _row,Int _col) : row(_row), col(_col){};
        ~TextPos(){};
        typedef int Int;
        const Int row;
        const Int col;
    
    protected:

    private:
};

} // namespace parser

#endif // PARSER_TEXTPOS_H
