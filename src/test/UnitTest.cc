#include "src/pch.h"
#include "src/core/common.h"
#include "UnitTest.h"
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>

int main(int argc, char* argv[])
{
    /*std::cout << "Initializing logging..." << std::endl;
    mmorpg::Logger::configure((mmorpg::conf_dir() / "testsuite.properties").string());

    mmorpg::Logger logger("mmorpg");
    logger.info("Initializing tests...");

    std::string testPath = (argc > 1) ? std::string(argv[1]) : "";
    CppUnit::TestResult controller;

    CppUnit::TestResultCollector result;
    controller.addListener(&result);

    CppUnit::TextTestProgressListener progress;
    controller.addListener(&progress);

    CppUnit::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    try {
        std::cout << "Running tests "  <<  testPath;
        runner.run(controller, testPath);
        std::cerr << std::endl;

        CppUnit::CompilerOutputter outputter(&result, std::cerr);
        outputter.write();
    } catch(const std::invalid_argument &e) {
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl;
        return 0;
    }

    return result.wasSuccessful() ? 0 : 1;*/
return 0;
}
