#include "Tokenizer.h"

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
    else if (isIdentifierStart(c))
        nextIdentifier();
    else if (isQuotedIdentifierStart(c))
        nextQuotedIdentifier();
    else if (isIntegerStart(c))
        nextIntegerOrBVLit();
    throw new UnknownCharacterException(c);
}

}}