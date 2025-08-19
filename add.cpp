//write program to add and sub two numbers
#include <iostream>
using namespace std;

int main() {
    int num1, num2;
    cout << "Enter two numbers: ";
    cin >> num1 >> num2;

    int sum = num1 + num2;
    int difference = num1 - num2;

    cout << "Sum: " << sum << endl;
    cout << "Difference: " << difference << endl;
    // multiply
    int product = num1 * num2;
    cout << "Product: " << product << endl;

    return 0;
}