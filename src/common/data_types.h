#ifndef COMMON__DATA_TYPES_H
#define COMMON__DATA_TYPES_H
#include <string>
#include <boost/multiprecision/cpp_int.hpp>

namespace common{
    typedef wchar_t     Char;
    typedef boost::multiprecision::cpp_int      Integer;
    typedef boost::multiprecision::cpp_rational Rational;
    typedef bool        Boolean;
    typedef std::wstring String;
    typedef std::wstring ID;
}

#endif
