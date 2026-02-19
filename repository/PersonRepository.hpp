// repository/PersonRepository.hpp
#pragma once
#include "../database/Database.hpp"
#include "../model/PersonModel.hpp"
#include <vector>

class PersonRepository
{
private:
    Database &db;

public:
    PersonRepository(Database &database) : db(database) {}

    // CRUD操作
    PersonModel findById(int id)
    {
        PersonModel person;
        if (id <= 0)
            return person; // 验证ID有效性

        std::string sql = "SELECT * FROM persons WHERE id = " + std::to_string(id);
        Json::Value result = db.query(sql);
        if (result.size() > 0)
        {
            person = PersonModel::fromJson(result[0]);
        }
        return person;
    }

    std::vector<PersonModel> findAll()
    {
        std::vector<PersonModel> persons;
        std::string sql = "SELECT * FROM persons ORDER BY id";
        Json::Value result = db.query(sql);
        // LOG(LogLevel::Debug) << result.toStyledString();
        for (const auto &item : result)
        {
            try
            {
                // LOG(LogLevel::Debug) << "Parsing record: " << item.toStyledString();
                auto person = PersonModel::fromJson(item);
                // LOG(LogLevel::Debug) << "Parsed Person ID: " << person.getId() << " create_at: " << person.getCreateAt();
                persons.push_back(person);
            }
            catch (const std::exception &e)
            {
                LOG(LogLevel::Error) << "Error parsing record: " << e.what() << " Record data: " << item.toStyledString();
            }
        }
        return persons;
    }

    int save(PersonModel &person)
    {
        std::string sql = "INSERT INTO persons (name, card_number, balance, status, department, work_center) VALUES ('" +
                          db.escape(person.getName()) + "', '" +
                          db.escape(person.getCardNumber()) + "', '" +
                          std::to_string(person.getBalance()) + "', '" +
                          db.escape(person.getStatus()) + "', '" +
                          db.escape(person.getDepartment()) + "', '" +
                          db.escape(person.getWorkCenter()) + "')";
        // LOG(LogLevel::Debug) << "Executing SQL: " << sql;
        if (db.execute(sql))
        {
            LOG(LogLevel::Debug) << "Insert successful.";
            int newId = db.getLastInsertId();
            person.setId(newId);
            return newId;
        }
        // LOG(LogLevel::Error) << "Failed to execute SQL: " << sql;
        return -1;
    }

    bool update(const PersonModel &person)
    {
        if (person.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid person ID for update: " << person.getId();
            return false;
        }

        std::string sql = "UPDATE persons SET name = '" + db.escape(person.getName()) +
                          "', card_number = '" + db.escape(person.getCardNumber()) +
                          "', status = '" + db.escape(person.getStatus()) +
                          "', department = '" + db.escape(person.getDepartment()) +
                          "', work_center = '" + db.escape(person.getWorkCenter()) +
                          "' WHERE id = " + std::to_string(person.getId());
        return db.execute(sql);
    }

    bool remove(int id)
    {
        if (id <= 0)
        {
            LOG(LogLevel::Error) << "Invalid person ID for deletion: " << id;
            return false;
        }
        std::string sql = "DELETE FROM persons WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    // 特定查询
    PersonModel findByCardNumber(const std::string &cardNumber)
    {
        PersonModel person;
        if (cardNumber.empty())
            return person;

        std::string sql = "SELECT * FROM persons WHERE card_number = '" + db.escape(cardNumber) + "' LIMIT 1";
        Json::Value result = db.query(sql);
        if (result.size() > 0)
        {
            person = PersonModel::fromJson(result[0]);
        }
        return person;
    }

    std::vector<PersonModel> findByName(const std::string &name)
    {
        std::vector<PersonModel> persons;
        if (name.empty())
            return persons;

        std::string sql = "SELECT * FROM persons WHERE name LIKE '%" + db.escape(name) + "%' ORDER BY name";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            persons.push_back(PersonModel::fromJson(item));
        }
        return persons;
    }

    std::vector<PersonModel> findByDepartment(const std::string &department)
    {
        std::vector<PersonModel> persons;
        if (department.empty())
            return persons;

        std::string sql = "SELECT * FROM persons WHERE department = '" + db.escape(department) + "' ORDER BY name";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            persons.push_back(PersonModel::fromJson(item));
        }
        return persons;
    }

    // 业务操作
    bool updateBalance(int id, double newBalance)
    {
        if (id <= 0)
            return false;

        std::string sql = "UPDATE persons SET balance = " + std::to_string(newBalance) +
                          " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    bool updateStatus(int id, const std::string &newStatus)
    {
        if (id <= 0 || newStatus.empty())
            return false;

        std::string sql = "UPDATE persons SET status = '" + db.escape(newStatus) +
                          "' WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    bool existsByCardNumber(const std::string &cardNumber)
    {
        if (cardNumber.empty())
            return false;

        std::string sql = "SELECT COUNT(*) AS count FROM persons WHERE card_number = '" + db.escape(cardNumber) + "'";
        Json::Value result = db.query(sql);
        if (result.size() > 0 && result[0].isMember("count"))
        {
            Json::Value countValue = result[0]["count"];
            int count = 0;

            // 尝试获取值
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
        LOG(LogLevel::Debug) << "Count not found in query result.";
        return false;
    }

    // 新增：按状态查询
    std::vector<PersonModel> findByStatus(const std::string &status)
    {
        std::vector<PersonModel> persons;
        if (status.empty())
            return persons;

        std::string sql = "SELECT * FROM persons WHERE status = '" + db.escape(status) + "' ORDER BY name";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            persons.push_back(PersonModel::fromJson(item));
        }
        return persons;
    }

    // 原子增加余额
    bool increaseBalance(int id, double amount)
    {
        if (id <= 0)
            return false;

        std::string sql = "UPDATE persons SET balance = balance + " +
                          std::to_string(amount) +
                          " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    // 原子减少余额
    bool decreaseBalance(int id, double amount)
    {
        if (id <= 0 || amount <= 0)
            return false;

        // 先检查余额是否足够（在事务中使用）
        std::string checkSql = "SELECT balance FROM persons WHERE id = " +
                               std::to_string(id) + " FOR UPDATE";
        Json::Value result = db.query(checkSql);

        if (result.size() > 0)
        {
            double currentBalance = 0.0;
            if (result[0].isMember("balance"))
            {
                Json::Value balanceValue = result[0]["balance"];
                if (balanceValue.isString())
                {
                    currentBalance = std::stod(balanceValue.asString());
                }
                else if (balanceValue.isNumeric())
                {
                    currentBalance = balanceValue.asDouble();
                }
            }

            if (currentBalance >= amount)
            {
                std::string updateSql = "UPDATE persons SET balance = balance - " +
                                        std::to_string(amount) +
                                        " WHERE id = " + std::to_string(id);
                return db.execute(updateSql);
            }
        }
        return false;
    }

    // 带锁查询（用于事务）
    PersonModel findByIdForUpdate(int id)
    {
        PersonModel person;
        if (id <= 0)
            return person;

        std::string sql = "SELECT * FROM persons WHERE id = " +
                          std::to_string(id) + " FOR UPDATE";
        Json::Value result = db.query(sql);
        if (result.size() > 0)
        {
            person = PersonModel::fromJson(result[0]);
        }
        return person;
    }

    // 验证人员状态（用于事务）
    bool checkPersonStatus(int id, const std::string &expectedStatus = "正常")
    {
        if (id <= 0)
            return false;

        std::string sql = "SELECT status FROM persons WHERE id = " +
                          std::to_string(id);
        Json::Value result = db.query(sql);

        if (result.size() > 0 && result[0].isMember("status"))
        {
            std::string status = result[0]["status"].asString();
            return status == expectedStatus;
        }
        return false;
    }
};