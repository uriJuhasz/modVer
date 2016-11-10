#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "common/Log.h"

using namespace std;
using namespace common;

const string logName = "run.log";

string time2String(const tm& t){
    stringstream ss;
    ss << put_time(&t, "%d-%m-%Y %H-%M-%S");
    return ss.str();
}

int main(int argc, char*argv[])
{
    cout << "Start" << endl;

    ofstream logFile(logName.c_str(), ofstream::out);
    Log log(logFile);
    log.log(0,Log::Info,"Starting log");
//    auto now = chrono::system_clock::now();
    auto nowT = time(nullptr);
    auto now = localtime(&nowT);
    auto nowString = time2String(*now);
    log.log(0,Log::Info,nowString);

    vector<string> args(argv,argv+argc);
    for (unsigned int i=0;i<args.size();i++){
        cout << i << ": " << args[i] << endl;
        i++;
    }

    log.log(0,Log::Info,"Closing log");
    cout << "End" << endl;
    return 0;
}
