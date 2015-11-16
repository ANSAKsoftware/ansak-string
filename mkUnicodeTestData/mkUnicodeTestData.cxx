///////////////////////////////////////////////////////////////////////////
//
// 2015.11.22
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
///////////////////////////////////////////////////////////////////////////
//
// mkUnicodeTestDataxx -- uses istream/ostream and ansak::split to generate
//                        test data for the isUnicode8 unit tests, because
//                        AWK isn't universally available.
//
///////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <fstream>

#include <string>
#include <vector>
#include "string_splitjoin.hxx"

using namespace std;
using namespace ansak;

///////////////////////////////////////////////////////////////////////////
// process -- once the two files have been determined and opened/created,
// convert the one format to the other

void process(filebuf& inbuf, filebuf& outbuf)
{
    istream inStream(&inbuf);
    ostream outStream(&outbuf);

    string oneLine;
    getline(inStream, oneLine);
    auto fields = split(oneLine, ';');
    if (!fields.empty())
    {
        outStream << "char32_t validUnicode8CodePoints[] = {" << endl << "    ";
        int thisLine = 0;
        bool firstOne = true;
        int nCount = 0;
        while (oneLine.size() > 10)
        {
            auto surrogateN = fields[1].find("Surrogate");
            auto privateN = fields[1].find("Private");
            if (surrogateN != string::npos || privateN != string::npos)
            {
                getline(inStream, oneLine);
                fields = split(oneLine, ';');
                continue;
            }

            if (!firstOne && nCount % 10 != 0)
            {
                outStream << ", ";
            }
            outStream << "0x" << fields[0];
            getline(inStream, oneLine);
            fields = split(oneLine, ';');
            firstOne = false;
            ++nCount;
            if (oneLine.size() != 0 && nCount % 10 == 0)
            {
                outStream << ',' << endl << "    ";
            }
        }

        outStream << endl << "};" << endl;
    }

    outbuf.close();
}

///////////////////////////////////////////////////////////////////////////
// main -- simple parameter parsing and set up for "process" above.

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
        cerr << "    to convert ;-delimited Unicode data into an array of test data." << endl;
        return 1;
    }

    // if argv[1] does not exist, complain and leave
    struct stat statData;
    auto rc = stat(argv[1], &statData);
    if (rc != 0)
    {
        cerr << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
        cerr << "    infile does not exist." << endl;
        return 2;
    }

    // if argv[2] can not be created, complain and leave
    bool opened1 = false;
    bool opened2 = false;
    try
    {
        filebuf inBuf;
        inBuf.open(argv[1], std::ios::in);
        opened1 = true;

        filebuf outBuf;
        outBuf.open(argv[2], std::ios::out | std::ios::trunc);
        opened2 = true;

        process(inBuf, outBuf);
        inBuf.close();
        outBuf.close();
    }
    catch (...)
    {
        cerr << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
        if (opened2)
        {
            cerr << "    an error occurred in processing the Unicode data." << endl;
            return 4;
        }
        else if (opened1)
        {
            cerr << "    outfile could not be created." << endl;
            return 3;
        }
        else
        {
            cerr << "    infile could not be opened." << endl;
            return 2;
        }
    }

    return 0;
}

