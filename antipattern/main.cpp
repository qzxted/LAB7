#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
    void throwIntError()
    {
        throw 42;
    }

    void throwStringError()
    {
        throw std::string("String exception");
    }

    void throwStdError()
    {
        throw std::invalid_argument("Invalid argument");
    }

    void rethrowSliced()
    {
        try
        {
            throw std::invalid_argument("Derived error");
        }
        catch (const std::exception& ex)
        {
            throw ex;
        }
    }
}

int main()
{
    try
    {
        throwIntError();
    }
    catch (const std::exception& ex)
    {
        std::cout << "std::exception caught int: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "catch (...) caught primitive int\n";
    }

    try
    {
        throwStringError();
    }
    catch (const std::exception& ex)
    {
        std::cout << "std::exception caught string: " << ex.what() << '\n';
    }
    catch (...)
    {
        std::cout << "catch (...) caught std::string\n";
    }

    try
    {
        rethrowSliced();
    }
    catch (const std::invalid_argument&)
    {
        std::cout << "Caught as std::invalid_argument\n";
    }
    catch (const std::exception&)
    {
        std::cout << "Caught as base std::exception due to slicing\n";
    }

    try
    {
        throwStdError();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Proper std::exception: " << ex.what() << '\n';
    }

    return 0;
}