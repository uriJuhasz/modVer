#include "Tokenizer.h"
/*
namespace parser
{

namespace boogie
{
Tokenizer::Tokenizer()
{
}

Tokenizer::~Tokenizer()
{
}

Token Tokenizer::curToken()
{
    if (pCurToken.kind==Token::Kind::noToken)
        nextToken();
    return pCurToken;
}

void Tokenizer::nextToken()
{
    skipSpaces();
    Char c = cur();
    if (isKeywordOrIdentifierStart(c))
        nextKeywordOrIdentifier();
    else if (isQuotedIdentifierStart(c))
        nextQuotedIdentifier();
    else if (isIntegerStart(c))
        nextIntegerOrBVLit();
    else if (isStringLiteralStart(c))
        nextStringLiteral();
    else 
        nextOperator();
}

const set<Char> identifierChars = {'_','.','$','#','\'','`','~','^','\\','?'};

bool isKeywordOrIdentifierStart(Char c){
    return ctype<Char>::alpha(c) || identifierChars.has(c);
}

}}*/