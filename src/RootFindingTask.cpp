#include "RootFindingTask.h"
#include "RootFindingExceptions.h"

#include <cmath>
#include <stdexcept>

void RootFindingTask::validateInputs(double left, double right, double tolerance, std::size_t maxIterations)
{
    if (!std::isfinite(left))
    {
        throw InvalidParameterException("left", left);
    }
    if (!std::isfinite(right))
    {
        throw InvalidParameterException("right", right);
    }
    if (!std::isfinite(tolerance))
    {
        throw InvalidParameterException("tolerance", tolerance);
    }
    if (left >= right)
    {
        throw std::invalid_argument("Left boundary must be less than right boundary");
    }
    if (tolerance <= 0.0)
    {
        throw std::invalid_argument("Tolerance must be positive");
    }
    if (maxIterations == 0)
    {
        throw std::invalid_argument("Max iterations must be positive");
    }
}

RootFindingTask::RootFindingTask(double left, double right, double tolerance, std::size_t maxIterations)
{
    configure(left, right, tolerance, maxIterations);
}

double RootFindingTask::left() const noexcept
{
    return left_;
}

double RootFindingTask::right() const noexcept
{
    return right_;
}

double RootFindingTask::tolerance() const noexcept
{
    return tolerance_;
}

std::size_t RootFindingTask::maxIterations() const noexcept
{
    return maxIterations_;
}

bool RootFindingTask::hasSolutions() const noexcept
{
    return solutions_.has_value();
}

RootSolutions RootFindingTask::solutions() const
{
    if (!solutions_.has_value())
    {
        throw std::logic_error("Solutions have not been computed yet");
    }
    return *solutions_;
}

void RootFindingTask::configure(double left, double right, double tolerance, std::size_t maxIterations)
{
    validateInputs(left, right, tolerance, maxIterations);

    left_ = left;
    right_ = right;
    tolerance_ = tolerance;
    maxIterations_ = maxIterations;
}

double RootFindingTask::solveBisection(const Function& f) const
{
    if (!f)
    {
        throw std::invalid_argument("Function must be callable");
    }

    double a = left_;
    double b = right_;
    double fa = f(a);
    double fb = f(b);

    if (!std::isfinite(fa) || !std::isfinite(fb))
    {
        throw std::domain_error("Function value is non-finite on interval boundaries");
    }
    if (fa * fb > 0.0)
    {
        throw std::domain_error("Bisection requires opposite signs at interval endpoints");
    }

    double mid = a;
    for (std::size_t iter = 1; iter <= maxIterations_; ++iter)
    {
        mid = a + (b - a) * 0.5;
        const double fm = f(mid);
        if (!std::isfinite(fm))
        {
            throw std::domain_error("Function value is non-finite during bisection");
        }
        if (std::abs(fm) <= tolerance_ || (b - a) * 0.5 <= tolerance_)
        {
            return mid;
        }
        if (fa * fm < 0.0)
        {
            b = mid;
            fb = fm;
        }
        else
        {
            a = mid;
            fa = fm;
        }
    }

    throw ConvergenceException("bisection", maxIterations_);
}

double RootFindingTask::solveSecant(const Function& f) const
{
    if (!f)
    {
        throw std::invalid_argument("Function must be callable");
    }

    double x0 = left_;
    double x1 = right_;
    double f0 = f(x0);
    double f1 = f(x1);

    if (!std::isfinite(f0) || !std::isfinite(f1))
    {
        throw std::domain_error("Function value is non-finite in secant method");
    }

    for (std::size_t iter = 1; iter <= maxIterations_; ++iter)
    {
        const double denom = f1 - f0;
        if (std::abs(denom) <= 1e-15)
        {
            throw std::domain_error("Zero denominator in secant method");
        }

        const double x2 = x1 - f1 * (x1 - x0) / denom;
        if (!std::isfinite(x2))
        {
            throw std::overflow_error("Secant iteration overflow");
        }

        const double f2 = f(x2);
        if (!std::isfinite(f2))
        {
            throw std::domain_error("Function value is non-finite in secant iteration");
        }

        if (std::abs(x2 - x1) <= tolerance_ || std::abs(f2) <= tolerance_)
        {
            return x2;
        }

        x0 = x1;
        f0 = f1;
        x1 = x2;
        f1 = f2;
    }

    throw ConvergenceException("secant", maxIterations_);
}

double RootFindingTask::solveNewton(const Function& f, const Function& df) const
{
    if (!f || !df)
    {
        throw std::invalid_argument("Function and derivative must be callable");
    }

    double x = 0.5 * (left_ + right_);
    for (std::size_t iter = 1; iter <= maxIterations_; ++iter)
    {
        const double fx = f(x);
        const double dfx = df(x);
        if (!std::isfinite(fx) || !std::isfinite(dfx))
        {
            throw std::domain_error("Non-finite function or derivative in Newton method");
        }
        if (std::abs(dfx) <= 1e-15)
        {
            throw std::domain_error("Zero derivative in Newton method");
        }

        const double nextX = x - fx / dfx;
        if (!std::isfinite(nextX))
        {
            throw std::overflow_error("Newton iteration overflow");
        }
        if (std::abs(nextX - x) <= tolerance_)
        {
            return nextX;
        }
        x = nextX;
    }

    throw ConvergenceException("newton", maxIterations_);
}

void RootFindingTask::solveBothMethods(const Function& f)
{
    RootSolutions temp{};
    temp.bisectionRoot = solveBisection(f);
    temp.secantRoot = solveSecant(f);
    solutions_ = temp;
}

void RootFindingTask::solveBothMethodsUnsafe(const Function& f)
{
    RootSolutions current{};
    current.bisectionRoot = solveBisection(f);
    solutions_ = current;
    current.secantRoot = solveSecant(f);
    solutions_ = current;
}