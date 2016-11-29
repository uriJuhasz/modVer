#include "Parser.h"

#include <set>
#include <memory>
#include <functional>
#include <locale>
#include <iostream>

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
    
    void parseKW(const String& pattern){
        auto r = tryParseKW(pattern);
        if (!r)
            throw new ExpectedException(pattern);
    }
    bool tryParseKW(const String& pattern);
    void parseTypeDeclaration();
    void parseConstantDeclaration();
    void parseVariableDeclaration();
    void parseFunctionDeclaration();
    void parseAxiom();
    void parseProcedureDeclaration();
    void parseImplementation();
    
    unique_ptr<AST::TypeArguments> parseTypeArguments(){
        skipSpaces();
        if (tryParseKW("<"))
            skipBalancedUntil('>');
        return unique_ptr<AST::TypeArguments>(new AST::TypeArguments);
    }
    unique_ptr<AST::ProcedureSignature> parseProcedureSignature(){
        auto typeArgs = parseTypeArguments();
        parseKW("(");
        skipBalancedUntil(')');
        
        return unique_ptr<AST::ProcedureSignature>(new AST::ProcedureSignature);
    }

    unique_ptr<AST::ProcedureSpec> parseProcedureSpec(){
        while (has(1)){
            auto free = tryParseKW("free");
            if (tryParseKW("requires"))
                skipUntil(';');
            else if (tryParseKW("ensures"))
                skipUntil(';');
            else if (tryParseKW("modifies"))
                skipUntil(';');
            else if (free)
                throw new ExpectedException("requires/ensures/modifies");
            else
                break;
        }
        return unique_ptr<AST::ProcedureSpec>(new AST::ProcedureSpec());
    }
    unique_ptr<AST::ProcedureBody> parseProcedureBody(){
        parseKW("{");
        skipBalancedUntil('}');
        return unique_ptr<AST::ProcedureBody>(new AST::ProcedureBody());
    }

    unique_ptr<AST::Attributes> parseAttributes(){
        skipSpaces();
        while (tryLex("{")){
            skipSpaces();
            lex(":");
            auto id = parseIdentifier();
            skipBalancedUntil('}');
        
        }
        return unique_ptr<AST::Attributes>(new AST::Attributes());
    }
    unique_ptr<AST::Identifier>  parseIdentifier(){
        auto r = tryParseIdentifier();
        if (r.get()==nullptr)
            throw new ExpectedException("identifier");
        return r;
    }
    unique_ptr<AST::Identifier>  tryParseIdentifier(){
        skipSpaces();
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

using std::cout;
using std::endl;
void Parser::parse(const common::String& in, Program&program)
{
    cout << "Parsing program" << endl;
    start(in);
    skipSpaces();
    while (has(1)){
        skipSpaces();
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
    std::wcout << "   Parsing type declaration";
    auto attributes = parseAttributes();
    auto finite = tryParseKW("finite");
    finite = finite;
    auto id = parseIdentifier();
    std::wcout << L" \"" << id->name << "\"" << endl;
    skipUntil(';');
/*    auto typeArgs = parseIdentifierSequence();
    if (!finite && tryParse("=")){
        auto synonym = parseType()
    }
    parse(";");*/
}


void Parser::parseConstantDeclaration(){
    std::wcout << "   Parsing type declaration" << endl;
    skipUntil(';');
}
void Parser::parseVariableDeclaration(){
    std::wcout << "   Parsing variable declaration" << endl;
    skipUntil(';');
}
void Parser::parseFunctionDeclaration(){
    std::wcout << "   Parsing function declaration" << endl;
    skipUntil(';');
}
void Parser::parseAxiom(){
    std::wcout << "   Parsing axiom declaration" << endl;
    skipUntil(';');
}

void Parser::parseProcedureDeclaration(){
    cout << "  p: Parsing procedure";
    auto attributes = parseAttributes();
    auto id = parseIdentifier();
    auto typeArgs = parseTypeArguments();
    auto signature = parseProcedureSignature();
    auto body = !tryLex(";");
    auto spec = parseProcedureSpec();
    if (body)
        parseProcedureBody();
    std::wcout << " " << id->name << endl;
    lex("(");
    skipBalancedUntil(')');
    if (tryParseKW("returns")){
        lex("(");
        skipBalancedUntil(')');
    }
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
    skipSpaces();
    ContextHolder ch = pushNewContext();
    unsigned int i = 0;
    if (!has(pattern.size()))
        return false;
    for (unsigned int i=0;i<pattern.size();i++)
        if (cur()==pattern[i])
            next();
        else{
            ch.popReject();
            return false;
        }
    if (has(1) && isIdentifierChar(cur())){
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
    while (has(1)){
        while (has(1) && isSpace(cur()))
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
