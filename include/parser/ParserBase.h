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
        assert(curBufPos()<buf.size()); 
        return buf[curBufPos()];
    }
    void next(){
        checkInBounds();
        assert(curBufPos()<buf.size()); 
        if (isEOL(cur())){
            curTextPos().row++;
            curTextPos().col=0;
        }else{
            curTextPos().col++;
        }
        curBufPos()++;
    }
    TextPos curPos()const{return curTextPos();}
    
    class ContextHolder{ 
        friend class ParserBase;
        ~ContextHolder(){
            if (canPop())
                popReject();
            assert(!canPop());
        }
        void popAccept(){ 
            assert(canPop());
            auto p = b.posStack.top();
            b.posStack.pop();
            d=-1;
            b.posStack.top()=p;
            assert(!canPop());
        }
        void popReject(){ 
            assert(canPop());
            b.posStack.pop();
            d=-1;
            assert(!canPop());
        }
        private: 
            ContextHolder(ParserBase& _b, int _d) : b(_b), d(_d){assert(d>0);}
            ParserBase& b;
            int d;
            bool canPop(){return d>0 && (int)b.posStack.size()==d;}
    };
    ContextHolder pushNewContext(){
        posStack.push(posStack.top());
        return ContextHolder(*this,posStack.size());
    }
    
    bool isEOL(Char c){return c=='\n';}
private:
    void pushPos();
    void dropPos();
    void popPos();
    
    typedef unsigned int BufPos;
    class PosPair{ 
    public: 
        PosPair(BufPos _bufPos,TextPos _textPos) 
            : bufPos(_bufPos), textPos(_textPos){}
        BufPos bufPos; 
        TextPos textPos;
    };
    std::stack<PosPair> posStack;

    const String buf;
    
    inline BufPos&  curBufPos(){return posStack.top().bufPos;}
    inline TextPos& curTextPos(){return posStack.top().textPos;}
    inline const BufPos&  curBufPos()const {return posStack.top().bufPos;}
    inline const TextPos& curTextPos()const {return posStack.top().textPos;}
    
    void checkInBounds()const{
        if (curBufPos()>=buf.size())
            throw new ReadPastEndException();
    }
};

}

#endif // PARSERBASE_H_INCLUDED
