#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <iostream>

const double DEFAULT_CCW_TOL = 1e-6;

double clamp(double a, double l, double u)
{
    if (a < l)
        return l;
    if (a > u)
        return u;
    return a;
}

struct DirectRectangle
{
    std::vector<double> c;
    double y;
    std::vector<int> d;
    double r;

    DirectRectangle(std::vector<double> c, double y, std::vector<int> d, double r)
        : c(std::move(c)), y(y), d(std::move(d)), r(r) {}
};

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
    std::sort(rects.begin(), rects.end(), [](const DirectRectangle &a, const DirectRectangle &b)
         { return (a.r != b.r) ? (a.r < b.r) : (a.y < b.y); });

    std::vector<DirectRectangle> hull;
    for (const auto &rect : rects)
    {
        if (!hull.empty() && std::abs(rect.r - hull.back().r) < 1e-9)
        {
            continue;
        }

        while (!hull.empty() && rect.y <= hull.back().y)
        {
            hull.pop_back();
        }

        while (hull.size() >= 2)
        {
            const DirectRectangle &a = hull.end()[-2];
            const DirectRectangle &b = hull.end()[-1];
            if (is_ccw(a, b, rect))
            {
                hull.pop_back();
            }
            else
            {
                break;
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
        auto candidates = get_split_intervals(rects, min_radius);

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
    return 1*x[0]*x[0]+x[1]*x[1]+2;
}

double test_func3(const std::vector<double> &x)
{
    return x[0]*x[0]+x[1]*x[1]+x[2]*x[2]+3;
}


double test_func4(const std::vector<double> &x)
{
    // A simple quadratic function
    float x0_bias = 1;
    return (x[0]+x0_bias)*(x[0]+x0_bias)+(x[1]+x0_bias)*(x[1]+x0_bias)+(x[2]+x0_bias)*(x[2]+x0_bias)+(x[3]+x0_bias)*(x[3]+x0_bias)+4;
}

double test_func5(const std::vector<double> &x)
{
    // A Rosenbrock function (common optimization benchmark)
    return x[0]*x[0]+x[1]*x[1]+x[2]*x[2]+x[3]*x[3]+x[4]*x[4]+5;
}


double test_func6(const std::vector<double> &x)
{
    // A sinusoidal function with multiple dimensions
    return x[0]+x[1]+x[2]+x[3]+x[4]+x[5];
}

int main()
{

    std::vector<double> lower_bound1 = {-2};
    std::vector<double> upper_bound1 = {2};

    // Test 1: 
    std::cout << "Optimizing test_func1  " << std::endl;
    auto result = optimize(test_func1, lower_bound1, upper_bound1, 100, 1e-5);
    for (auto r : result)
    {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    std::cout << "Result: " << test_func1(result) << std::endl;
    std::cout << std::endl;

    std::vector<double> lower_bound2 = {-2, -2};
    std::vector<double> upper_bound2 = {2, 2};



    // Test 2: More complex function
    std::cout << "Optimizing test_func2 with bounds [-2, 2] x [-2, 2]..." << std::endl;
    result = optimize(test_func2, lower_bound2, upper_bound2, 100, 1e-5);
    for (auto r : result)
    {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    std::cout << "Result: " << test_func2(result) << std::endl;
    std::cout << std::endl;




    // Test 3: 
    std::vector<double> lower_bound3 = {-2, -2, -2};
    std::vector<double> upper_bound3 = {2, 2, 2};
    std::cout << "Optimizing test_func4 with bounds [-2, 2] x [-2, 2]..." << std::endl;
    result = optimize(test_func3, lower_bound3, upper_bound3, 100, 1e-5);
    for (auto r : result)
    {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    std::cout << "Result: " << test_func3(result) << std::endl;
    std::cout << std::endl;



    // Test 4: 
    std::vector<double> lower_bound4 = {-2, -2, -2, -2};
    std::vector<double> upper_bound4 = {2, 2,2,2};
    std::cout << "Optimizing test_func4  with bounds [-2, 2] x [-2, 2]..." << std::endl;
    result = optimize(test_func4, lower_bound4, upper_bound4, 100, 1e-5);
    for (auto r : result)
    {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    std::cout << "Result: " << test_func4(result) << std::endl;
    std::cout << std::endl;

    // Test 5: 
    std::vector<double> lower_bound5 = {-3, -3, -3, -3, -3};
    std::vector<double> upper_bound5 = {3, 3, 3, 3, 3};
    std::cout << "Optimizing test_func5 with bounds [-3, 3] x [-3, 3] x [-3, 3] x [-3, 3]..." << std::endl;
    result = optimize(test_func5, lower_bound5, upper_bound5, 100, 1e-5);
    for (auto r : result)
    {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    std::cout << "Result: " << test_func5(result) << std::endl;
    std::cout << std::endl;


    // Test 6:  
    std::vector<double> lower_bound6 = {-1, -1, -1, -1, -1, -1};
    std::vector<double> upper_bound6 = {3, 3, 3, 3, 3,3};
    std::cout << "Optimizing test_func6  with bounds  " << std::endl;
    result = optimize(test_func6, lower_bound6, upper_bound6, 100, 1e-5);
    for (auto r : result)
    {
        std::cout << r << " ";
    }
    std::cout << std::endl;

    std::cout << "Result: " << test_func6(result) << std::endl;
    std::cout << std::endl;

    return 0;
}