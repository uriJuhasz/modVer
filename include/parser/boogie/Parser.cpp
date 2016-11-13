#include "Parser.h"

#include <set>

namespace frontend{
namespace boogie{
namespace parser{
    
using common::String;
using common::Char;

Parser::Parser()
{
}

Parser::~Parser()
{
}


AST::Program Parser::parse(const String& in)
{
    start(in);
    skipSpaces();
    while (!isEOL()){
        if (tryParseKW("type"))
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


const std::set<Char> identifierChars = {'_','.','$','#','\'','~','^','?'};

bool isIdentifierChar(Char c){return identifierChars.find(c)!=identifierChars.end();}

bool Parser::tryParseKW(const String& pattern){
    ContextHolder ch = pushNewContext();
    auto i = 0;
    while (!isEOL() && i<pattern.size() && cur()==pattern[i])
        next();
    if (!isEOL() && isIdentifierChar(cur())){
        ch.popReject();
        return false;
    }else{
        ch.popAccept();
        return true;
    }
}

void Parser::parseTypeDeclaration(){
    skipUntil(';');
}
void Parser::parseConstDeclaration(){
    skipUntil(';');
}
void Parser::parseVarDeclaration(){
    skipUntil(';');
}
void Parser::parseVarDeclaration(){
    skipUntil(';');
}


}//namespace parser
}//namespace boogie
}//namespace frontend
