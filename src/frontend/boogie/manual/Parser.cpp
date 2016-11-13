#include "Parser.h"

#include <set>
#include <memory>
#include <functional>
#include <locale>

namespace frontend{
namespace boogie{
namespace parser{
    
using common::String;
using common::Char;
using std::set;
using std::unique_ptr;
using std::function;
using frontend::parser::ParserBase;
using String = common::String;

/*****************************************************************************
 * Classifiers
****************************************************************************/
const std::set<Char> identifierExtraChars = {'_','.','$','#','\'','~','^','?'};
const std::locale loc;
bool isIdentifierStart(Char c){return std::use_facet< std::ctype<Char> >(loc).is(std::ctype<Char>::alpha,c) || identifierExtraChars.find(c)!=identifierExtraChars.end();}
bool isIdentifierChar(Char c){return std::use_facet< std::ctype<Char> >(loc).is(std::ctype<Char>::digit,c) || isIdentifierStart(c);}


class Parser : protected ParserBase
{
public:
    
    Parser();
    ~Parser();
    
    void parse(const common::String& in, Program&program);
private:
    
    bool tryParseKW(const String& pattern);
    void parseTypeDeclaration();
    void parseConstantDeclaration();
    void parseVariableDeclaration();
    void parseFunctionDeclaration();
    void parseAxiom();
    void parseProcedureDeclaration();
    void parseImplementation();
    
    unique_ptr<AST::Attributes> parseAttributes(){
        skipSpaces();
        while (tryLex("{")){
            skipSpaces();
            lex(":");
            auto id = parseIdentifier();
            skipBalancedUntil('}');
        
        }
    }
    unique_ptr<AST::Identifier>  parseIdentifier(){
        auto r = tryParseIdentifier();
        if (r.get()==nullptr)
            throw new ExpectedException("identifier");
        return r;
    }
    unique_ptr<AST::Identifier>  tryParseIdentifier(){
        common::String id;
        if (!has(1))
            return nullptr; 
        if (!isIdentifierStart(cur()))
            return nullptr;
        id += cur();
        next();
        while (has(1) && isIdentifierChar(cur())){
            id += cur();
            next();
        }
        return unique_ptr<AST::Identifier>(new AST::Identifier(id));
    }
    unique_ptr<std::vector<AST::Identifier>> parseIdentifierSequence();
//    unique_ptr<AST::Type> parseType();
    
    
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

Parser::Parser()
{
}

Parser::~Parser()
{
}


void Parser::parse(const common::String& in, Program&program)
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


void Parser::parseTypeDeclaration(){
    auto attributes = parseAttributes();
    auto finite = tryParseKW("finite");
    auto name = parseIdentifier();
    skipUntil(';');
/*    auto typeArgs = parseIdentifierSequence();
    if (!finite && tryParse("=")){
        auto synonym = parseType()
    }
    parse(";");*/
}


void Parser::parseConstantDeclaration(){
    skipUntil(';');
}
void Parser::parseVariableDeclaration(){
    skipUntil(';');
}
void Parser::parseFunctionDeclaration(){
    skipUntil(';');
}
void Parser::parseAxiom(){
    skipUntil(';');
}

void Parser::parseProcedureDeclaration(){
    auto c = skipBalancedUntil([](Char c){return c=='{' || c==';';});
    if (c=='{')
        skipBalancedUntil('}');
}
void Parser::parseImplementation(){
    skipBalancedUntil([](auto c){return c=='{';});
    skipBalancedUntil([](auto c){return c=='}';});
}


void Parser::skipUntil(Char end){
    while (has(1)){
        eatCommentsAndStrings();
        if (!has(1))
            throw new SkipException();
        Char c = cur();
        next();
        if (c==end)
            return;
    }
}
Char Parser::skipBalancedUntil(Char end){
    return skipBalancedUntil([end](auto c){return c==end;});
}
Char Parser::skipBalancedUntil(function<bool(Char)> isEnd){
    while (has(1)){
        if (tryEatComment() || tryEatStringLiteral() || tryEatBalanced())
            continue;
        else{
            auto c = cur();
            next();
            if (isEnd(c))
                return c;
        }
    }
    throw new SkipException();
}
bool Parser::tryEatBalanced(){
    bool r = false;
    while (has(1)){
        Char end;
        if (tryLex("("))
            end = ')';
        else if (tryLex("["))
            end = ']';
        else if (tryLex("{"))
            end = '}';
        else
            return r;
        r = true;
        skipBalancedUntil([end](auto c){return c==end;});
    }
    throw new SkipException();
}



bool Parser::tryParseKW(const String& pattern){
    ContextHolder ch = pushNewContext();
    unsigned int i = 0;
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


const set<Char> spaceChars = {0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x20};
bool isSpace(Char c){
    return spaceChars.find(c)!=spaceChars.end();
}
bool Parser::tryLex(const string& s){
    if (!has(s.size()))
        return false;
    for (unsigned int i=0;i<s.size();i++)
        if (s[i]!=peek(i))
            return false;
    for (unsigned int i=0;i<s.size();i++)
        next();
    return true;
}
bool Parser::lex(const string& s){
    bool r = tryLex(s);
    if (!r)
        throw new LexException();
    return true;
}
void Parser::skipSpaces(){
    while (!isEOL()){
        while (!isEOL() && isSpace(cur()))
            next();
        if (tryLex("//"))
            lexSingleLineComment();
        else if (tryLex("/*"))
            lexMultiLineComment();
        else
            break;
    }
}
bool Parser::lexSingleLineComment(){
    while (has(1)){
        auto c = cur();
        next();
        if (isEOL(c))
            break;
    }
    return true;
}
bool Parser::lexMultiLineComment(){
    while (has(2)){
        if (tryLex("/*"))
            lexMultiLineComment();
        else if (tryLex("*/"))
            return true;
        else
            next();
    }
    throw new MLCommentException();
}

bool Parser::tryEatComment(){
    if (!has(2))
        return false;
    if (tryLex("//"))
        return lexSingleLineComment();
    else if (tryLex("/*"))
        return lexMultiLineComment();
    else 
        return false;
}
bool Parser::tryEatStringLiteral(){
    if (!has(2))
        return false;
    if (tryLex("\""))
        return lexStringLiteral();
    else 
        return false;
}
bool Parser::lexStringLiteral(){
    while (has(1)){
        auto c = cur();
        next();
        if (c=='"')
            return true;
        if (c=='\n' || c=='\r')
            throw new StringLiterlExceedsLineException();
        if (c=='\\')
            lex("\"");
    }
    throw new StringLiterlExceedsFileException();
}
bool Parser::eatComments(){
    bool r = false;
    while (has(2)){
        if (tryEatComment())
            r=true;
        else
            break;
    }
    return r;
}
bool Parser::eatCommentsAndStrings(){
    bool r = false;
    while (has(1))
        if (tryEatComment() || tryEatStringLiteral())
            r = true;
        else
            return r;
    return r;
}


void parse(const common::String& input, Program& output){
    Parser parser;
    parser.parse(input,output);
}

}//namespace parser
}//namespace boogie
}//namespace frontend
