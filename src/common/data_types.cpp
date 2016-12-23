#include "data_types.h"
#include <locale>
#include <unordered_map>
#include <limits>

namespace common{

static const std::unordered_map<wchar_t,int32_t> digitMap = {
    {'0',0},
    {'1',1},
    {'2',2},
    {'3',3},
    {'4',4},
    {'5',5},
    {'6',6},
    {'7',7},
    {'8',8},
    {'9',9}
};

int32_t char2Int32(wchar_t c){
    return digitMap.at(c);
}

int32_t  string2Int32   (const std::wstring& s)
{
  if (s.length()==0)
      throw new StringConversionException();
  int64_t v = 0;
  for (auto c : s)
  { //allow leading 0s
      if (!isdigit(c))
          throw new StringConversionException();
      int i = char2Int32(c);
      v = v*10 + i;
      if (v>std::numeric_limits<int32_t>::max())
        throw new StringConversionException();
  }
  return (int32_t)v;
}

Integer  string2Integer (const std::wstring& s){
    Integer v = 0;
    for (auto c : s)
    { //allow leading 0s
        if (!isdigit(c))
            throw new StringConversionException();
        int i = char2Int32(c);
        v = v*10 + i;
    }
    return v;
}
/*
Rational string2Rational(const String& s)
{

}*/

}//namespace common
