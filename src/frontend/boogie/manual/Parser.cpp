#include "Parser.h"

#include <set>
#include <memory>
#include <functional>

namespace frontend{
namespace boogie{
namespace parser{
    
using common::String;
using common::Char;
using std::set;
using std::unique_ptr;
using std::function;

Parser::Parser()
{
}

Parser::~Parser()
{
}


void Parser::parse(const String& in, Program&program)
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
    skipUntil(';');
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


const std::set<Char> identifierChars = {'_','.','$','#','\'','~','^','?'};

bool isIdentifierChar(Char c){return identifierChars.find(c)!=identifierChars.end();}

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

}//namespace parser
}//namespace boogie
}//namespace frontend
