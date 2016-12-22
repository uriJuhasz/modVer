#include "ParseTree.h"

namespace frontend::boogie::ParseTree{

ConstantOrderSpec::ConstantOrderSpec(){}
ConstantOrderSpec::ConstantOrderSpec(const ConstantOrderSpec& o)
    : specified(o.specified), complete(o.complete), parents(o.parents){}
pConstantOrderSpec ConstantOrderSpec::clone() const{return make_unique<ConstantOrderSpec>(*this); }

bool isDigit(char c)
{
    return c>='0' && c<='9';
}

int char2Int(char c)
{
    if (!isDigit(c))
        throw new StringConversionException();
    return c-'0';
}

bool tryGetDigit(const string& s, int& i, int& o){
    if (i>=s.length() || !isDigit(s[i]))
        return false;
    auto c = s[i];
    i++;
    o=char2Int(c);
}
int getDigit(const string& s, int& i){
    int o;
    if  (!tryGetDigit(s,i,o))
        throw new StringConversionException();
    return o;
}

Integer getDec(const string& s, int& i){
    Integer r=getDigit(s,i);
    while (tryGetDigit(s,i))
        r=r*10+d;
}
enum class Sign{ Pos,Neg };

Sign getSign(const string& s, int& i){
    if (i>=s.length())
        return Sign::Pos;
    if
    Integer r=getDigit(s,i);
    while (tryGetDigit(s,i))
        r=r*10+d;
}

bool sign(char c){return c=="+" ? false : true; }

Float string2Float   (const String& s)
{
    /* From Boogie:
     * String must be either of the format *e*f*e*
     * or of the special value formats: 0NaN*e* 0nan*e* 0+oo*e* 0-oo*e*
     * Where * indicates an integer value (digit)
     */
    if (s.length()>=7)
    {
        auto s4 = s.substr(0,4);
        auto isNan = s4=="0NaN" || s4=="0nan";
        auto isInf = s4=="0+oo" || s4=="0-oo";
        if (isNan || isInf)
        {
            int i=4;
            int x = getDec(s,i);
            getChar(s,i,'e');
            int y = getDec(s,i);
            if (isNan)
                return Float::nan(x,y);
            else
                return Float::inf(sign(s4[1]),x,y);
        }else{
            int i=0;
            auto sign = getSign(s,i);
            auto sig = getDec(s,i);
            getChar(s,i,'e');
            auto exp = getDec(s,i);
            getChar(s,i,'f');
            auto sigL = getDec(s,i);
            getChar(s,i,'e');
            auto expL = getDec(s,i);
            if (!inbits(sig,sigL) || !inbits(exp,expL))
                throw new StringConversionException();
            return Float(sign,sig,sigL,exp,expL);
        }
    }
}

} //namespace frontend::boogie::ParseTree
