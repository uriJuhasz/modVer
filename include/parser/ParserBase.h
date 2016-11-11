#ifndef PARSERBASE_H_INCLUDED
#define PARSERBASE_H_INCLUDED

#include "TextPos.h"
#include <istream>
#include <vector>


namespace parser{
using std::istream;
using std::vector;

class ParserBase{
public:
    ParserBase(istream& input);
    typedef char Char;

    virtual ~ParserBase();

    virtual Char cur();
    virtual Char get();
    virtual TextPos curPos();
    virtual void pushPos();
    virtual void dropPos();
    virtual void popPos();
    
    class ContextHolder{ private:ParserBase& b;};
    class PosPair{private:BufPos bufPos; TextPos textPos;};
private:
    typedef int BufPos;
    string buf;
    int bufPos = 0;
};

}

#endif // PARSERBASE_H_INCLUDED
