#ifndef FRONTEND_BOOGIE_PARSER_H
#define FRONTEND_BOOGIE_PARSER_H

#include "../AST.h"
#include "../../parser/ParserBase.h"
#include "common/data_types.h"

namespace frontend{
namespace boogie{
namespace parser{
using common::String;

class Parser : protected ::parser::ParserBase
{
public:
    Parser();
    ~Parser();
    
    AST::Program parse(const String& s);
private:
    const String* input;
    void skipSpaces();
    void skipUntil(Char c);
    void skipUntil(const class std::set<Char>&);
    
    bool tryParseKW(const String& pattern);
    void parseTypeDeclaration();
    void parseConstantDeclaration();
    void parseVariableDeclaration();
    void parseFunctionDeclaration();
    void parseAxiom();
    void parseProcedureDeclaration();
    void parseImplementation();
};

}//namespace parser
}//namespace boogie
}//namespace frontend

#endif // PARSER_H
