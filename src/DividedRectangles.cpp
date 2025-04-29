/**
 * @file DividedRectangles.cpp
 * @brief Implementation of a DIRECT (DIviding RECTangles) optimization algorithm.
 * 
 * This file contains the implementation of the DIRECT optimization algorithm, which is
 * a derivative-free global optimization method. The algorithm divides the search space
 * into hyper-rectangles and iteratively refines the search by splitting intervals based
 * on a given objective function. The implementation supports multi-dimensional optimization.
 * 
 * The file also includes several test functions to demonstrate the usage of the optimization
 * algorithm.
 * 
 * @details
 * - The `DirectRectangle` structure represents a hyper-rectangle in the search space.
 * - The `direct` function implements the main DIRECT algorithm.
 * - The `optimize` function provides a user-friendly interface for optimization.
 * - Several helper functions are included for interval splitting, radius computation,
 *   and convex hull construction.
 * - Debugging information can be written to files when `DEBUG_MODE` is enabled.
 * 
 * @dependencies
 * - Standard C++ libraries: `<vector>`, `<cmath>`, `<algorithm>`, `<functional>`, `<numeric>`,
 *   `<iterator>`, `<iostream>`, `<cassert>`, `<fstream>`.
 * - Custom header: `"DividedRectangles.h"`.
 * 
 * @functions
 * - `clamp`: Clamps a value between lower and upper bounds.
 * - `are_equal`: Compares two `DirectRectangle` objects for equality within a tolerance.
 * - `is_ccw`: Checks if three rectangles form a counter-clockwise turn.
 * - `basis`: Generates a basis vector for a given dimension.
 * - `compute_radius`: Computes the radius of a rectangle based on its division levels.
 * - `get_split_intervals`: Identifies candidate rectangles for splitting.
 * - `split_interval`: Splits a rectangle into smaller rectangles based on the objective function.
 * - `direct`: Implements the DIRECT optimization algorithm.
 * - `optimize`: Provides a simplified interface for optimization.
 * - Test functions (`test_func1` to `test_func6`): Example objective functions for testing.
 * 
 * @debugging
 * - Debugging information is written to files in the `./debugdata/` directory when
 *   `DEBUG_MODE` is defined.
 * - Debug files include `rects.txt`, `candidates.txt`, and `new_rects.txt`.
 * 
 * @usage
 * - Include this file and the corresponding header in your project.
 * - Define an objective function as a `std::function<double(const std::vector<double>&)>`.
 * - Call the `optimize` function with the objective function, bounds, and optional parameters.
 * 
 * @example
 * ```
 * std::vector<double> lower_bound = {0.0};
 * std::vector<double> upper_bound = {1.0};
 * auto result = optimize(test_func1, lower_bound, upper_bound);
 * std::cout << "Optimal value: " << result[0] << std::endl;
 * ```
 */
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <iostream>
#include <cassert>
#include <fstream>

#include "DividedRectangles.h"

#define DEBUG_MODE
#ifdef DEBUG_MODE
void write_debug_info(const std::vector<DirectRectangle> &rects, const std::string &filename, int iter)
{
    std::ofstream debug_file(filename, std::ios_base::app);
    for (const auto &rect : rects)
    {
        debug_file << iter << "\t" << rect.r << "\t" << rect.y << "\t[";
        for (size_t i = 0; i < rect.c.size(); ++i)
        {
            debug_file << rect.c[i];
            if (i < rect.c.size() - 1)
                debug_file << ", ";
        }
        debug_file << "]\t[";
        for (size_t i = 0; i < rect.d.size(); ++i)
        {
            debug_file << rect.d[i];
            if (i < rect.d.size() - 1)
                debug_file << ", ";
        }
        debug_file << "]" << std::endl;
    }
    debug_file.close();
}
#endif

// Function to clamp a value between lower and upper bounds


double clamp(double a, double l, double u)
{
    if (a < l)
        return l;
    if (a > u)
        return u;
    return a;
}

bool are_equal(const DirectRectangle &a, const DirectRectangle &b, double tol = 1e-9)
{
    if (a.c.size() != b.c.size())
        return false;
    for (size_t i = 0; i < a.c.size(); ++i)
    {
        if (std::abs(a.c[i] - b.c[i]) > tol)
            return false;
    }
    if (std::abs(a.y - b.y) > tol)
        return false;
    if (a.d != b.d)
        return false;
    if (std::abs(a.r - b.r) > tol)
        return false;
    return true;
}

bool is_ccw(const DirectRectangle &a, const DirectRectangle &b, const DirectRectangle &c, double tol = DEFAULT_CCW_TOL)
{
    double val = a.r * (b.y - c.y) - a.y * (b.r - c.r) + (b.r * c.y - b.y * c.r);
    return val < tol;
}

std::vector<double> basis(int i, int n)
{
    std::vector<double> e(n, 0.0);
    e[i] = 1.0;
    return e;
}

double compute_radius(const std::vector<int> &d)
{
    double sum = 0.0;
    for (int di : d)
    {
        double term = 0.5 * std::pow(3.0, -di);
        sum += term * term;
    }
    return std::sqrt(sum);
}

std::vector<DirectRectangle> get_split_intervals(std::vector<DirectRectangle> &rects, double r_min)
{
    std::sort(rects.begin(), rects.end(), [r_min](const DirectRectangle &a, const DirectRectangle &b)
              { return (std::abs(a.r - b.r) >= r_min) ? (a.r < b.r) : (a.y < b.y); });

    std::vector<DirectRectangle> hull;
    for (const auto &rect : rects)
    {
        if (!hull.empty() && std::abs(rect.r - hull.back().r) < 1e-9)
        {
            continue;
        }

        if (!hull.empty() && rect.y <= hull.back().y)
        {
            hull.pop_back();
        }

        if (hull.size() >= 2)
        {
            const DirectRectangle &a = hull.end()[-2];
            const DirectRectangle &b = hull.end()[-1]; // b.y is greater than a.y

            if (is_ccw(a, b, rect))
            {
                hull.pop_back();
            }
        }

        hull.push_back(rect);
    }

    auto it = std::remove_if(hull.begin(), hull.end(), [r_min](const DirectRectangle &rect)
                             { return rect.r < r_min - 1e-9; });
    hull.erase(it, hull.end());

    return hull;
}

std::vector<DirectRectangle> split_interval(const DirectRectangle &rect, const std::function<double(const std::vector<double> &)> &g)
{
    std::vector<double> c = rect.c;
    int n = c.size();
    std::vector<int> d = rect.d;
    int d_min = *std::min_element(d.begin(), d.end());

    std::vector<int> dirs;
    for (int i = 0; i < d.size(); ++i)
    {
        if (d[i] == d_min)
            dirs.push_back(i);
    }

    double delta = std::pow(3.0, -d_min - 1);
    std::vector<std::pair<std::vector<double>, std::vector<double>>> Cs;
    std::vector<std::pair<double, double>> Ys;

    for (int i : dirs)
    {
        std::vector<double> e = basis(i, n);
        std::vector<double> c_plus = c;
        std::vector<double> c_minus = c;
        for (int k = 0; k < n; ++k)
        {
            c_plus[k] = clamp(c_plus[k] + delta * e[k], 0.0, 1.0);
            c_minus[k] = clamp(c_minus[k] - delta * e[k], 0.0, 1.0);
        }
        Ys.emplace_back(g(c_plus), g(c_minus));
        Cs.emplace_back(std::move(c_plus), std::move(c_minus));
    }

    std::vector<double> minvals;
    for (const auto &y : Ys)
    {
        minvals.push_back(std::min(y.first, y.second));
    }

    std::vector<size_t> indices(dirs.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&minvals](size_t a, size_t b)
              { return minvals[a] < minvals[b]; });

    std::vector<DirectRectangle> new_rects;
    std::vector<int> current_d = d;

    for (size_t idx : indices)
    {
        int dir = dirs[idx];
        current_d[dir] += 1;
        double r = compute_radius(current_d);
        new_rects.emplace_back(Cs[idx].first, Ys[idx].first, current_d, r);
        new_rects.emplace_back(Cs[idx].second, Ys[idx].second, current_d, r);
    }

    new_rects.emplace_back(c, rect.y, current_d, compute_radius(current_d));
    return new_rects;
}

std::vector<DirectRectangle> direct(const std::function<double(const std::vector<double> &)> &f,
                                    const std::vector<double> &lower_bound,
                                    const std::vector<double> &upper_bound,
                                    int max_iterations = 100,
                                    double min_radius = 1e-5)
{
    int n = lower_bound.size();
    auto g = [&](const std::vector<double> &x)
    {
        std::vector<double> scaled(n);
        for (int i = 0; i < n; ++i)
        {
            scaled[i] = x[i] * (upper_bound[i] - lower_bound[i]) + lower_bound[i];
        }
        return f(scaled);
    };

    std::vector<double> center(n, 0.5);
    std::vector<DirectRectangle> rects;
    rects.emplace_back(center, g(center), std::vector<int>(n, 0), compute_radius(std::vector<int>(n, 0)));

    for (int k = 0; k < max_iterations; ++k)
    {
#ifdef DEBUG_MODE
        write_debug_info(rects, "./debugdata/rects.txt", k);
#endif
        auto candidates = get_split_intervals(rects, min_radius);

#ifdef DEBUG_MODE
        write_debug_info(candidates, "./debugdata/candidates.txt", k);
#endif
        std::vector<DirectRectangle> new_rects;
        for (const auto &rect : rects)
        {
            bool found = false;
            for (const auto &c : candidates)
            {
                if (are_equal(rect, c))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                new_rects.push_back(rect);
        }

        for (const auto &c : candidates)
        {
            auto split = split_interval(c, g);
            new_rects.insert(new_rects.end(), split.begin(), split.end());
        }

        rects = std::move(new_rects);

#ifdef DEBUG_MODE
        write_debug_info(rects, "./debugdata/new_rects.txt", k+1);
#endif

    }

    return rects;
}

std::vector<double> optimize(const std::function<double(const std::vector<double> &)> &f,
                             const std::vector<double> &lower_bound,
                             const std::vector<double> &upper_bound,
                             int max_iterations = 100,
                             double min_radius = 1e-5)
{
    auto rects = direct(f, lower_bound, upper_bound, max_iterations, min_radius);
    if (rects.empty())
        return std::vector<double>(lower_bound.size(), 0.5);

    auto best = std::min_element(rects.begin(), rects.end(),
                                 [](const DirectRectangle &a, const DirectRectangle &b)
                                 {
                                     return a.y < b.y;
                                 });

    std::vector<double> result;
    for (size_t i = 0; i < best->c.size(); ++i)
    {
        result.push_back(best->c[i] * (upper_bound[i] - lower_bound[i]) + lower_bound[i]);
    }

    return result;
}
double test_func1(const std::vector<double> &x)
{
    return std::sin(x[0]) + std::sin(2 * x[0]) + std::sin(4 * x[0]) + std::sin(8 * x[0]);
}

double test_func2(const std::vector<double> &x)
{
    return 1 * x[0] * x[0] + x[1] * x[1] + 2;
}

double test_func3(const std::vector<double> &x)
{
    return x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + 3;
}

double test_func4(const std::vector<double> &x)
{
    // A simple quadratic function
    float x0_bias = 1;
    return (x[0] + x0_bias) * (x[0] + x0_bias) + (x[1] + x0_bias) * (x[1] + x0_bias) + (x[2] + x0_bias) * (x[2] + x0_bias) + (x[3] + x0_bias) * (x[3] + x0_bias) + 4;
}

double test_func5(const std::vector<double> &x)
{
    // A Rosenbrock function (common optimization benchmark)
    return x[0] * x[0] + x[1] * x[1] + x[2] * x[2] + x[3] * x[3] + x[4] * x[4] + 5;
}

double test_func6(const std::vector<double> &x)
{
    // A sinusoidal function with multiple dimensions
    return x[0] + x[1] + x[2] + x[3] + x[4] + x[5];
}
