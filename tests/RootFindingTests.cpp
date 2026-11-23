#include "RootFindingExceptions.h"
#include "RootFindingTask.h"

#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

TEST(RootFindingTaskTest, SolvesQuadraticRootWithTolerance)
{
    RootFindingTask task(1.0, 3.0, 1e-9, 100);
    const auto f = [](double x) { return x * x - 4.0; };
    const auto df = [](double x) { return 2.0 * x; };

    EXPECT_NEAR(task.solveBisection(f), 2.0, 1e-8);
    EXPECT_NEAR(task.solveSecant(f), 2.0, 1e-8);
    EXPECT_NEAR(task.solveNewton(f, df), 2.0, 1e-8);
}

TEST(RootFindingTaskTest, RejectsInvalidIntervalAndNonFiniteValues)
{
    EXPECT_THROW(RootFindingTask(5.0, 2.0, 1e-6, 50), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(std::numeric_limits<double>::quiet_NaN(), 2.0, 1e-6, 50), InvalidParameterException);
    EXPECT_THROW(RootFindingTask(1.0, std::numeric_limits<double>::infinity(), 1e-6, 50), InvalidParameterException);
}

TEST(RootFindingTaskTest, ThrowsWhenNoSignChangeInBisection)
{
    RootFindingTask task(3.0, 5.0, 1e-6, 50);
    const auto f = [](double x) { return x * x - 4.0; };
    EXPECT_THROW(task.solveBisection(f), std::domain_error);
}

TEST(RootFindingTaskTest, ThrowsOnZeroDerivativeInNewton)
{
    RootFindingTask task(1.0, 3.0, 1e-6, 50);
    const auto f = [](double x) { return x * x - 4.0; };
    const auto zeroDeriv = [](double) { return 0.0; };
    EXPECT_THROW(task.solveNewton(f, zeroDeriv), std::domain_error);
}

TEST(RootFindingTaskTest, ThrowsOnConvergenceFailure)
{
    RootFindingTask task(1.0, 2.0, 1e-12, 2);
    const auto f = [](double x) { return x * x - 2.0; };
    EXPECT_THROW(task.solveBisection(f), ConvergenceException);
}

TEST(RootFindingTaskTest, StrongGuaranteePreservesPreviousSolutions)
{
    RootFindingTask task(1.0, 3.0, 1e-8, 100);
    const auto goodFn = [](double x) { return x * x - 4.0; };
    task.solveBothMethods(goodFn);
    const double savedBisection = task.solutions().bisectionRoot;
    const double savedSecant = task.solutions().secantRoot;

    task.configure(3.0, 5.0, 1e-8, 100);
    EXPECT_THROW(task.solveBothMethods(goodFn), std::domain_error);

    EXPECT_TRUE(task.hasSolutions());
    EXPECT_DOUBLE_EQ(task.solutions().bisectionRoot, savedBisection);
    EXPECT_DOUBLE_EQ(task.solutions().secantRoot, savedSecant);
}

TEST(RootFindingTaskTest, UnsafeMethodDemonstratesStateCorruption)
{
    RootFindingTask task(1.0, 3.0, 1e-8, 100);
    const auto goodFn = [](double x) { return x * x - 4.0; };
    task.solveBothMethods(goodFn);
    const double savedBisection = task.solutions().bisectionRoot;

    task.configure(3.0, 5.0, 1e-8, 100);
    EXPECT_THROW(task.solveBothMethodsUnsafe(goodFn), std::domain_error);
    EXPECT_DOUBLE_EQ(task.solutions().bisectionRoot, savedBisection);
}