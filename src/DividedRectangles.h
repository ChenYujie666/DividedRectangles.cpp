#ifndef DIVIDED_RECTANGLES_H
#define DIVIDED_RECTANGLES_H

#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cassert>

const double DEFAULT_CCW_TOL = 1e-6;

double clamp(double a, double l, double u);

struct DirectRectangle
{
    std::vector<double> c;
    double y;
    std::vector<int> d;
    double r;

    DirectRectangle(std::vector<double> c, double y, std::vector<int> d, double r)
        : c(std::move(c)), y(y), d(std::move(d)), r(r) {}
};

bool are_equal(const DirectRectangle &a, const DirectRectangle &b, double tol);
bool is_ccw(const DirectRectangle &a, const DirectRectangle &b, const DirectRectangle &c, double tol);
std::vector<double> basis(int i, int n);
double compute_radius(const std::vector<int> &d);
std::vector<DirectRectangle> get_split_intervals(std::vector<DirectRectangle> &rects, double r_min);
std::vector<DirectRectangle> split_interval(const DirectRectangle &rect, const std::function<double(const std::vector<double> &)> &g);
std::vector<DirectRectangle> direct(const std::function<double(const std::vector<double> &)> &f,
                                    const std::vector<double> &lower_bound,
                                    const std::vector<double> &upper_bound,
                                    int max_iterations,
                                    double min_radius);
std::vector<double> optimize(const std::function<double(const std::vector<double> &)> &f,
                              const std::vector<double> &lower_bound,
                              const std::vector<double> &upper_bound,
                              int max_iterations,
                              double min_radius);

double test_func1(const std::vector<double> &x);
double test_func2(const std::vector<double> &x);
double test_func3(const std::vector<double> &x);
double test_func4(const std::vector<double> &x);
double test_func5(const std::vector<double> &x);
double test_func6(const std::vector<double> &x);

#endif // DIVIDED_RECTANGLES_H