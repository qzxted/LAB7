#include "RootFindingExceptions.h"
#include "RootFindingTask.h"
#include "ScopeLogger.h"

#include <exception>
#include <iostream>
#include <stdexcept>

namespace
{
    double sampleFn(double x)
    {
        return x * x - 4.0;
    }

    void level3RunTask(RootFindingTask& task)
    {
        ScopeLogger logger("level3RunTask");
        task.solveBothMethods(sampleFn);
    }

    void level2MiddleLayer(RootFindingTask& task)
    {
        ScopeLogger logger("level2MiddleLayer");
        try
        {
            level3RunTask(task);
        }
        catch (const std::exception& ex)
        {
            std::cout << "[RETHROW] level2 caught: " << ex.what() << '\n';
            throw;
        }
    }

    void level1Entry(RootFindingTask& task)
    {
        ScopeLogger logger("level1Entry");
        level2MiddleLayer(task);
    }
}

int main()
{
    try
    {
        RootFindingTask validTask(1.0, 3.0, 1e-7, 100);
        validTask.solveBothMethods(sampleFn);
        const auto res = validTask.solutions();
        std::cout << "Valid roots -> bisection: " << res.bisectionRoot
                  << ", secant: " << res.secantRoot << '\n';

        RootFindingTask noSignChangeTask(3.0, 5.0, 1e-7, 100);
        level1Entry(noSignChangeTask);
    }
    catch (const InvalidParameterException& ex)
    {
        std::cout << "Invalid parameter (" << ex.parameter() << "): " << ex.what() << '\n';
    }
    catch (const ConvergenceException& ex)
    {
        std::cout << "Convergence error after " << ex.iterations() << " iterations: " << ex.what() << '\n';
    }
    catch (const RootFindingException& ex)
    {
        std::cout << "Root finding error: " << ex.what() << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << "Standard exception: " << ex.what() << '\n';
    }

    return 0;
}