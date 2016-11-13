#ifndef FRONTEND_BOOGIE_PARSER_H
#define FRONTEND_BOOGIE_PARSER_H

#include "../AST.h"
#include "../../parser/ParserBase.h"
#include "common/data_types.h"
#include <functional>

namespace frontend{
namespace boogie{
namespace parser{
using common::String;
using AST::Program;
using std::string;

class Parser : protected ::parser::ParserBase
{
public:
    class MLCommentException : public Exception{};
    class SkipException : public Exception{};
    class StringLiterlExceedsLineException : public Exception{};
    class StringLiterlExceedsFileException : public Exception{};
    class LexException : public Exception{};
    
    Parser();
    ~Parser();
    
    void parse(const String& in, Program&program);
private:
    
    bool tryParseKW(const String& pattern);
    void parseTypeDeclaration();
    void parseConstantDeclaration();
    void parseVariableDeclaration();
    void parseFunctionDeclaration();
    void parseAxiom();
    void parseProcedureDeclaration();
    void parseImplementation();
    
    
    Char skipBalancedUntil(Char end);
    Char skipBalancedUntil(std::function<bool(Char)> isEnd);
    bool tryEatBalanced();
    
    bool tryLex(const string& s);
    bool lex(const string& s);
    void skipSpaces();
    bool lexStringLiteral();
    bool tryEatStringLiteral();
    bool eatComments();
    bool eatCommentsAndStrings();
    bool tryEatComment();
    bool lexSingleLineComment();
    bool lexMultiLineComment();
    void skipUntil(Char c);
    void skipUntil(const class std::set<Char>&);
    
    const String* input;
};//class Parser

}//namespace parser
}//namespace boogie
}//namespace frontend

#endif // PARSER_H
