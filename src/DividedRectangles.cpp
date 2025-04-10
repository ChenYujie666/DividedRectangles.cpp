#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <iostream>

using namespace std;

const double DEFAULT_CCW_TOL = 1e-6;

double clamp(double a, double l, double u){
    if(a < l) return l;
    if(a > u) return u;
    return a;
}

struct DirectRectangle {
    vector<double> c;
    double y;
    vector<int> d;
    double r;

    DirectRectangle(vector<double> c, double y, vector<int> d, double r)
        : c(move(c)), y(y), d(move(d)), r(r) {}
};

bool are_equal(const DirectRectangle& a, const DirectRectangle& b, double tol = 1e-9) {
    if (a.c.size() != b.c.size()) return false;
    for (size_t i = 0; i < a.c.size(); ++i) {
        if (abs(a.c[i] - b.c[i]) > tol) return false;
    }
    if (abs(a.y - b.y) > tol) return false;
    if (a.d != b.d) return false;
    if (abs(a.r - b.r) > tol) return false;
    return true;
}

bool is_ccw(const DirectRectangle& a, const DirectRectangle& b, const DirectRectangle& c, double tol = DEFAULT_CCW_TOL) {
    double val = a.r * (b.y - c.y) - a.y * (b.r - c.r) + (b.r * c.y - b.y * c.r);
    return val < tol;
}

vector<double> basis(int i, int n) {
    vector<double> e(n, 0.0);
    e[i] = 1.0;
    return e;
}

double compute_radius(const vector<int>& d) {
    double sum = 0.0;
    for (int di : d) {
        double term = 0.5 * pow(3.0, -di);
        sum += term * term;
    }
    return sqrt(sum);
}

vector<DirectRectangle> get_split_intervals(vector<DirectRectangle>& rects, double r_min) {
    sort(rects.begin(), rects.end(), [](const DirectRectangle& a, const DirectRectangle& b) {
        return (a.r != b.r) ? (a.r < b.r) : (a.y < b.y);
    });

    vector<DirectRectangle> hull;
    for (const auto& rect : rects) {
        if (!hull.empty() && abs(rect.r - hull.back().r) < 1e-9) {
            continue;
        }

        while (!hull.empty() && rect.y <= hull.back().y) {
            hull.pop_back();
        }

        while (hull.size() >= 2) {
            const DirectRectangle& a = hull.end()[-2];
            const DirectRectangle& b = hull.end()[-1];
            if (is_ccw(a, b, rect)) {
                hull.pop_back();
            } else {
                break;
            }
        }

        hull.push_back(rect);
    }

    auto it = remove_if(hull.begin(), hull.end(), [r_min](const DirectRectangle& rect) {
        return rect.r < r_min - 1e-9;
    });
    hull.erase(it, hull.end());

    return hull;
}

vector<DirectRectangle> split_interval(const DirectRectangle& rect, const function<double(const vector<double>&)>& g) {
    vector<double> c = rect.c;
    int n = c.size();
    vector<int> d = rect.d;
    int d_min = *min_element(d.begin(), d.end());

    vector<int> dirs;
    for (int i = 0; i < d.size(); ++i) {
        if (d[i] == d_min) dirs.push_back(i);
    }

    double delta = pow(3.0, -d_min - 1);
    vector<pair<vector<double>, vector<double>>> Cs;
    vector<pair<double, double>> Ys;

    for (int i : dirs) {
        vector<double> e = basis(i, n);
        vector<double> c_plus = c;
        vector<double> c_minus = c;
        for (int k = 0; k < n; ++k) {
            c_plus[k] = clamp(c_plus[k] + delta * e[k], 0.0, 1.0);
            c_minus[k] = clamp(c_minus[k] - delta * e[k], 0.0, 1.0);
        }
        Ys.emplace_back(g(c_plus), g(c_minus));
        Cs.emplace_back(move(c_plus), move(c_minus));
    }

    vector<double> minvals;
    for (const auto& y : Ys) {
        minvals.push_back(min(y.first, y.second));
    }

    vector<size_t> indices(dirs.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&minvals](size_t a, size_t b) {
        return minvals[a] < minvals[b];
    });

    vector<DirectRectangle> new_rects;
    vector<int> current_d = d;

    for (size_t idx : indices) {
        int dir = dirs[idx];
        current_d[dir] += 1;
        double r = compute_radius(current_d);
        new_rects.emplace_back(Cs[idx].first, Ys[idx].first, current_d, r);
        new_rects.emplace_back(Cs[idx].second, Ys[idx].second, current_d, r);
    }

    new_rects.emplace_back(c, rect.y, current_d, compute_radius(current_d));
    return new_rects;
}

vector<DirectRectangle> direct(const function<double(const vector<double>&)>& f,
                               const vector<double>& lower_bound,
                               const vector<double>& upper_bound,
                               int max_iterations = 100,
                               double min_radius = 1e-5) {
    int n = lower_bound.size();
    auto g = [&](const vector<double>& x) {
        vector<double> scaled(n);
        for (int i = 0; i < n; ++i) {
            scaled[i] = x[i] * (upper_bound[i] - lower_bound[i]) + lower_bound[i];
        }
        return f(scaled);
    };

    vector<double> center(n, 0.5);
    vector<DirectRectangle> rects;
    rects.emplace_back(center, g(center), vector<int>(n, 0), compute_radius(vector<int>(n, 0)));

    for (int k = 0; k < max_iterations; ++k) {
        auto candidates = get_split_intervals(rects, min_radius);

        vector<DirectRectangle> new_rects;
        for (const auto& rect : rects) {
            bool found = false;
            for (const auto& c : candidates) {
                if (are_equal(rect, c)) {
                    found = true;
                    break;
                }
            }
            if (!found) new_rects.push_back(rect);
        }

        for (const auto& c : candidates) {
            auto split = split_interval(c, g);
            new_rects.insert(new_rects.end(), split.begin(), split.end());
        }

        rects = move(new_rects);
    }

    return rects;
}

vector<double> optimize(const function<double(const vector<double>&)>& f,
                        const vector<double>& lower_bound,
                        const vector<double>& upper_bound,
                        int max_iterations = 100,
                        double min_radius = 1e-5) {
    auto rects = direct(f, lower_bound, upper_bound, max_iterations, min_radius);
    if (rects.empty()) return vector<double>(lower_bound.size(), 0.5);

    auto best = min_element(rects.begin(), rects.end(),
        [](const DirectRectangle& a, const DirectRectangle& b) {
            return a.y < b.y;
        });

    vector<double> result;
    for (size_t i = 0; i < best->c.size(); ++i) {
        result.push_back(best->c[i] * (upper_bound[i] - lower_bound[i]) + lower_bound[i]);
    }

    return result;
}
double my_function(const vector<double>& x) {
    return sin(x[0]) + sin(2*x[0]) + sin(4*x[0]) + sin(8*x[0]) + x[1];
}

int main(){

    vector<double> lower_bound = {-2, -2};
    vector<double> upper_bound = {2, 2};
    auto result = optimize(my_function, lower_bound, upper_bound, 100, 1e-5);


    cout << "Optimizing a simple function..." << endl;
    for(auto r : result){
        cout << r << " ";
    }
    cout << endl;

    cout << "Result: " << my_function(result) << endl;
    return 0;
}