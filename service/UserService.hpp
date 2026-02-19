#pragma once
#include <crypt.h>
#include "../repository/UserRepository.hpp"
#include "../model/UserMode.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <vector>
#include <map>
const std::string DEFAULT_RESET_PASSWORD = "jzy123456";
class UserService
{
private:
    UserRepository repository;
    // 静态map用于跟踪登录失败次数
    static std::map<std::string, int> loginAttempts;
    // 最大登录失败次数
    static const int MAX_LOGIN_ATTEMPTS = 5;

public:
    UserService() : repository(Database::getInstance()) {}
    // 查询
    UserModel getUserById(int id)
    {
        return repository.findById(id);
    }
    UserModel getUserByUsername(const std::string &username)
    {
        return repository.findByUsername(username);
    }
    std::vector<UserModel> getAllUsers()
    {
        return repository.findAll();
    }
    // 业务操作
    UserModel createUser(const UserModel &user)
    {
        UserModel newUser = user;
        if (repository.existsByUsername(user.getUsername()))
        {
            // 用户名已存在，返回空对象或抛异常
            LOG(LogLevel::Error) << "Username already exists: " << user.getUsername();
            return UserModel(0);//用户名已经存在
        }
        //  检查密码复杂度
        if (!isPasswordComplex(newUser.getPassword()))
        {
            LOG(LogLevel::Error) << "Password complexity check failed for user: " << user.getUsername();
            return UserModel(-1);//密码复杂度不够
        }

        // 生成密码哈希和盐值
        newUser.setPasswordHash(hashPassword(newUser.getPassword()));
        newUser.setPassword(""); // 清除明文密码

        if (repository.save(newUser) == -1)
        {
            LOG(LogLevel::Error) << "Failed to save new user: " << user.getUsername();
            return UserModel(-2);//创建用户失败
        }
        return newUser;
    }
    bool updateUser(const UserModel &user)
    {
        LOG(LogLevel::Debug) << user.getRole();
        return repository.update(user);
    }
    bool deleteUser(int id)
    {
        return repository.remove(id);
    }
    // 认证
    UserModel authenticate(const std::string &username, const std::string &password)
    {
        // 检查登录失败次数
        if (loginAttempts[username] >= MAX_LOGIN_ATTEMPTS)
        {
            LOG(LogLevel::Warning) << "User account locked due to too many login attempts: " << username;
            return UserModel(-3); // 用户被锁定
        }

        UserModel user = repository.findByUsername(username);
        LOG(LogLevel::Debug) << "Stored password hash: " << user.getPasswordHash();
        LOG(LogLevel::Debug) << "Provided password: " << user.getPassword();
        if (user.getId() != 0 && verifyPassword(password, user.getPasswordHash()))
        {
            // 登录成功，重置失败次数
            loginAttempts[username] = 0;
            LOG(LogLevel::Info) << "User authenticated successfully: " << username;
            return user;
        }

        // 登录失败，增加失败次数
        loginAttempts[username]++;
        LOG(LogLevel::Warning) << "Failed login attempt for user: " << username
                               << " (attempt " << loginAttempts[username] << "/" << MAX_LOGIN_ATTEMPTS << ")";

        return UserModel(-4); // 返回-4表示登录失败
    }

    // 检查用户名是否存在
    bool isUsernameExists(const std::string &username)
    {
        return repository.existsByUsername(username);
    }

    // 密码复杂度检查
    bool isPasswordComplex(const std::string &password)
    {
        LOG(LogLevel::Debug) << "Checking password: " << password;
        // 检查密码长度（至少8个字符）
        if (password.length() < 1)
        {
            LOG(LogLevel::Debug) << "Password too short: " << password.length() << " characters.";
            return false;
        }

        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;

        // 检查字符类型
        for (char c : password)
        {
            if (isupper(c))
                hasUpper = true;
            if (islower(c))
                hasLower = true;
            if (isdigit(c))
                hasDigit = true;
            if (!isalnum(c))
                hasSpecial = true;
        }

        // 至少需要满足三个条件
        int conditionsMet = 0;
        if (hasUpper)
            conditionsMet++;
        if (hasLower)
            conditionsMet++;
        if (hasDigit)
            conditionsMet++;
        if (hasSpecial)
            conditionsMet++;
        LOG(LogLevel::Debug) << "Password complexity check: "
                             << "Upper=" << hasUpper << ", Lower=" << hasLower
                             << ", Digit=" << hasDigit << ", Special=" << hasSpecial;
        return conditionsMet >= 0;
    }

    // 密码哈希 - 使用bcrypt算法
    std::string hashPassword(const std::string &password)
    {
        return UserModel::hashPassword(password);
    }
    // 验证密码
    bool verifyPassword(const std::string &password, const std::string &password_hash)
    {
        // bcrypt哈希中已经包含了盐值，所以我们可以直接使用它进行验证
        struct crypt_data data;
        data.initialized = 0;
        char *result = crypt_r(password.c_str(), password_hash.c_str(), &data);

        if (result)
        {
            return std::string(result) == password_hash;
        }
        else
        {
            return false;
        }
    }

    // 修改密码
    bool changePassword(int id, const std::string &oldPassword, const std::string &newPassword)
    {

        UserModel currentUser = repository.findById(id);
        if (currentUser.getId() == 0)
        {
            return false;
        }

        // 验证旧密码
        if (!verifyPassword(oldPassword, currentUser.getPasswordHash()))
        {
            return false;
        }

        // 检查新密码复杂度
        if (!isPasswordComplex(newPassword))
        {
            return false;
        }

        // 生成新密码的哈希值
        std::string newPasswordHash = hashPassword(newPassword);
        if (newPasswordHash.empty())
        {
            return false;
        }

        // 更新用户密码哈希
        currentUser.setPasswordHash(newPasswordHash);
        return repository.updatePasswordHash(currentUser);
    }

    // 重置密码（管理员操作）
    bool resetPassword(int id, const std::string &newPassword = DEFAULT_RESET_PASSWORD)
    {
        UserModel user = repository.findById(id);
        if (user.getId() == 0)
        {
            return false;
        }   
        // 检查新密码复杂度
        if (!isPasswordComplex(newPassword))
        {
            return false;
        }   
        // 生成新密码的哈希值
        std::string newPasswordHash = hashPassword(newPassword);
        if (newPasswordHash.empty())
        {
            return false;
        }
        // 更新用户密码哈希
        user.setPasswordHash(newPasswordHash);
        return repository.updatePasswordHash(user);
    }

    // 修改用户名
    bool changeUsername(int id, const std::string &newUsername)
    {
        UserModel user = repository.findById(id);
        if (user.getId() == 0)
        {
            return false;
        }
        // 检查新用户名是否已存在
        if (repository.existsByUsername(newUsername))
        {
            return false;
        }
        // 更新用户名
        user.setUsername(newUsername);
        return repository.updateUsername(user);
    }

    // 获取当前用户（需要根据实际情况实现）
    UserModel getCurrentUser()
    {
        
        // 这里需要根据实际情况实现，比如从会话或令牌中获取当前用户
        // 暂时返回一个空用户作为示例
        return UserModel();
    }
};

// 定义静态成员
std::map<std::string, int> UserService::loginAttempts;
const int UserService::MAX_LOGIN_ATTEMPTS;