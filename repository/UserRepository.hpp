#pragma once
#include "../database/Database.hpp"
#include "../model/UserMode.hpp"
#include <vector>

class UserRepository
{
private:
    Database &db;

public:
    UserRepository(Database &database) : db(database) {}

    // 新增：检查用户是否存在
    bool existsByUsername(const std::string &username)
    {
        if (username.empty())
            return false;
        std::string sql = "SELECT COUNT(*) AS count FROM users WHERE username = '" + db.escape(username) + "'";
        Json::Value result = db.query(sql);
        if (result.size() > 0 && result[0].isMember("count"))
        {
            int count = 0;
            Json::Value countValue = result[0]["count"];
            if (countValue.isInt())
            {
                count = countValue.asInt();
            }
            else if (countValue.isString())
            {
                try
                {
                    count = std::stoi(countValue.asString());
                }
                catch (...)
                {
                    count = 0;
                }
            }
            else if (countValue.isNumeric())
            {
                count = countValue.asInt();
            }
            return count > 0;
        }
        return false;
    }

    // 新增：根据用户名查找用户
    UserModel findByUsername(const std::string &username)
    {
        UserModel user;
        if (username.empty())
            return user;
        std::string sql = "SELECT * FROM users WHERE username = '" + db.escape(username) + "'";
        Json::Value result = db.query(sql);
        LOG(LogLevel::Debug) << "findByUsername query result: " << result.toStyledString();
        if (result.size() > 0)
        {
            user = UserModel::fromJson(result[0]);
        }
        return user;
    }

    // CRUD操作
    int save(UserModel &user)
    {
        std::string sql = "INSERT INTO users (username, password_hash, role) VALUES ('" +
                          db.escape(user.getUsername()) + "', '" +
                          db.escape(user.getPasswordHash()) + "', '" +
                          db.escape(user.getRole()) + "')";
        LOG(LogLevel::Debug) << sql;
        if (db.execute(sql))
        {
            // 获取插入的ID
            int newId = db.getLastInsertId();
            user.setId(newId);
            return newId;
        }
        LOG(LogLevel::Debug) << "INSERT FAILURE";
        return -1; // 插入失败
    }
    bool update(const UserModel &user)
    {
        if (user.getId() <= 0)
            return false;
        LOG(LogLevel::Debug) << user.getRole();
        LOG(LogLevel::Debug) << db.escape(user.getRole());
        std::string sql = "UPDATE users SET role = '" + db.escape(user.getRole()) +
                          "' WHERE id = " + std::to_string(user.getId());
        LOG(LogLevel::Debug) << sql;
        return db.execute(sql);
    }
    bool updateUsername(const UserModel &user)
    {
        if (user.getId() <= 0)
            return false;
        std::string sql = "UPDATE users SET username = '" + db.escape(user.getUsername()) +
                          "' WHERE id = " + std::to_string(user.getId());
        LOG(LogLevel::Debug) << sql;
        return db.execute(sql);
    }

    bool updatePasswordHash(const UserModel &user)
    {
        if (user.getId() <= 0)
            return false;
        std::string sql = "UPDATE users SET password_hash = '" + db.escape(user.getPasswordHash()) +
                          "' WHERE id = " + std::to_string(user.getId());
        LOG(LogLevel::Debug) << sql;
        return db.execute(sql);
    }
    bool remove(int id)
    {
        if (id <= 0)

            return false;
        std::string sql = "DELETE FROM users WHERE id = " + std::to_string(id);
        LOG(LogLevel::Debug) << "Executing SQL: " << sql;
        return db.execute(sql);
    }

    UserModel findById(int id)
    {
        UserModel user;
        if (id <= 0)
            return user;
        std::string sql = "SELECT * FROM users WHERE id = " + std::to_string(id);
        Json::Value result = db.query(sql);
        if (result.size() > 0)
        {
            user = UserModel::fromJson(result[0]);
        }
        return user;
    }

    std::vector<UserModel> findAll()
    {
        std::vector<UserModel> users;
        std::string sql = "SELECT * FROM users ORDER BY username";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            users.push_back(UserModel::fromJson(item));
        }
        return users;
    }
};