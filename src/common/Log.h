#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <map>
#include <ostream>
#include <string>
namespace common {
using std::map;
using std::ostream;
using std::string;

class Log
{
    public:
        Log(ostream& out);
        virtual ~Log();

        typedef enum {
            Info,
            Warning,
            Error
        } Severity;

        typedef int Verbosity;

        void log(Verbosity v,Severity s,const string& message);
    protected:

    private:
        Verbosity verbosity=0;
        ostream& out;


};

} // namespace common

#endif // COMMON_LOG_H
