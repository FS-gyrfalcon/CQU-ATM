#ifndef ATM_UI_H
#define ATM_UI_H

#include "simple_json.h"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <string>
#include <vector>
#include <functional>
#include <memory>

using namespace ftxui;

class ATMWithFTXUI {
private:
    SimpleJson userData;
    std::string currentAccount;
    bool isLoggedIn;
    int loginAttempts;
    const double INITIAL_BALANCE = 10000.0;
    const double DAILY_WITHDRAWAL_LIMIT = 5000.0;
    const double SINGLE_WITHDRAWAL_LIMIT = 2000.0;

    // FTXUI组件状态
    std::string accountInput;
    std::string passwordInput;
    std::string message;
    std::string balanceMessage;
    std::string withdrawAmount;
    std::string transferAccount;
    std::string transferConfirmAccount;
    std::string transferAmount;
    std::string oldPassword;
    std::string newPassword;
    std::string confirmPassword;

    int selectedMenuItem;
    bool shouldExit;
    std::vector<std::string> menuItems;

public:
    ATMWithFTXUI();
    void run();

private:
    // 核心功能方法
    void loadUserData();
    void saveUserData();
    bool isAccountExists(const std::string& account);
    bool login();
    bool createNewAccount(const std::string& account);
    void handleMenuSelection(int selection);
    void handleWithdraw();
    void handleTransfer();
    void handleChangePassword();
    void ejectCard();
    bool isAllDigits(const std::string& str);
    std::string getCurrentTime();

    // UI组件创建方法
    Component createLoginComponent();
    Component createMainMenuComponent();
    Component createBalanceComponent();
    Component createWithdrawComponent();
    Component createTransferComponent();
    Component createChangePasswordComponent();
    Component createAppComponent();

    // UI辅助方法
    Element largeText(const std::string& content);
    Element titleText(const std::string& content);
    Element highlightText(const std::string& content);
    Component largeButton(const std::string& label, std::function<void()> on_click);
    Component largeInput(std::string* content, const std::string& placeholder);
    Element card(Element content);
    Element infoPanel(const std::string& title, const std::vector<std::string>& items);
};

#endif // ATM_UI_H
