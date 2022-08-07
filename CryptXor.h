#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <time.h>
#include <string.h>

using namespace std;
class CryptXor
{
    public:
        CryptXor();
        CryptXor(int argc,char* argv[]);
        void crypt(string pathTarget);
        void printManual();
        void testAverageLength();
    private:
        int mode[3]{0,0,0}; //-e=0 -d=1 -c -r
        void manageParams(int argc,char* argv[]);
        string target="";
        string password="";
        vector<vector<string>> inputFiles;
        int maxLengthInput[3]={1000000,10000000,20000000};   //img audio video
        fstream targetFile, inputFile;
};
