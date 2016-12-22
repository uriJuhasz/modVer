#ifndef COMMON__DATA_TYPES_H
#define COMMON__DATA_TYPES_H
#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <exception>

namespace common{
    typedef wchar_t     Char;
    typedef boost::multiprecision::cpp_int      Integer;
    typedef boost::multiprecision::cpp_rational Rational;
    typedef bool        Boolean;
    typedef std::wstring String;
    typedef std::wstring ID;
    
    class StringConversionException : public std::exception{};
    int32_t  string2Int32   (const std::wstring& s);
    Integer  string2Integer (const std::wstring& s);
    Rational string2Rational(const std::wstring& s);
}

#endif
