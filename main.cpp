#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char*argv[])
{
    cout << "Start" << endl;
    vector<string> args(argv,argv+argc);
    for (unsigned int i=0;i<args.size();i++){
        cout << i << ": " << args[i] << endl;
        i++;
    }
    cout << "End" << endl;
    return 0;
}
