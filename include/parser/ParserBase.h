#ifndef PARSERBASE_H_INCLUDED
#define PARSERBASE_H_INCLUDED

#include "TextPos.h"
#include <string>
#include <cassert>
#include <exception>
#include <stack>


namespace parser{
using std::exception;


class ParserBase{
public:
    class Exception : public exception{};
    class ReadPastEndException : public Exception{};
    typedef char Char;
    typedef std::string String;
    
    ParserBase(const String& _in) : buf(_in){
        posStack.push(PosPair(0,TextPos(0,0)));
    }
    virtual ~ParserBase(){}

    Char cur() {
        checkInBounds(); 
        assert(curBufPos()>=0 && curBufPos()<buf.size()); 
        return buf[curBufPos()];
    }
    void next(){
        checkInBounds();
        assert(curBufPos()>=0 && curBufPos()<buf.size()); 
        if (isEOL(cur())){
            curTextPos().row++;
            curTextPos().col=0;
        }else{
            curTextPos().col++;
        }
        curBufPos()++;
    }
    TextPos curPos(){return curTextPos;}
    
    class ContextHolder{ 
        friend class ParserBase;
        private: 
            ContextHolder(ParserBase& _b) : b(_b){}
            ParserBase& b;
    };
    ContextHolder pushNewContext(){
        
    }
    
    bool isEOL(Char c){return c=='\n';}
private:
    void pushPos();
    void dropPos();
    void popPos();
    
    typedef unsigned int BufPos;
    class PosPair{ 
    public: 
        PosPair(BufPos _bufPos,TextPos _textPos) : bufPos(_bufPos), textPos(_textPos){}
        BufPos bufPos; 
        TextPos textPos;
    };
    std::stack<PosPair> posStack;

    const String buf;
    
    inline BufPos&  curBufPos(){return posStack.top().bufPos;}
    inline TextPos& curTextPos(){return posStack.top().textPos;}
    
    void checkInBounds(){
        if (curBufPos()>=buf.size())
            throw new ReadPastEndException();
    }
};

}

#endif // PARSERBASE_H_INCLUDED
