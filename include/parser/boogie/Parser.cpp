#include "Parser.h"

namespace frontend{
namespace boogie{
namespace parser{
Parser::Parser()
{
}

Parser::~Parser()
{
}


AST::Program Parser::parse(const String& in)
{
    start(s);
    skipSpaces();
    while (!isEnd()){
        if (tryParse("type"))
            parseTypeDeclaration();
        else if (tryParseKW("const"))
            parseConstantDeclaration();
        else if (tryParseKW("var"))
            parseVariableDeclaration();
        else if (tryParseKW("function"))
            parseFunctionDeclaration();
        else if (tryParseKW("axiom"))
            parseAxiom();
        else if (tryParseKW("procedure"))
            parseProcedureDeclaration();
        else if (tryParseKW("implementation"))
            parseImplementation();
        else
            throw new Exception();
    }
}


AST::Program Parser::tryParseKW(const String& pattern){
    try{
        ContextHolder ch = pushNewContext();
        auto i = 0;
        while (!isEOL() && i<pattern.size() && cur()==pattern[i])
            next();
        if (!isEOL() && isIdentifierChar(cur()))
    }
        
}
