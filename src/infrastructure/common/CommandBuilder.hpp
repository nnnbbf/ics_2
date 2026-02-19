// infrastructure/common/CommandBuilder.hpp (用于INSERT/UPDATE/DELETE)
#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace Infrastructure::Common {
    
    class CommandBuilder {
    private:
        std::string command_;
        std::string table_;
        std::vector<std::pair<std::string, std::string>> setValues_;
        std::vector<std::pair<std::string, std::string>> values_;
        std::string whereCondition_;
        
    public:
        // INSERT命令
        CommandBuilder& insertInto(const std::string& table) {
            command_ = "INSERT";
            table_ = table;
            return *this;
        }
        
        CommandBuilder& value(const std::string& column, const std::string& value) {
            values_.emplace_back(column, value);
            return *this;
        }
        
        CommandBuilder& value(const std::string& column, int value) {
            values_.emplace_back(column, std::to_string(value));
            return *this;
        }
        
        // UPDATE命令
        CommandBuilder& update(const std::string& table) {
            command_ = "UPDATE";
            table_ = table;
            return *this;
        }
        
        CommandBuilder& set(const std::string& column, const std::string& value) {
            setValues_.emplace_back(column, value);
            return *this;
        }
        
        CommandBuilder& set(const std::string& column, int value) {
            setValues_.emplace_back(column, std::to_string(value));
            return *this;
        }
        
        // DELETE命令
        CommandBuilder& deleteFrom(const std::string& table) {
            command_ = "DELETE";
            table_ = table;
            return *this;
        }
        
        // WHERE条件
        CommandBuilder& where(const std::string& condition) {
            whereCondition_ = condition;
            return *this;
        }
        
        // 构建命令
        std::string build() {
            if (command_ == "INSERT") {
                return buildInsert();
            } else if (command_ == "UPDATE") {
                return buildUpdate();
            } else if (command_ == "DELETE") {
                return buildDelete();
            }
            throw std::runtime_error("Unknown command type");
        }
        
    private:
        std::string buildInsert() {
            std::string sql = "INSERT INTO " + table_ + " (";
            std::string values = " VALUES (";
            
            for (size_t i = 0; i < values_.size(); ++i) {
                if (i > 0) {
                    sql += ", ";
                    values += ", ";
                }
                sql += values_[i].first;
                values += values_[i].second;
            }
            
            sql += ")" + values + ")";
            return sql;
        }
        
        std::string buildUpdate() {
            std::string sql = "UPDATE " + table_ + " SET ";
            
            for (size_t i = 0; i < setValues_.size(); ++i) {
                if (i > 0) sql += ", ";
                sql += setValues_[i].first + " = " + setValues_[i].second;
            }
            
            if (!whereCondition_.empty()) {
                sql += " WHERE " + whereCondition_;
            }
            
            return sql;
        }
        
        std::string buildDelete() {
            std::string sql = "DELETE FROM " + table_;
            
            if (!whereCondition_.empty()) {
                sql += " WHERE " + whereCondition_;
            }
            
            return sql;
        }
    };
    
} // namespace Infrastructure::Common