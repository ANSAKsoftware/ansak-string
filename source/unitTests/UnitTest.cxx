///////////////////////////////////////////////////////////////////////////
//
// 2014.10.08
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
// UnitTest.cpp -- Simple framework within which to run unit tests.
//
///////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <stdexcept>

#include <cppunit/TestSuite.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>

using namespace std;

int main(int argc, char* argv[])
{
    string testPath = (argc > 1) ? string(argv[1]) : "";

    // Create the event manager and test controller
    CppUnit::TestResult controller;

    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener(&result);        

    // Add a listener that print dots as test run.
    CppUnit::TextTestProgressListener progress;
    controller.addListener(&progress);      

    // Add the top suite to the test runner
    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());

    try
    {
        cout << "Running "  <<  testPath;
        runner.run(controller, testPath);

        cerr << endl;

        // Print test in a compiler compatible format.
        CppUnit::CompilerOutputter outputter(&result, cerr);
        outputter.write();                      
    }
    catch (std::invalid_argument &e)  // Test path not resolved
    {
        cerr << endl  
             << "ERROR: "  <<  e.what()
             << endl;
        return 0;
    }

    return result.wasSuccessful() ? 0 : 1;
}

