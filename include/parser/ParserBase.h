#ifndef PARSERBASE_H_INCLUDED
#define PARSERBASE_H_INCLUDED

#include "Pos.h"
#include <istream>

using std::istream;

namespace parser{

class ParserBase{
public:
    ParserBase(istream& input);
    typedef char Char;

    virtual ~ParserBase();

    virtual Char cur();
    virtual Char get();
    virtual Pos curPos();
    virtual void pushPos();
    virtual void dropPos();
    virtual void popPos();
private:
    istream& input;
    vector<Char> buf;
};

}

#endif // PARSERBASE_H_INCLUDED
