# WELLCOME!
# 
# 本项目全程在Ubuntu环境中开发。
# 为什么？
# Windows你自己来说说为什么
# 但凡你***环境配置方便一点我也不至于上Linux
# 

# 🏦 ATM模拟银行系统

一个基于C++和FTXUI开发的现代化终端ATM模拟系统，提供完整的银行账户管理功能。

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![FTXUI](https://img.shields.io/badge/FTXUI-Terminal%20UI-green.svg)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey.svg)

## ✨ 功能特性

### 🔐 账户管理
- **账户注册**: 19位数字账号 + 实名认证
- **安全登录**: 密码错误3次自动锁定
- **身份验证**: 身份证号(18位) + 真实姓名验证
- **密码管理**: 6位数字密码，支持修改

### 💰 金融服务
- **余额查询**: 实时查看账户余额
- **现金取款**: 支持100的整数倍取款
- **转账服务**: 安全的跨账户转账
- **限额管理**: 单笔¥2000，单日¥5000取款限额

### 🎨 用户体验
- **现代化界面**: 基于FTXUI的终端图形界面
- **直观操作**: 键盘导航，按钮交互
- **实时反馈**: 清晰的操作状态提示
- **数据持久化**: JSON格式本地存储

## 🛠️ 技术栈

- **编程语言**: C++17
- **用户界面**: FTXUI (Functional Terminal User Interface)
- **数据存储**: JSON格式文件
- **构建工具**: CMake

## 📦 安装与编译

### 环境要求
- GCC 7.0+ 或 Clang 5.0+
- CMake 3.11+
- Linux 或 Windows (需要Mingw-w64)

### 编译步骤

#### Linux 环境
```bash
# 克隆项目
git clone https://github.com/yourusername/atm-simulator.git
cd atm-simulator

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make

# 运行
./atm_with_ftxui
```

#### Windows 交叉编译 (在Ubuntu中)
```bash
# 安装交叉编译工具链
sudo apt install mingw-w64

# 编译为Windows可执行文件
x86_64-w64-mingw32-g++ main.cpp atm_ui.cpp simple_json.cpp -o atm_windows.exe -lftxui-component -lftxui-dom -lftxui-screen -std=c++17
```

### 依赖安装

#### Ubuntu/Debian
```bash
sudo apt install cmake g++ libftxui-dev
```

#### 手动安装FTXUI
```bash
git clone https://github.com/ArthurSonzogni/FTXUI.git
cd FTXUI
mkdir build && cd build
cmake .. -DFTXUI_BUILD_EXAMPLES=OFF -DFTXUI_BUILD_TESTS=OFF
make
sudo make install
```

## 🚀 使用指南

### 首次使用
1. 启动程序后进入登录界面
2. 点击"注册账户"创建新账户
3. 填写完整的注册信息：
   - 19位数字账号
   - 6位数字密码
   - 18位身份证号码
   - 真实姓名(2-20字符)

### 主要操作流程
1. **登录账户**: 输入19位账号和6位密码
2. **主菜单导航**: 使用键盘选择功能
3. **金融服务**: 进行查询、取款、转账等操作
4. **安全退出**: 完成操作后选择"退卡"

### 功能限制
- 🔒 密码连续错误3次将锁定账户
- 💵 单笔取款不超过¥2000
- 📅 单日取款总额不超过¥5000
- 🔄 转账需要双重确认对方账户

## 📁 项目结构

```
atm-simulator/
├── main.cpp              # 程序入口点
├── atm_ui.h/cpp          # 用户界面和业务逻辑
├── simple_json.h/cpp     # JSON数据存储处理
├── CMakeLists.txt        # 构建配置
├── users.json           # 用户数据文件(自动生成)
└── README.md            # 项目说明文档
```

## 🎯 核心功能详解

### 账户注册系统
```cpp
// 严格的输入验证
bool isValidAccount(const std::string& account);    // 19位数字验证
bool isValidIdCard(const std::string& idCard);      // 18位身份证验证
bool isIdCardRegistered(const std::string& idCard); // 防重复注册
```

### 安全认证机制
- 🔐 密码加密存储
- 🚫 连续失败锁定
- 👤 实名制账户管理

### 交易风控系统
- 💰 余额实时检查
- 📊 取款限额控制
- 🔍 转账身份验证

## 🐛 故障排除

### 常见问题

**Q: 编译时找不到FTXUI库**
```bash
# 确保FTXUI正确安装
sudo apt install libftxui-dev
# 或手动设置库路径
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

**Q: 程序启动后立即退出**
```bash
# 检查终端尺寸
# 程序需要足够大的终端窗口(建议80x25以上)
```

**Q: 用户数据丢失**
```bash
# 检查users.json文件权限
chmod 644 users.json
```

### 日志调试
程序运行日志会显示在标准输出，包含：
- 用户操作记录
- 错误状态信息
- 系统运行状态
  
# 开源社区的恩情还不完\0/\0/\0/
# FTXUI的恩情还不完\0/\0/\0/
# Deepseek的恩情还不完\0/\0/\0/

⭐ 如果这个项目对您有帮助，请给我们一个Star！
