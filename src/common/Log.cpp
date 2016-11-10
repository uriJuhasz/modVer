#include "common/Log.h"
#include <iostream>

using namespace std;
namespace common {

Log::Log(ostream& _out) : out(_out)
{
    //ctor
}

Log::~Log()
{
    //dtor
}

const map<const Log::Severity,const string> severity2StringMap =
    {{Log::Info,"I"},{Log::Warning,"W"},{Log::Error,"E"}};

void Log::log(Log::Verbosity v,Log::Severity s,const string& message){
    if (v>verbosity)
        return;
    cout << severity2StringMap.at(s) << ":" << message << endl;
    out << severity2StringMap.at(s) << ":" << message << endl;
}


} // namespace common
