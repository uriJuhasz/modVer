#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "common/Log.h"
#include "frontend/boogie/coco/Parser.h"

using namespace std;
using namespace common;
using std::chrono::system_clock;

const string logName = "run.log";

string toString(const chrono::system_clock::time_point& t){
	auto t2 = system_clock::to_time_t(t);
//	return ctime(&t2, "%d-%m-%Y %H-%M-%S");
    auto tm = std::localtime(&t2);
	stringstream ss;
	ss << put_time(tm, "%d-%m-%Y %H-%M-%S");
    return ss.str();
}

const string boogieFileName = "../../test/AVLTree.bpl";

int parseBoogieFile(const string& fileName);
int main(int argc, char*argv[])
{
    cout << "Start" << endl;

    ofstream logFile(logName.c_str(), ofstream::out);
    Log log(logFile);
    log.log(0,Log::Info,"Starting log");
//    auto now = chrono::system_clock::now();
	auto now = chrono::system_clock::now();
	auto nowString = toString(now);
    log.log(0,Log::Info,"current time:" + nowString);

    cout << "I:Command line:" << endl;
    vector<string> args(argv,argv+argc);
    for (unsigned int i=0;i<args.size();i++){
        cout << "*I:" << i << ": " << args[i] << endl;
        i++;
    }

    auto r = parseBoogieFile(boogieFileName);
    if (r!=0)
        log.log(0,Log::Error,"Failed to parse boogie file \"" + boogieFileName + "\"\n");
    
    log.log(0,Log::Info,"Closing log");
    std::cin.get();
    cout << "End" << endl;
    cin.get();
    return 0;
}
/*
int parseBoogieFile1(const string& boogieFileName){
    cout << "I:Opening Boogie file \"" << boogieFileName << "\"" << endl;
    ifstream boogieFile(boogieFileName,ios::binary);
    if (!boogieFile.good() || !boogieFile.is_open()){
        cerr << "E:Failed to open Boogie file." << endl;
        return 1;
    }

    
    cout << "I: Reading file" << endl;
    wstring input(
        (std::istreambuf_iterator<char>(boogieFile) ),
        (std::istreambuf_iterator<char>()));   
    cout << "  I: Total " << input.size() << " bytes" << endl;
        
    if (boogieFile.bad() || boogieFile.fail()){
        cerr << "E: Failed to read from Boogie file." << endl;
        return 2;
    }
    
    cout << "I: Parsing" << endl;
    frontend::boogie::parser::Parser parser;
    parser.parse();
//    frontend::boogie::AST::Program program;
//    frontend::boogie::parser::parse(input,program);
    
    cout << "I: Closing Boogie file \"" << boogieFileName << "\"" << endl;
    return 0;
}
*/
int parseBoogieFile(const string& boogieFN){

    cout << "I:Opening Boogie file \"" << boogieFN << "\"" << endl;
    ifstream boogieFile(boogieFN,ios::binary);
    if (!boogieFile.good() || !boogieFile.is_open()){
        cerr << "E:Failed to open Boogie file." << endl;
        return 1;
    }

    cout << "I: Reading file" << endl;
    std::basic_string<unsigned char> input(
//    std::wstring input(
        (std::istreambuf_iterator<char>(boogieFile) ),
        (std::istreambuf_iterator<char>()));
    cout << "  I: Total " << input.size() << " bytes" << endl;

    if (boogieFile.bad() || boogieFile.fail()){
        cerr << "E: Failed to read from Boogie file." << endl;
        return 2;
    }

    cout << "I: Parsing" << endl;
    pProgram prog = frontend::boogie::parser::Parser::parse(input);
    auto ni = prog->implementations.size();
    cout << "I: Done parsing - implementations : " << ni << endl;

    cout << "I: Closing Boogie file \"" << boogieFN << "\"" << endl;
    return 0;
}
