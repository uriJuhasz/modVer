#include "ParseTree.h"

namespace frontend{
namespace boogie{
namespace ParseTree{

using common::StringConversionException;

ConstantOrderSpec::ConstantOrderSpec(){}
ConstantOrderSpec::ConstantOrderSpec(const ConstantOrderSpec& o)
    : specified(o.specified), complete(o.complete), parents(o.parents){}
pConstantOrderSpec ConstantOrderSpec::clone() const{return make_unique<ConstantOrderSpec>(*this); }

bool isDigit(wchar_t c)
{
    return c>='0' && c<='9';
}

int char2Int(wchar_t c)
{
    if (!isDigit(c))
        throw new StringConversionException();
    return c-'0';
}

wchar_t getChar(const wstring& s, unsigned int& i)
{
    if (i>=s.length())
        throw new StringConversionException();
    auto c = s[i]; i++;
    return c;
}
void getChar(const wstring& s, unsigned int& i, wchar_t e)
{
    if (getChar(s,i)!=e)
        throw new StringConversionException();
}

bool tryGetDigit(const wstring& s, unsigned int& i, int& o){
    if (i>=s.length() || !isDigit(s[i]))
        return false;
    o=char2Int(getChar(s,i));
    return true;
}
int getDigit(const wstring& s, unsigned int& i){
    int o;
    if  (!tryGetDigit(s,i,o))
        throw new StringConversionException();
    return o;
}

Integer getDec(const wstring& s, unsigned int& i){
    Integer r=getDigit(s,i);
    int d;
    while (tryGetDigit(s,i,d))
        r=r*10+d;
    return r;
}
int32_t getInt32(const wstring& s, unsigned int& i){
    int64_t r=getDigit(s,i);
    int d;
    while (tryGetDigit(s,i,d))
    {
        r=r*10+d;
        if (r >= std::numeric_limits<int32_t>::max())
            throw new StringConversionException();
    }
    return (int32_t)r;
}
Sign getSign(const wstring& s, unsigned int& i){
    auto r = Sign::Pos;
    if (i<s.length()){
        auto c = s[i];
        if (c=='-') r = Sign::Neg;
            else if (i!='+') getChar(s,i);
    }
    return r;
}

Sign sign(char c){return c=='+' ? Sign::Pos : Sign::Neg; }

bool inbits(Integer i, int32_t bits){return abs(i)<pow(Integer(2),bits);}

Float string2Float   (const wstring& s)
{
    /* From Boogie:
     * String must be either of the format *e*f*e*
     * or of the special value formats: 0NaN*e* 0nan*e* 0+oo*e* 0-oo*e*
     * Where * indicates an integer value (digit)
     */

    if (s.length()<7)
        throw new StringConversionException();
    auto s4 = s.substr(0,4);
    auto isNan = s4.compare(L"0NaN")==0 || s4.compare(L"0nan")==0;
    auto isInf = s4.compare(L"0+oo")==0 || s4.compare(L"0-oo")==0;
    if (isNan || isInf)
    {
        unsigned int i=4;
        auto x = getInt32(s,i);
        getChar(s,i,'e');
        auto y = getInt32(s,i);
        if (isNan)
            return Float::nan(x,y);
        else
            return Float::inf(sign(s4[1]),x,y);
    }else{
        unsigned int i=0;
        auto sign = getSign(s,i);
        auto sig = getDec(s,i);
        getChar(s,i,'e');
        auto exp = getDec(s,i);
        getChar(s,i,'f');
        auto sigL = getInt32(s,i);
        getChar(s,i,'e');
        auto expL = getInt32(s,i);
        if (!inbits(sig,sigL) || !inbits(exp,expL))
            throw new StringConversionException();
        return Float(sign,sig,sigL,exp,expL);
    }
}

}}} //namespace frontend::boogie::ParseTree
