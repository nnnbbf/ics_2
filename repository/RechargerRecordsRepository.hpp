#pragma once
#include "../database/Database.hpp"
#include "../model/RechargerRecordsModel.hpp"
#include <vector>

class RechargerRecordsRepository
{
private:
    Database &db;

public:
    RechargerRecordsRepository(Database& database) : db(database) {}
    
    // CRUD操作
    RechargerRecordsModel  findById(int id)
    {
        RechargerRecordsModel record;
        if (id <= 0) return record;
        
        std::string sql = "SELECT * FROM recharge_records WHERE id = " + std::to_string(id);
        Json::Value result = db.query(sql);
        if (result.size() > 0) {
            record = RechargerRecordsModel::fromJson(result[0]);
        }
        return record;
    }
    
    std::vector<RechargerRecordsModel> findAll()
    {
        std::vector<RechargerRecordsModel> records;
        std::string sql = "SELECT * FROM recharge_records ORDER BY recharge_date DESC";
        Json::Value result = db.query(sql);
        for (const auto& item : result) {
            records.push_back(RechargerRecordsModel::fromJson(item));
        }
        return records;
    }
    
    int save(RechargerRecordsModel& record)
    {
        std::string sql = "INSERT INTO recharge_records (person_id, person_name, amount, operator_name) VALUES ('" +
                          std::to_string(record.getPersonId()) + "', '" +
                          db.escape(record.getPersonName()) + "', '" +
                          std::to_string(record.getAmount()) + "', '" +
                          db.escape(record.getOperator()) + "')";
        
        if (db.execute(sql)) {
            int newId = db.getLastInsertId();
            record.setId(newId);
            return newId;
        }
        return -1;
    }
    
    bool update(const RechargerRecordsModel& record)
    {
        if (record.getId() <= 0) {
            LOG(LogLevel::Error) << "Invalid recharge record ID for update: " << record.getId();
            return false;
        }
        
        std::string sql = "UPDATE recharge_records SET person_id = '" + std::to_string(record.getPersonId()) +
                          "', person_name = '" + db.escape(record.getPersonName()) +
                          "', amount = '" + std::to_string(record.getAmount()) +
                          "', operator_name = '" + db.escape(record.getOperator()) +
                          "' WHERE id = " + std::to_string(record.getId());
        return db.execute(sql);
    }
    
    bool remove(int id)
    {
        if (id <= 0) {
            LOG(LogLevel::Error) << "Invalid recharge record ID for deletion: " << id;
            return false;
        }
        
        std::string sql = "DELETE FROM recharge_records WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }
    
    // 特定查询
    std::vector<RechargerRecordsModel> findByPersonId(int personId)
    {
        std::vector<RechargerRecordsModel> records;
        if (personId <= 0) return records;
        
        std::string sql = "SELECT * FROM recharge_records WHERE person_id = " + std::to_string(personId) + 
                          " ORDER BY recharge_date DESC";
        Json::Value result = db.query(sql);
        for (const auto& item : result) {
            records.push_back(RechargerRecordsModel::fromJson(item));
        }
        return records;
    }
    
    std::vector<RechargerRecordsModel> findByPersonName(const std::string& personName)
    {
        std::vector<RechargerRecordsModel> records;
        if (personName.empty()) return records;
        
        std::string sql = "SELECT * FROM recharge_records WHERE person_name LIKE '%" + 
                          db.escape(personName) + "%' ORDER BY recharge_date DESC";
        Json::Value result = db.query(sql);
        for (const auto& item : result) {
            records.push_back(RechargerRecordsModel::fromJson(item));
        }
        return records;
    }
    
    std::vector<RechargerRecordsModel> findByDateRange(const std::string& startDate, const std::string& endDate)
    {
        std::vector<RechargerRecordsModel> records;
        if (startDate.empty() || endDate.empty()) return records;
        
        std::string sql = "SELECT * FROM recharge_records WHERE recharge_date BETWEEN '" + 
                          db.escape(startDate) + "' AND '" + db.escape(endDate) + 
                          "' ORDER BY recharge_date DESC";
        Json::Value result = db.query(sql);
        for (const auto& item : result) {
            records.push_back(RechargerRecordsModel::fromJson(item));
        }
        return records;
    }
    
    std::vector<RechargerRecordsModel> findByOperator(const std::string& operatorName)
    {
        std::vector<RechargerRecordsModel> records;
        if (operatorName.empty()) return records;
        
        std::string sql = "SELECT * FROM recharge_records WHERE operator_name LIKE '%" + 
                          db.escape(operatorName) + "%' ORDER BY recharge_date DESC";
        Json::Value result = db.query(sql);
        for (const auto& item : result) {
            records.push_back(RechargerRecordsModel::fromJson(item));
        }
        return records;
    }
    
    // 统计查询
    double getTotalAmountByPersonId(int personId)
    {
        if (personId <= 0) return 0.0;
        
        std::string sql = "SELECT SUM(amount) AS total_amount FROM recharge_records WHERE person_id = " + 
                          std::to_string(personId);
        Json::Value result = db.query(sql);
        if (result.size() > 0 && result[0].isMember("total_amount")) {
            Json::Value totalValue = result[0]["total_amount"];
            if (totalValue.isString()) {
                try {
                    return std::stod(totalValue.asString());
                } catch (...) {
                    return 0.0;
                }
            } else if (totalValue.isNumeric()) {
                return totalValue.asDouble();
            }
        }
        return 0.0;
    }
    
    double getTotalAmountByDateRange(const std::string& startDate, const std::string& endDate)
    {
        if (startDate.empty() || endDate.empty()) return 0.0;
        
        std::string sql = "SELECT SUM(amount) AS total_amount FROM recharge_records WHERE recharge_date BETWEEN '" + 
                          db.escape(startDate) + "' AND '" + db.escape(endDate) + "'";
        Json::Value result = db.query(sql);
        if (result.size() > 0 && result[0].isMember("total_amount")) {
            Json::Value totalValue = result[0]["total_amount"];
            if (totalValue.isString()) {
                try {
                    return std::stod(totalValue.asString());
                } catch (...) {
                    return 0.0;
                }
            } else if (totalValue.isNumeric()) {
                return totalValue.asDouble();
            }
        }
        return 0.0;
    }
    
    // 分页查询
    std::vector<RechargerRecordsModel> findByPage(int page, int pageSize)
    {
        std::vector<RechargerRecordsModel> records;
        if (page <= 0 || pageSize <= 0) return records;
        
        int offset = (page - 1) * pageSize;
        std::string sql = "SELECT * FROM recharge_records ORDER BY recharge_date DESC LIMIT " + 
                          std::to_string(pageSize) + " OFFSET " + std::to_string(offset);
        
        Json::Value result = db.query(sql);
        for (const auto& item : result) {
            records.push_back(RechargerRecordsModel::fromJson(item));
        }
        return records;
    }
    
    int getTotalCount()
    {
        std::string sql = "SELECT COUNT(*) AS count FROM recharge_records";
        Json::Value result = db.query(sql);
        if (result.size() > 0 && result[0].isMember("count")) {
            Json::Value countValue = result[0]["count"];
            if (countValue.isString()) {
                try {
                    return std::stoi(countValue.asString());
                } catch (...) {
                    return 0;
                }
            } else if (countValue.isNumeric()) {
                return countValue.asInt();
            }
        }
        return 0;
    }
};