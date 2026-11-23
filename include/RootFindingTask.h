#pragma once

#include <cstddef>
#include <functional>
#include <optional>

struct RootSolutions
{
    double bisectionRoot = 0.0;
    double secantRoot = 0.0;
};

class RootFindingTask
{
public:
    using Function = std::function<double(double)>;

private:
    double left_ = 0.0;
    double right_ = 1.0;
    double tolerance_ = 1e-6;
    std::size_t maxIterations_ = 100;
    std::optional<RootSolutions> solutions_;

    static void validateInputs(double left, double right, double tolerance, std::size_t maxIterations);

public:
    RootFindingTask(double left, double right, double tolerance, std::size_t maxIterations);

    double left() const noexcept;
    double right() const noexcept;
    double tolerance() const noexcept;
    std::size_t maxIterations() const noexcept;
    bool hasSolutions() const noexcept;
    RootSolutions solutions() const;

    void configure(double left, double right, double tolerance, std::size_t maxIterations);
    double solveBisection(const Function& f) const;
    double solveSecant(const Function& f) const;
    double solveNewton(const Function& f, const Function& df) const;
    void solveBothMethods(const Function& f);
    void solveBothMethodsUnsafe(const Function& f);
};