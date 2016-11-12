#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "common/data_types.h"
#include "parser/ParserBase.h"

namespace parser
{

namespace boogie
{

using common::Integer;
using common::Boolean;
using common::String;
using common::ID;

class Token{
    public:
    enum class Kind{
        typeRW,
        varRW,
        constRW,
        procedureRW,
        implementationRW,
        axiomRW,
        functionRW,

        returnsRW,

        uniqueRW,
        finiteRW,

        assertRW,
        assumeRW,
        havocRW,
        ifRW,
        thenRW,
        elseRW,
        whileRW,
        returnRW,
        callRW,
        breakRW,
        gotoRW,

        ensuresRW,
        requiresRW,
        invariantRW,
        modifiesRW,

        freeRW,
        oldRW,
        whereRW,

        forallRW,
        existsRW,
        sepRW,

        boolRW,
        intRW,
        bvType,
        
        falseRW,
        trueRW,
        
        completeRW,
        orderLTRW,
        
        wildcardRW,

        leftCurly,
        rightCurly,
        leftSquare,
        rightSquare,
        leftAngle,
        rightAngle,
        leftParen,
        rightParen,

        dotRW,
        commaRW,
        colonRW,
        semicolonRW,
        assignRW,
        
        eqRW,
        
        notRW,
        negRW,
        
        mulRW,
        divRW,
        modRW,
        addRW,
        subRW,
        concatRW,
        eq2RW,
        neqRW,
        gtRW,
        geRW,
        ltRW,
        leRW,
        andRW,
        orRW,
        implRW,
        equivRW,
        
        id,
        intLit,
        stringLit,
        bvLit,
    };
    enum Kind kind;
    protected: Token(Kind _kind) : kind(_kind){}
};

class IdentifierToken : public Token{ 
public:
    IdentifierToken(const common::ID _name) 
        : Token(Token::Kind::id), name(_name){}; 
    common::ID name;  
};
class IntegerLiteralToken : public Token{ 
        public:
    IntegerLiteralToken(Integer _value) 
        : Token(Token::Kind::intLit), value(_value){} 
    Integer value;
};
class StringLiteralToken : Token{
public:
    StringLiteralToken(String _value) 
        : Token(Token::Kind::stringLit), value(_value){}
    String value;
};
class BitvectorLiteralToken : public Token{ 
public: 
    BitvectorLiteralToken(int _width,Integer _value) 
        : Token(Token::Kind::bvLit), width(_width), value(_value){}
    int width;
    Integer value;
};
class KeywordToken : Token{
public:
    KeywordToken(Token::Kind _kind) : Token(_kind){}
};
class BitVectorTypeToken : Token{
public:
    BitVectorTypeToken(int _width) 
    : Token(Token::Kind::bvType), width(_width) {}
    int width;
};



//class BooleanLiteralToken : public LiteralToken{ public: Boolean value;}
//class RationalLiteralToken : public LiteralToken{ Rational value;}

class Tokenizer : protected ParserBase
{
public:
    Tokenizer();
    ~Tokenizer();

};

}

}

#endif // TOKENIZER_H
