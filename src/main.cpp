#include <iostream>
#include <windows.h>  // 添加Windows头文件

// 使用指针实现交换两个整数的函数
void swapWithPointers(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 使用引用实现交换两个整数的函数
void swapWithReferences(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int main() {
    // 添加这一行来设置控制台代码页为UTF-8
    SetConsoleOutputCP(65001);
    // 测试指针版本的交换函数
    int num1 = 10, num2 = 20;
    std::cout << "交换前(指针版本): num1 = " << num1 << ", num2 = " << num2 << std::endl;
    swapWithPointers(&num1, &num2);
    std::cout << "交换后(指针版本): num1 = " << num1 << ", num2 = " << num2 << std::endl;
    
    // 测试引用版本的交换函数
    int num3 = 30, num4 = 40;
    std::cout << "交换前(引用版本): num3 = " << num3 << ", num4 = " << num4 << std::endl;
    swapWithReferences(num3, num4);
    std::cout << "交换后(引用版本): num3 = " << num3 << ", num4 = " << num4 << std::endl;
    
    return 0;
} 