#include "atm_ui.h"
#include <iostream>

int main() {
    ATMWithFTXUI atm;
    atm.run();
    std::cout << "感谢使用ATM系统，再见！" << std::endl;
    return 0;
}