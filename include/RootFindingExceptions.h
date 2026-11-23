#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

class RootFindingException : public std::runtime_error
{
public:
    explicit RootFindingException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class InvalidParameterException : public RootFindingException
{
private:
    std::string parameter_;
    double value_;

public:
    InvalidParameterException(const std::string& parameter, double value)
        : RootFindingException("Invalid parameter: " + parameter),
          parameter_(parameter),
          value_(value)
    {
    }

    const std::string& parameter() const noexcept
    {
        return parameter_;
    }

    double value() const noexcept
    {
        return value_;
    }
};

class ConvergenceException : public RootFindingException
{
private:
    std::size_t iterations_;

public:
    ConvergenceException(const std::string& methodName, std::size_t iterations)
        : RootFindingException("Convergence failed in " + methodName),
          iterations_(iterations)
    {
    }

    std::size_t iterations() const noexcept
    {
        return iterations_;
    }
};