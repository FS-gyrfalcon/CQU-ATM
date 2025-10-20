#include "atm_ui.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <ctime>

ATMWithFTXUI::ATMWithFTXUI() : 
    isLoggedIn(false), 
    loginAttempts(0),
    accountInput(""),
    passwordInput(""),
    message("WELCOME ATM！"),
    balanceMessage(""),
    withdrawAmount(""),
    transferAccount(""),
    transferConfirmAccount(""),
    transferAmount(""),
    oldPassword(""),
    newPassword(""),
    confirmPassword(""),
    selectedMenuItem(0),
    shouldExit(false) {
    
    menuItems = {"💰 余额查询", "💵 取款服务", "🔀 转账服务", "🔑 修改密码", "🚪 退卡", "❌ Ctrl+C以退出系统"};
    loadUserData();
}

void ATMWithFTXUI::loadUserData() {
    if (!userData.loadFromFile("users.json")) {
        message = "用户数据文件不存在，将创建新文件。";
    }
}

void ATMWithFTXUI::saveUserData() {
    userData.saveToFile("users.json");
}

bool ATMWithFTXUI::isAccountExists(const std::string& account) {
    return userData.hasKey(account + "_password");
}

std::string ATMWithFTXUI::getCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", ltm);
    return std::string(buffer);
}

// UI辅助方法实现
Element ATMWithFTXUI::largeText(const std::string& content) {
    return text(content) | bold | center | size(WIDTH, GREATER_THAN, 20);
}

Element ATMWithFTXUI::titleText(const std::string& content) {
    return text(content) | bold | center | size(HEIGHT, EQUAL, 3);
}

Element ATMWithFTXUI::highlightText(const std::string& content) {
    return text(content) | bold;
}

Component ATMWithFTXUI::largeButton(const std::string& label, std::function<void()> on_click) {
    return Button(label, on_click) | 
           size(WIDTH, GREATER_THAN, 20) | 
           size(HEIGHT, EQUAL, 3);
}

Component ATMWithFTXUI::largeInput(std::string* content, const std::string& placeholder) {
    return Input(content, placeholder) | 
           size(WIDTH, GREATER_THAN, 30) | 
           size(HEIGHT, EQUAL, 3);
}

Element ATMWithFTXUI::card(Element content) {
    return content | borderRounded | 
           size(WIDTH, GREATER_THAN, 70) | size(HEIGHT, GREATER_THAN, 15);
}

Element ATMWithFTXUI::infoPanel(const std::string& title, const std::vector<std::string>& items) {
    std::vector<Element> itemElements;
    for (const auto& item : items) {
        itemElements.push_back(text(" • " + item));
    }
    
    return vbox({
        text(title) | bold | center,
        separator(),
        vbox(itemElements)
    }) | borderRounded;
}

// UI组件创建方法实现
Component ATMWithFTXUI::createLoginComponent() {
    auto accountInputComponent = largeInput(&accountInput, "请输入账号");
    auto passwordInputComponent = largeInput(&passwordInput, "请输入密码");
    
    auto loginButton = largeButton("🔐 登录", [this] { 
        if (login()) {
            message = "登录成功！";
        }
    });
    
    auto newAccountButton = largeButton("📝 创建新账户", [this] { 
        if (accountInput.empty()) {
            message = "账号不能为空！";
            return;
        }
        
        if (isAccountExists(accountInput)) {
            message = "账户 " + accountInput + " 已存在！请直接登录或使用其他账号。";
            return;
        }
        
        if (createNewAccount(accountInput)) {
            message = "账户创建成功！";
        }
    });

    auto exitButton = largeButton("❌ Ctrl+C以退出系统", [this] {
        shouldExit = true;
    });

    auto container = Container::Vertical({
        accountInputComponent,
        passwordInputComponent,
        Container::Horizontal({
            loginButton,
            newAccountButton
        }),
        exitButton
    });

    return Renderer(container, [=] {
        std::vector<std::string> infoItems = {
            "单笔取款限额: " + std::to_string((int)SINGLE_WITHDRAWAL_LIMIT) + " 元",
            "单日取款限额: " + std::to_string((int)DAILY_WITHDRAWAL_LIMIT) + " 元",
            "初始账户余额: " + std::to_string((int)INITIAL_BALANCE) + " 元",
            "密码要求: 6位数字"
        };
        
        auto infoPanelElement = infoPanel("系统信息", infoItems);
        
        return vbox({
            titleText("🏦 WELCOME！"),
            text("当前时间: " + getCurrentTime()) | center,
            separator(),
            hbox({
                vbox({
                    text("用户登录") | bold | center,
                    separator(),
                    vbox({
                        hbox(text("📧 账号: "), accountInputComponent->Render()) | flex,
                        hbox(text("🔒 密码: "), passwordInputComponent->Render()) | flex,
                    }) | borderRounded,
                    separator(),
                    hbox({
                        loginButton->Render() | flex,
                        newAccountButton->Render() | flex,
                    }),
                    separator(),
                    exitButton->Render() | center,
                }) | flex,
                separator(),
                infoPanelElement | flex,
            }) | flex,
            separator(),
            (message.find("错误") != std::string::npos || message.find("失败") != std::string::npos) ? 
                text(message) | bold | center | size(HEIGHT, EQUAL, 2) :
                text(message) | center | size(HEIGHT, EQUAL, 2),
            filler()
        }) | borderDouble | 
           size(WIDTH, GREATER_THAN, 100) | size(HEIGHT, GREATER_THAN, 35);
    });
}

Component ATMWithFTXUI::createMainMenuComponent() {
    std::vector<Component> menuButtons;
    
    for (int i = 0; i < menuItems.size(); i++) {
        menuButtons.push_back(
            largeButton(menuItems[i], [this, i] { 
                selectedMenuItem = i;
                handleMenuSelection(i);
            })
        );
    }

    auto menuContainer = Container::Vertical(menuButtons);
    
    return Renderer(menuContainer, [=] {
        std::vector<Element> menuElements;
        for (int i = 0; i < menuButtons.size(); i++) {
            menuElements.push_back(menuButtons[i]->Render() | size(HEIGHT, EQUAL, 4));
        }
        
        double balance = std::stod(userData.get(currentAccount + "_balance"));
        double dailyWithdrawal = std::stod(userData.get(currentAccount + "_daily_withdrawal"));
        
        std::vector<std::string> accountInfo = {
            "账户号码: " + currentAccount,
            "当前余额: " + std::to_string((int)balance) + " 元",
            "今日已取款: " + std::to_string((int)dailyWithdrawal) + " 元",
            "剩余可取: " + std::to_string((int)(DAILY_WITHDRAWAL_LIMIT - dailyWithdrawal)) + " 元"
        };
        
        auto accountInfoPanel = infoPanel("账户信息", accountInfo);
        
        return vbox({
            titleText("🏦 ATM主菜单"),
            text("欢迎您，" + currentAccount) | center,
            text("当前时间: " + getCurrentTime()) | center,
            separator(),
            hbox({
                vbox(menuElements) | flex,
                separator(),
                accountInfoPanel | flex
            }) | flex,
            separator(),
            (message.find("错误") != std::string::npos || message.find("失败") != std::string::npos) ? 
                text(message) | bold | center | size(HEIGHT, EQUAL, 2) :
                text(message) | center | size(HEIGHT, EQUAL, 2),
            filler()
        }) | borderDouble | 
           size(WIDTH, GREATER_THAN, 100) | size(HEIGHT, GREATER_THAN, 35);
    });
}

Component ATMWithFTXUI::createBalanceComponent() {
    auto backButton = largeButton("🔙 返回主菜单", [this] { 
        selectedMenuItem = 1;
        message = "返回主菜单";
    });
    
    return Renderer(backButton, [=] {
        double balance = std::stod(userData.get(currentAccount + "_balance"));
        
        auto balanceCard = vbox({
            text("💰 账户余额") | bold | center,
            separator(),
            text("¥ " + std::to_string((int)balance)) | 
                bold | 
                center | 
                size(HEIGHT, EQUAL, 5) |
                size(WIDTH, EQUAL, 30),
            separator(),
            text("账户号码: " + currentAccount) | center,
            text("查询时间: " + getCurrentTime()) | center
        }) | borderDouble | center;
        
        return vbox({
            titleText("💰 余额查询"),
            separator(),
            balanceCard,
            separator(),
            backButton->Render() | center,
            filler()
        }) | borderDouble | 
           size(WIDTH, GREATER_THAN, 100) | size(HEIGHT, GREATER_THAN, 35);
    });
}

Component ATMWithFTXUI::createWithdrawComponent() {
    auto amountInput = largeInput(&withdrawAmount, "输入取款金额");
    auto withdrawButton = largeButton("💵 确认取款", [this] { 
        handleWithdraw();
    });
    auto backButton = largeButton("🔙 返回主菜单", [this] { 
        selectedMenuItem = 1;
        message = "返回主菜单";
        withdrawAmount = "";
    });

    auto container = Container::Vertical({
        amountInput,
        Container::Horizontal({
            withdrawButton,
            backButton
        })
    });

    return Renderer(container, [=] {
        double balance = std::stod(userData.get(currentAccount + "_balance"));
        double dailyWithdrawal = std::stod(userData.get(currentAccount + "_daily_withdrawal"));
        
        std::vector<std::string> limitInfo = {
            "当前余额: " + std::to_string((int)balance) + " 元",
            "今日已取: " + std::to_string((int)dailyWithdrawal) + " 元",
            "单笔限额: " + std::to_string((int)SINGLE_WITHDRAWAL_LIMIT) + " 元",
            "单日限额: " + std::to_string((int)DAILY_WITHDRAWAL_LIMIT) + " 元",
            "剩余可取: " + std::to_string((int)(DAILY_WITHDRAWAL_LIMIT - dailyWithdrawal)) + " 元"
        };
        
        auto limitPanel = infoPanel("💵 取款限额", limitInfo);
        
        return vbox({
            titleText("💵 取款服务"),
            separator(),
            hbox({
                vbox({
                    text("请输入取款金额") | bold | center,
                    separator(),
                    hbox(text("💰 金额: "), amountInput->Render()) | center,
                    separator(),
                    hbox({
                        withdrawButton->Render() | flex,
                        backButton->Render() | flex,
                    }),
                }) | flex,
                separator(),
                limitPanel | flex
            }) | flex,
            separator(),
            (message.find("错误") != std::string::npos || message.find("失败") != std::string::npos) ? 
                text(message) | bold | center | size(HEIGHT, EQUAL, 2) :
                text(message) | center | size(HEIGHT, EQUAL, 2),
            filler()
        }) | borderDouble | 
           size(WIDTH, GREATER_THAN, 100) | size(HEIGHT, GREATER_THAN, 35);
    });
}

Component ATMWithFTXUI::createTransferComponent() {
    auto accountInput = largeInput(&transferAccount, "输入对方账号");
    auto confirmInput = largeInput(&transferConfirmAccount, "再次输入对方账号");
    auto amountInput = largeInput(&transferAmount, "输入转账金额");
    auto transferButton = largeButton("🔀 确认转账", [this] { 
        handleTransfer();
    });
    auto backButton = largeButton("🔙 返回主菜单", [this] { 
        selectedMenuItem = 1;
        message = "返回主菜单";
        transferAccount = "";
        transferConfirmAccount = "";
        transferAmount = "";
    });

    auto container = Container::Vertical({
        accountInput,
        confirmInput,
        amountInput,
        Container::Horizontal({
            transferButton,
            backButton
        })
    });

    return Renderer(container, [=] {
        double balance = std::stod(userData.get(currentAccount + "_balance"));
        
        std::vector<std::string> transferInfo = {
            "当前余额: " + std::to_string((int)balance) + " 元",
            "自己选择，自己承担！",
            "转账前请仔细核对信息",
            "转账操作不可撤销"
        };
        
        auto infoPanelElement = infoPanel("💡 转账说明", transferInfo);
        
        return vbox({
            titleText("🔀 转账服务"),
            separator(),
            hbox({
                vbox({
                    text("请输入转账信息") | bold | center,
                    separator(),
                    vbox({
                        hbox(text("👤 对方账号: "), accountInput->Render()),
                        hbox(text("✅ 确认账号: "), confirmInput->Render()),
                        hbox(text("💰 转账金额: "), amountInput->Render()),
                    }),
                    separator(),
                    hbox({
                        transferButton->Render() | flex,
                        backButton->Render() | flex,
                    }),
                }) | flex,
                separator(),
                infoPanelElement | flex
            }) | flex,
            separator(),
            (message.find("错误") != std::string::npos || message.find("失败") != std::string::npos) ? 
                text(message) | bold | center | size(HEIGHT, EQUAL, 2) :
                text(message) | center | size(HEIGHT, EQUAL, 2),
            filler()
        }) | borderDouble | 
           size(WIDTH, GREATER_THAN, 100) | size(HEIGHT, GREATER_THAN, 35);
    });
}

Component ATMWithFTXUI::createChangePasswordComponent() {
    auto oldInput = largeInput(&oldPassword, "输入旧密码");
    auto newInput = largeInput(&newPassword, "输入新密码");
    auto confirmInput = largeInput(&confirmPassword, "确认新密码");
    auto changeButton = largeButton("🔑 确认修改", [this] { 
        handleChangePassword();
    });
    auto backButton = largeButton("🔙 返回主菜单", [this] { 
        selectedMenuItem = 1;
        message = "返回主菜单";
        oldPassword = "";
        newPassword = "";
        confirmPassword = "";
    });

    auto container = Container::Vertical({
        oldInput,
        newInput,
        confirmInput,
        Container::Horizontal({
            changeButton,
            backButton
        })
    });

    return Renderer(container, [=] {
        std::vector<std::string> passwordInfo = {
            "密码必须为6位数字",
            "不要使用简单密码",
            "不要使用生日等个人信息",
            "定期更换密码更安全"
        };
        
        auto infoPanelElement = infoPanel("🔒 密码要求", passwordInfo);
        
        return vbox({
            titleText("🔑 修改密码"),
            separator(),
            hbox({
                vbox({
                    text("请输入密码信息") | bold | center,
                    separator(),
                    vbox({
                        hbox(text("🔒 旧密码: "), oldInput->Render()),
                        hbox(text("🆕 新密码: "), newInput->Render()),
                        hbox(text("✅ 确认密码: "), confirmInput->Render()),
                    }),
                    separator(),
                    hbox({
                        changeButton->Render() | flex,
                        backButton->Render() | flex,
                    }),
                }) | flex,
                separator(),
                infoPanelElement | flex
            }) | flex,
            separator(),
            (message.find("错误") != std::string::npos || message.find("失败") != std::string::npos) ? 
                text(message) | bold | center | size(HEIGHT, EQUAL, 2) :
                text(message) | center | size(HEIGHT, EQUAL, 2),
            filler()
        }) | borderDouble | 
           size(WIDTH, GREATER_THAN, 100) | size(HEIGHT, GREATER_THAN, 35);
    });
}

Component ATMWithFTXUI::createAppComponent() {
    auto loginComponent = createLoginComponent();
    auto mainMenuComponent = createMainMenuComponent();
    auto balanceComponent = createBalanceComponent();
    auto withdrawComponent = createWithdrawComponent();
    auto transferComponent = createTransferComponent();
    auto passwordComponent = createChangePasswordComponent();

    return Container::Tab({
        loginComponent,        // 0 - 登录界面
        mainMenuComponent,     // 1 - 主菜单
        balanceComponent,      // 2 - 余额查询
        withdrawComponent,     // 3 - 取款
        transferComponent,     // 4 - 转账
        passwordComponent      // 5 - 修改密码
    }, &selectedMenuItem);
}

// 核心业务逻辑方法实现
bool ATMWithFTXUI::login() {
    if (accountInput.empty()) {
        message = "❌ 账号不能为空！";
        return false;
    }

    if (!isAccountExists(accountInput)) {
        message = "❌ 账号不存在！请先创建账户。";
        return false;
    }

    if (userData.get(accountInput + "_locked") == "true") {
        message = "❌ 账户已被锁定，请联系银行客服！";
        return false;
    }

    if (userData.get(accountInput + "_password") == passwordInput) {
        currentAccount = accountInput;
        isLoggedIn = true;
        loginAttempts = 0;
        selectedMenuItem = 1;
        passwordInput = "";
        return true;
    } else {
        loginAttempts++;
        if (loginAttempts >= 3) {
            userData.set(accountInput + "_locked", "true");
            saveUserData();
            message = "❌ 密码错误3次，账户已被锁定！";
        } else {
            message = "❌ 密码错误，还剩 " + std::to_string(3 - loginAttempts) + " 次尝试机会";
        }
        return false;
    }
}

bool ATMWithFTXUI::createNewAccount(const std::string& account) {
    if (isAccountExists(account)) {
        message = "❌ 账户 " + account + " 已存在！创建失败。";
        return false;
    }

    if (passwordInput.length() != 6 || !isAllDigits(passwordInput)) {
        message = "❌ 密码必须是6位数字！";
        return false;
    }

    userData.set(account + "_password", passwordInput);
    userData.set(account + "_balance", std::to_string(INITIAL_BALANCE));
    userData.set(account + "_daily_withdrawal", "0");
    userData.set(account + "_locked", "false");
    saveUserData();

    currentAccount = account;
    isLoggedIn = true;
    selectedMenuItem = 1;
    passwordInput = "";
    
    message = "✅ 账户创建成功！初始余额: " + std::to_string((int)INITIAL_BALANCE) + " 元";
    return true;
}

void ATMWithFTXUI::handleMenuSelection(int selection) {
    switch (selection) {
        case 0: selectedMenuItem = 2; break;
        case 1: selectedMenuItem = 3; break;
        case 2: selectedMenuItem = 4; break;
        case 3: selectedMenuItem = 5; break;
        case 4: ejectCard(); break;
        case 5: shouldExit = true; break;
    }
}

void ATMWithFTXUI::handleWithdraw() {
    if (withdrawAmount.empty()) {
        message = "❌ 取款金额不能为空！";
        return;
    }

    double amount;
    try {
        amount = std::stod(withdrawAmount);
    } catch (...) {
        message = "❌ 请输入有效的金额！";
        return;
    }

    double balance = std::stod(userData.get(currentAccount + "_balance"));
    double dailyWithdrawal = std::stod(userData.get(currentAccount + "_daily_withdrawal"));

    if (amount <= 0) {
        message = "❌ 取款金额必须大于0！";
        return;
    }

    if (std::fmod(amount, 100) != 0) {
        message = "❌ 取款金额必须是100的整数倍！";
        return;
    }

    if (amount > SINGLE_WITHDRAWAL_LIMIT) {
        message = "❌ 单笔取款金额不能超过 " + std::to_string((int)SINGLE_WITHDRAWAL_LIMIT) + " 元！";
        return;
    }

    if (amount > balance) {
        message = "❌ 余额不足！";
        return;
    }

    if (dailyWithdrawal + amount > DAILY_WITHDRAWAL_LIMIT) {
        message = "❌ 超过单日取款限额！";
        return;
    }

    balance -= amount;
    dailyWithdrawal += amount;

    userData.set(currentAccount + "_balance", std::to_string(balance));
    userData.set(currentAccount + "_daily_withdrawal", std::to_string(dailyWithdrawal));
    saveUserData();

    message = "✅ 取款成功！取款金额: " + std::to_string((int)amount) + " 元";
    withdrawAmount = "";
}

void ATMWithFTXUI::handleTransfer() {
    if (transferAccount.empty() || transferConfirmAccount.empty() || transferAmount.empty()) {
        message = "❌ 请填写完整信息！";
        return;
    }

    if (transferAccount != transferConfirmAccount) {
        message = "❌ 两次输入的账户不一致！";
        return;
    }

    if (!isAccountExists(transferAccount)) {
        message = "❌ 转入账户不存在！";
        return;
    }

    if (transferAccount == currentAccount) {
        message = "❌ 不能转账给自己！";
        return;
    }

    double amount;
    try {
        amount = std::stod(transferAmount);
    } catch (...) {
        message = "❌ 请输入有效的金额！";
        return;
    }

    double balance = std::stod(userData.get(currentAccount + "_balance"));

    if (amount <= 0) {
        message = "❌ 转账金额必须大于0！";
        return;
    }

    if (amount > balance) {
        message = "❌ 余额不足！";
        return;
    }

    double targetBalance = std::stod(userData.get(transferAccount + "_balance"));
    
    balance -= amount;
    targetBalance += amount;

    userData.set(currentAccount + "_balance", std::to_string(balance));
    userData.set(transferAccount + "_balance", std::to_string(targetBalance));
    saveUserData();

    message = "✅ 转账成功！转账金额: " + std::to_string((int)amount) + " 元";
    transferAccount = "";
    transferConfirmAccount = "";
    transferAmount = "";
}

void ATMWithFTXUI::handleChangePassword() {
    if (oldPassword.empty() || newPassword.empty() || confirmPassword.empty()) {
        message = "❌ 请填写完整信息！";
        return;
    }

    if (userData.get(currentAccount + "_password") != oldPassword) {
        message = "❌ 旧密码错误！";
        return;
    }

    if (newPassword.length() != 6 || !isAllDigits(newPassword)) {
        message = "❌ 新密码必须是6位数字！";
        return;
    }

    if (newPassword != confirmPassword) {
        message = "❌ 两次输入的新密码不一致！";
        return;
    }

    userData.set(currentAccount + "_password", newPassword);
    saveUserData();

    message = "✅ 密码修改成功！";
    oldPassword = "";
    newPassword = "";
    confirmPassword = "";
}

void ATMWithFTXUI::ejectCard() {
    currentAccount = "";
    isLoggedIn = false;
    selectedMenuItem = 0;
    message = "✅ 已退卡，请取走您的卡片";
    
    accountInput = "";
    passwordInput = "";
    withdrawAmount = "";
    transferAccount = "";
    transferConfirmAccount = "";
    transferAmount = "";
    oldPassword = "";
    newPassword = "";
    confirmPassword = "";
}

bool ATMWithFTXUI::isAllDigits(const std::string& str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

void ATMWithFTXUI::run() {
    auto screen = ScreenInteractive::Fullscreen();
    auto component = createAppComponent();
    
    selectedMenuItem = 0;
    
    while (!shouldExit) {
        screen.Loop(component);
        if (shouldExit) {
            break;
        }
    }
}
