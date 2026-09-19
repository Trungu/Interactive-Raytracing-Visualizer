#pragma once
#include <string>
#include <iostream>

class TestRunner
{
private:
    int passed_ = 0;
    int failed_ = 0;

public:
    void check_test(const bool &passed, const std::string &message)
    {
        if (passed)
        {
            // pass messages are displayed in green
            passed_++;
            std::cout << "\033[32mPASS: " << message << "\033[0m\n";
        }
        else
        {
            // fail messages are displayed in red
            failed_++;
            std::cout << "\033[31mFAIL: " << message << "\033[0m\n";
        }
    }

    int finish() const
    {
        std::cout << passed_ << " cases passed, " << failed_ << " cases failed\n";

        return failed_ == 0 ? 0 : 1;
    }
};
