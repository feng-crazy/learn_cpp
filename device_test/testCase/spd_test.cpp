/*
 * spd_test.cpp
 *
 *  Created on: Dec 28, 2017
 *      Author: hdf_123
 */


#include <iostream>
#include "spdlog/spdlog.h"

int main(int, char* [])
{
    namespace spd = spdlog;

    try
    {
        std::string filename = "spdlog_example";
        auto console = spd::stdout_logger_mt("console");
        console->info("Welcome to spdlog!") ;
//        console->info() << "Creating file " << filename << "..";

        auto file_logger = spd::rotating_logger_mt("file_logger", filename, 1024 * 1024 * 5, 3);
        file_logger->info("Log file message number", 1);

        for (int i = 0; i < 100; ++i)
        {
            auto square = i*i;
//            file_logger->info() << i << '*' << i << '=' << square << " (" << "0x" << std::hex << square << ")";
        }

        // Change log level to all loggers to warning and above
        spd::set_level(spd::level::warn);
        console->info("This should not be displayed");
        console->warn("This should!");
        spd::set_level(spd::level::info);

        // Change format pattern to all loggers
        spd::set_pattern("  %Y-%m-%d %H:%M:%S.%e %l %v");
        spd::get("console")->info("This is another message with different format");

//        spd::set_level(spd::level::warn);
        int count = 0;
        int i = 0;
        float f = 0.0;
//        console->warn("hdf:spd test:count = %d i = %d f = {:f}", count,i,f);//不支持的格式
        console->warn("haha hdf:spd test:count = %d i = %d", count,i);
        console->warn() << "hdf:spd test:count";//不支持的格式
        count++;i++;f++;
        console->warn("hdf:spd test:count = {} i = {} f = {:f}", count,i,f);
        count++;i++;f++;
        console->warn("hdf:spd test:count = {} i = {} f = {:f}", count,i,f);
        console->warn("hdf:spd test:{}",f);

        console->error("Some error message with arg{}..", 1);

		// Conditional logging example
//		console->info_if(true, "Welcome to spdlog conditional logging!");

		// Formatting examples
		console->warn("Easy padding in numbers like {:08d}", 12);
		console->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
		console->info("Support for floats {:03.2f}", 1.23456);
		console->info("Positional args are {1} {0}..", "too", "supported");
		console->info("{:<30}", "left aligned");


        char test_ch[] = "hdf:spd test";
        console->warn("test_ch[]");
    }
    catch (const spd::spdlog_ex& ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
    return 0;
}

