#include "parser/ParserBase.h"

namespace parser {
    ParserBase::ParserBase(istream& _input)
        : input(_input)
    {};

    ParserBase::~ParserBase(){}
}
