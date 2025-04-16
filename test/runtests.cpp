#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include "..\src\DividedRectangles.h"

void run_test(const std::string& test_name, bool condition) {
    if (condition) {
        std::cout << "[PASS] " << test_name << std::endl;
    } else {
        std::cout << "[FAIL] " << test_name << std::endl;
    }
}

std::vector<double> generate_random_vector(size_t size, double lower_bound, double upper_bound) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lower_bound, upper_bound);

    std::vector<double> random_vector(size);
    for (auto &val : random_vector) {
        val = dis(gen);
    }
    return random_vector;
}

double rastrigin(const std::vector<double> &x) {
    double sum = 0;
    for(auto xi:x){
        sum += (xi*xi - 10*std::cos(2*3.14159265358979323846*xi));
    }
    return sum+x.size();
}

double stybtang(const std::vector<double> &x) {
    double sum = 0;
    for(auto xi:x){
        sum += xi*xi*xi - 16*xi*xi + 5*xi;
    }
    return sum;
}

int main() {

    std::vector<double> result;
#if 0
    std::vector<double> lower_bound = {-2};
    std::vector<double> upper_bound = {2};
    test_func1(upper_bound);
    auto result = optimize(test_func1, lower_bound, upper_bound, 100, 1e-5);
    run_test("Optimize TestFunc1", std::abs(test_func1(result) - (-2.5)) < 1e-2);

    lower_bound = {-2, -2};
    upper_bound = {2, 2};
    result = optimize(test_func2, lower_bound, upper_bound, 100, 1e-5);
    run_test("Optimize TestFunc2", std::abs(test_func2(result) - 2.0) < 1e-2);

    // Test 1:
    std::vector<double> lower_bound1 = {-2};
    std::vector<double> upper_bound1 = {2};
    result = optimize(test_func1, lower_bound1, upper_bound1, 100, 1e-5);
    run_test("Optimize TestFunc1", std::abs(test_func1(result) - (-2.5)) < 1e-2);

    // Test 2:
    std::vector<double> lower_bound2 = {-2, -2};
    std::vector<double> upper_bound2 = {2, 2};
    result = optimize(test_func2, lower_bound2, upper_bound2, 100, 1e-5);
    run_test("Optimize TestFunc2", std::abs(test_func2(result) - 2.0) < 1e-2);

    // Test 3:
    std::vector<double> lower_bound3 = {-2, -2, -2};
    std::vector<double> upper_bound3 = {2, 2, 2};
    result = optimize(test_func3, lower_bound3, upper_bound3, 100, 1e-5);
    run_test("Optimize TestFunc3", std::abs(test_func3(result) - 3.0) < 1e-2); // Adjust expected value as needed

    // Test 4:
    std::vector<double> lower_bound4 = {-2, -2, -2, -2};
    std::vector<double> upper_bound4 = {2, 2, 2, 2};
    result = optimize(test_func4, lower_bound4, upper_bound4, 100, 1e-5);
    run_test("Optimize TestFunc4", std::abs(test_func4(result) - 4.0) < 1e-2); // Adjust expected value as needed

    // Test 5:
    std::vector<double> lower_bound5 = {-3, -3, -3, -3, -3};
    std::vector<double> upper_bound5 = {3, 3, 3, 3, 3};
    result = optimize(test_func5, lower_bound5, upper_bound5, 100, 1e-5);
    run_test("Optimize TestFunc5", std::abs(test_func5(result) - 5.0) < 1e-2); // Adjust expected value as needed

    // Test 6:
    std::vector<double> lower_bound6 = {-1, -1, -1, -1, -1, -1};
    std::vector<double> upper_bound6 = {3, 3, 3, 3, 3, 3};
    result = optimize(test_func6, lower_bound6, upper_bound6, 100, 1e-5);
    run_test("Optimize TestFunc6", std::abs(test_func6(result) + 6.0) < 1e-1); // Adjust expected value as needed





    // Test 7:
    std::vector<double> lower_bound7(2,-10);
    std::vector<double> upper_bound7(2,20);
    result = optimize(rastrigin, lower_bound7, upper_bound7, 10, 1e-5);
    
    for(auto r: result)
    {
        std::cout << std::fixed << std::setprecision(10) << r << " ";
    }
    std::cout << std::endl;
    std::cout << " bset val "<< rastrigin(result) << std::endl;
    run_test("Optimize rastrigin", std::abs(rastrigin(result) - 0.0) < 1e-2); // Adjust expected value as needed
    // rastrigin is not good   too sharp

#endif

    // Test function stybtang:
#if 0
    int i = 1;
    while(i < 10){
        auto s1 = generate_random_vector(i, -1, 1);

        // std::cout<<"print(stybtang(np.array([";
        // for(auto s:s1){
        //     std::cout << std::fixed << std::setprecision(32) << s <<", ";
        // }
        // std::cout << "])))" << std::endl;
        

        std::cout << "stybtang(s1) = " << stybtang(s1) << std::endl;
        std::cout << std::endl;
    
        i++;

    }
#endif
    // Test 8:
    std::vector<double> lower_bound8(2,1);
    std::vector<double> upper_bound8(2,100);
    // int max_iter = 1;

    for (auto i = 0; i < 10; ++i)
    {
        int maxiter = 1 << i;
        result = optimize(stybtang, lower_bound8, upper_bound8, maxiter, 1e-5);
    
        
    
        
        std::cout << "maxiter: " << maxiter << std::endl; 
        // run_test("Optimize stybtang", std::abs(stybtang(result) - 0.0) < 1e-2); // Adjust expected value as needed
    
        std::cout << "c++ DiRect best result: [";
        for(auto r: result)
        {
            std::cout << r << " ";
        }
        std::cout <<"]"<< std::endl;
        std::cout << "c++ DiRect best value: "<< stybtang(result) << std::endl;
        std::cout << std::endl;

    }


    return 0;
}



