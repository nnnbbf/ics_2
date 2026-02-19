#pragma once
#include "../repository/PersonRepository.hpp"
#include "../repository/RechargerRecordsRepository.hpp"
#include "../model/RechargerRecordsModel.hpp"
#include "../database/Database.hpp"
#include "../util/log.hpp"
#include <string>
#include <vector>

class RechargerRecordsService {
private:
    Database& db;
    PersonRepository personRepo;
    RechargerRecordsRepository recordRepo;
    
public:
    RechargerRecordsService() 
        : db(Database::getInstance()), 
          personRepo(db), 
          recordRepo(db) {}
    
    // 核心充值方法
    bool recharge(int personId, double amount, const std::string& operatorName) {
        // 参数验证
        if (personId <= 0) {
            LOG(LogLevel::Error) << "Invalid person ID: " << personId;
            return false;
        }
        
        if (amount <= 0) {
            LOG(LogLevel::Error) << "Invalid recharge amount: " << amount;
            return false;
        }
        
        if (operatorName.empty()) {
            LOG(LogLevel::Error) << "Operator name is empty";
            return false;
        }
        
        LOG(LogLevel::Info) << "Starting recharge: person=" << personId 
                           << ", amount=" << amount 
                           << ", operator=" << operatorName;
        
        // 开始事务
        if (!db.beginTransaction()) {
            LOG(LogLevel::Error) << "Failed to begin transaction";
            return false;
        }
        
        try {
            // 1. 获取人员信息（带锁）
            PersonModel person = personRepo.findByIdForUpdate(personId);
            if (!person.isValid()) {
                LOG(LogLevel::Error) << "Person not found: " << personId;
                db.rollback();
                return false;
            }
            
            // 2. 检查人员状态
            if (person.getStatus() != "正常") {
                LOG(LogLevel::Error) << "Person status is not normal: " 
                                   << person.getStatus();
                db.rollback();
                return false;
            }
            
            // 3. 更新余额（原子操作）
            if (!personRepo.increaseBalance(personId, amount)) {
                LOG(LogLevel::Error) << "Failed to update balance";
                db.rollback();
                return false;
            }
            
            // 4. 创建充值记录
            RechargerRecordsModel record(
                personId,
                person.getName(),
                amount,
                "",  // 时间由数据库生成
                operatorName
            );
            
            if (recordRepo.save(record) == -1) {
                LOG(LogLevel::Error) << "Failed to save recharge record";
                db.rollback();
                return false;
            }
            
            // 5. 提交事务
            if (!db.commit()) {
                LOG(LogLevel::Error) << "Failed to commit transaction";
                db.rollback();
                return false;
            }
            
            LOG(LogLevel::Info) << "Recharge successful: person=" << personId 
                               << ", new balance=" << (person.getBalance() + amount)
                               << ", operator=" << operatorName;
            
            return true;
            
        } catch (const std::exception& e) {
            // 异常时回滚
            LOG(LogLevel::Error) << "Recharge failed with exception: " << e.what();
            db.rollback();
            return false;
        }
    }
    
    // 批量充值
    bool batchRecharge(const std::vector<int>& personIds, 
                      double amount, 
                      const std::string& operatorName) {
        if (personIds.empty() || amount <= 0 || operatorName.empty()) {
            return false;
        }
        
        bool allSuccess = true;
        
        for (int personId : personIds) {
            if (!recharge(personId, amount, operatorName)) {
                LOG(LogLevel::Error) << "Failed to recharge person: " << personId;
                allSuccess = false;
                // 继续尝试其他人，不中断整个批量操作
            }
        }
        
        return allSuccess;
    }
    
    // 带返还的撤销充值（冲正操作）
    bool reverseRecharge(int recordId, const std::string& operatorName) {
        if (recordId <= 0 || operatorName.empty()) {
            return false;
        }
        
        // 查找原充值记录
        RechargerRecordsModel record = recordRepo.findById(recordId);
        if (record.getId() == -1) {
            LOG(LogLevel::Error) << "Recharge record not found: " << recordId;
            return false;
        }
        
        LOG(LogLevel::Info) << "Reversing recharge record: " << recordId 
                           << ", person=" << record.getPersonId()
                           << ", amount=" << record.getAmount();
        
        // 开始事务
        if (!db.beginTransaction()) {
            LOG(LogLevel::Error) << "Failed to begin transaction";
            return false;
        }
        
        try {
            // 1. 获取人员信息（带锁）
            PersonModel person = personRepo.findByIdForUpdate(record.getPersonId());
            if (!person.isValid()) {
                LOG(LogLevel::Error) << "Person not found: " << record.getPersonId();
                db.rollback();
                return false;
            }
            
            // 2. 减少余额（相当于退还）
            if (!personRepo.decreaseBalance(record.getPersonId(), record.getAmount())) {
                LOG(LogLevel::Error) << "Failed to decrease balance";
                db.rollback();
                return false;
            }
            
            // 3. 创建冲正记录（负金额表示冲正）
            RechargerRecordsModel reversalRecord(
                record.getPersonId(),
                record.getPersonName(),
                -record.getAmount(),  // 负金额
                "",
                operatorName + " [冲正]"
            );
            
            if (recordRepo.save(reversalRecord) == -1) {
                LOG(LogLevel::Error) << "Failed to save reversal record";
                db.rollback();
                return false;
            }
            
            // 4. 提交事务
            if (!db.commit()) {
                LOG(LogLevel::Error) << "Failed to commit transaction";
                db.rollback();
                return false;
            }
            
            LOG(LogLevel::Info) << "Recharge reversal successful: record=" << recordId
                               << ", person=" << record.getPersonId();
            
            return true;
            
        } catch (const std::exception& e) {
            LOG(LogLevel::Error) << "Reversal failed with exception: " << e.what();
            db.rollback();
            return false;
        }
    }
    
    // 查询方法（直接调用Repository）
    RechargerRecordsModel getRecordById(int id) {
        return recordRepo.findById(id);
    }
    
    std::vector<RechargerRecordsModel> getAllRecords() {
        return recordRepo.findAll();
    }
    
    std::vector<RechargerRecordsModel> getRecordsByPersonId(int personId) {
        return recordRepo.findByPersonId(personId);
    }
    
    std::vector<RechargerRecordsModel> getRecordsByDateRange(const std::string& startDate, 
                                                           const std::string& endDate) {
        return recordRepo.findByDateRange(startDate, endDate);
    }
    
    std::vector<RechargerRecordsModel> getRecordsByOperator(const std::string& operatorName) {
        return recordRepo.findByOperator(operatorName);
    }
    
    // 统计方法
    double getTotalRechargeAmount(int personId) {
        return recordRepo.getTotalAmountByPersonId(personId);
    }
    
    double getTotalRechargeAmountByDateRange(const std::string& startDate, 
                                           const std::string& endDate) {
        return recordRepo.getTotalAmountByDateRange(startDate, endDate);
    }
    
    // 分页查询
    std::vector<RechargerRecordsModel> getRecordsByPage(int page, int pageSize) {
        return recordRepo.findByPage(page, pageSize);
    }
    
    int getTotalRecordsCount() {
        return recordRepo.getTotalCount();
    }
    
    // 创建记录（不带事务，简单插入）
    bool createRecord(RechargerRecordsModel& record) {
        if (recordRepo.save(record) == -1) {
            LOG(LogLevel::Error) << "Failed to create record";
            return false;
        }
        return true;
    }
    
    // 更新记录
    bool updateRecord(const RechargerRecordsModel& record) {
        return recordRepo.update(record);
    }
    
    // 删除记录
    bool deleteRecord(int id) {
        return recordRepo.remove(id);
    }
};