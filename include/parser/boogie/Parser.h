#ifndef FRONTEND_BOOGIE_PARSER_H
#define FRONTEND_BOOGIE_PARSER_H

#include "AST.h"
#include "parser/ParserBase.h"
#include "common/data_types.h"

namespace frontend{
namespace boogie{
namespace parser{

class Parser : protected ParserBase
{
public:
    Parser();
    ~Parser();
    
    BoogieProgram parse(const String& s);
private:
    const String* input;
};

}

}

}

#endif // PARSER_H
