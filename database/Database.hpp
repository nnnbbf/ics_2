// database/Database.hpp
#pragma once
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
#include "shared/util/log.hpp"
#include <string>
#include <mutex>

class Database
{
private:
    static Database *instance;
    MYSQL *mysql = nullptr;
    std::mutex dbMutex;

    Database() {} // 私有构造函数

public:
    static Database &getInstance()
    {
        static Database instance;
        return instance;
    }

    // 连接数据库
    bool connect(const std::string &host, const std::string &user,
                 const std::string &password, const std::string &db,
                 int port = 3306)
    {
        mysql = mysql_init(nullptr);
        if (!mysql)
            return false;
        if (!mysql_real_connect(mysql, host.c_str(), user.c_str(),
                                password.c_str(), db.c_str(), port, nullptr, 0))
        {
            return false;
        }
        return true;
    }

    // 执行SQL（INSERT/UPDATE/DELETE）
    bool execute(const std::string &sql)
    {
        LOG(LogLevel::Debug) << sql;
        std::lock_guard<std::mutex> lock(dbMutex);
        if (mysql_query(mysql, sql.c_str()))
        {
           LOG(LogLevel::Error) << "SQL Execute Error: " << mysql_error(mysql);
            return false;
        }
        return true;
    }

    // 查询SQL（SELECT）
    Json::Value query(const std::string &sql)
    {
        //LOG(LogLevel::Debug) << "Executing SQL: " << sql;
        std::lock_guard<std::mutex> lock(dbMutex);
        Json::Value result;
        if (mysql_query(mysql, sql.c_str()))
        {
            LOG(LogLevel::Error) << "SQL Query Error: " << mysql_error(mysql);
            return result; // 返回空结果
        }
        MYSQL_RES *res = mysql_store_result(mysql);
        if (!res)
        {
            LOG(LogLevel::Error) << "SQL Store Result Error: " << mysql_error(mysql);
            return result; // 返回空结果
        }
        MYSQL_ROW row;
        MYSQL_FIELD *fields = mysql_fetch_fields(res);
        int numFields = mysql_num_fields(res);
        while ((row = mysql_fetch_row(res)))
        {
            Json::Value record;
            for (int i = 0; i < numFields; ++i)
            {

                if (row[i])
                {
                    std::string fieldName = fields[i].name;
                    std::string value = row[i];
                    //LOG(LogLevel::Debug) << "Processing field: " << fieldName << " = " << value;
                    if(fieldName == "password_hash" || fieldName == "salt") {
                        //LOG(LogLevel::Debug) << "Skipping sensitive field: " << fieldName;
                        record[fieldName] = value;
                        continue;
                    }
                    // 特殊处理数值字段
                    if (fieldName == "count")
                    {
                        try
                        {
                            record[fieldName] = std::stoi(value);
                        }
                        catch (...)
                        {
                            record[fieldName] = 0; // 转换失败设为0
                        }
                    }
                    if (fieldName == "id" || fieldName == "balance")
                    {
                        try
                        {
                            if (fieldName == "id")
                            {
                                record[fieldName] = std::stoi(value);
                            }
                            else
                            {
                                // LOG(LogLevel::Debug) << "Converting balance value: " << value;
                                record[fieldName] = std::stod(value);
                            }
                        }
                        catch (...)
                        {
                            record[fieldName] = value; // 转换失败保持字符串
                        }
                    }
                    else
                    {
                        record[fieldName] = value;
                    }
                }
                else
                {
                    record[fields[i].name] = Json::nullValue;
                }
                //LOG(LogLevel::Debug) << "Field: " << fields[i].name << " Value: " << record[fields[i].name];
            }
            result.append(record);
        }
        // LOG(LogLevel::Debug) << "Query returned " << result.size() << " records.";
        mysql_free_result(res);
        return result;
    }

    // 获取最后插入的ID
    int getLastInsertId()
    {
        std::lock_guard<std::mutex> lock(dbMutex);
        return static_cast<int>(mysql_insert_id(mysql));
    }

    // SQL转义

    std::string escape(const std::string &str)
    {
        std::lock_guard<std::mutex> lock(dbMutex);

        // MySQL文档：转义后的字符串长度最多是原字符串的2倍 + 1
        size_t bufferSize = str.length() * 2 + 1;
        std::unique_ptr<char[]> escaped(new char[bufferSize]);

        unsigned long escapedLength = mysql_real_escape_string(mysql, escaped.get(), str.c_str(), str.length());

        if (escapedLength == static_cast<unsigned long>(-1))
        {
            // 转义失败，返回原始字符串或抛出异常
            return str;
        }

        return std::string(escaped.get(), escapedLength);
    }

    // 事务开始
    bool beginTransaction()
    {
        return execute("START TRANSACTION");
        ;
    }

    // 事务提交
    bool commit()
    {
        return execute("COMMIT");
    }

    // 回滚
    bool rollback()
    {
        try
        {
            LOG(LogLevel::Info) << "Transaction rollback initiated.";
            return execute("ROLLBACK");
        }
        catch (...)
        {

            // 忽略日志记录中的任何异常
            return false;
        }
    }

    // 关闭连接
    ~Database()
    {
        if (mysql)
            mysql_close(mysql);
    }
};