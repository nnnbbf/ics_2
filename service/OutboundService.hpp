#pragma once

#include "../database/Database.hpp"
#include "../repository/OutboundRepository.hpp"
#include "../repository/InventoryRepository.hpp"
#include "../repository/PersonRepository.hpp"
#include "../model/OutboundModel.hpp"
#include "../model/InventoryDetailModel.hpp"
#include "../model/PersonModel.hpp"
#include <vector>
#include <string>

class OutboundService
{
private:
    Database &db;
    OutboundRepository outboundRepo;
    InventoryRepository inventoryRepo;
    PersonRepository personRepo;

public:
    OutboundService() 
        : db(Database::getInstance()),
          outboundRepo(db),
          inventoryRepo(db),
          personRepo(db)
    {}

    /**************************************************************************
     * 查询操作
     **************************************************************************/

    // 获取所有出库记录
    std::vector<OutboundModel> getAllOutbound()
    {
        return outboundRepo.findAll();
    }

    // 根据ID获取出库记录
    OutboundModel getOutboundById(int id)
    {
        return outboundRepo.findById(id);
    }

    // 根据物料ID获取出库记录
    std::vector<OutboundModel> getOutboundByMaterialId(int material_id)
    {
        return outboundRepo.findByMaterialId(material_id);
    }

    // 根据仓库ID获取出库记录
    std::vector<OutboundModel> getOutboundByWarehouseId(int warehouse_id)
    {
        return outboundRepo.findByWarehouseId(warehouse_id);
    }

    // 根据人员ID获取出库记录
    std::vector<OutboundModel> getOutboundByPersonId(int person_id)
    {
        return outboundRepo.findByPersonId(person_id);
    }

    // 根据人员卡号获取出库记录
    std::vector<OutboundModel> getOutboundByPersonCardNumber(const std::string &card_number)
    {
        return outboundRepo.findByPersonCardNumber(card_number);
    }

    // 组合条件查询出库记录
    std::vector<OutboundModel> getOutboundByConditions(
        const std::string &warehouse_name = "",
        const std::string &material_name_keyword = "",
        const std::string &person_name = "",
        const std::string &person_card_number = "",
        const std::string &person_department = "",
        const std::string &operator_name = "",
        const std::string &start_date = "",
        const std::string &end_date = "")
    {
        return outboundRepo.findByConditions(
            warehouse_name, material_name_keyword, 
            person_name, person_card_number, person_department,
            operator_name, start_date, end_date
        );
    }

    // 获取出库统计
    Json::Value getOutboundStatistics(const std::string &start_date = "",
                                      const std::string &end_date = "")
    {
        return outboundRepo.getOutboundStatistics(start_date, end_date);
    }

    // 获取月度统计
    Json::Value getMonthlyStatistics(int year)
    {
        return outboundRepo.getMonthlyStatistics(year);
    }

    // 获取人员统计
    Json::Value getPersonStatistics()
    {
        return outboundRepo.getPersonStatistics();
    }

    // 获取物料统计
    Json::Value getMaterialStatistics()
    {
        return outboundRepo.getMaterialStatistics();
    }

    // 搜索出库记录
    std::vector<OutboundModel> searchOutbound(const std::string &keyword)
    {
        return outboundRepo.search(keyword);
    }

    // 获取最新出库记录
    std::vector<OutboundModel> getRecentOutbound(int limit = 10)
    {
        return outboundRepo.findRecent(limit);
    }

    /**************************************************************************
     * 业务操作
     **************************************************************************/

    // 创建出库记录（核心业务逻辑）
    int createOutbound(OutboundModel &record)
    {
        // 验证数据
        if (!validateOutboundRecord(record))
        {
            LOG(LogLevel::Error) << "Invalid outbound record data";
            return 0;
        }

        // 开启事务
        if (!db.beginTransaction())
        {
            LOG(LogLevel::Error) << "Failed to begin transaction";
            return 0;
        }

        try
        {
            // 1. 检查人员信息
            PersonModel person = personRepo.findByCardNumber(record.getPersonCardNumber());
            if (!person.isValid())
            {
                LOG(LogLevel::Error) << "Person not found with card number: " << record.getPersonCardNumber();
                db.rollback();
                return 0;
            }

            // 检查人员余额是否足够
            if (person.getBalance() < record.getTotalPrice())
            {
                LOG(LogLevel::Error) << "Insufficient balance for person: " << person.getName() 
                                    << ", Balance: " << person.getBalance() 
                                    << ", Required: " << record.getTotalPrice();
                db.rollback();
                return 0;
            }

            // 2. 检查库存信息
            InventoryDetailModel inventory = inventoryRepo.findByMaterialAndWarehouse(
                record.getMaterialId(), record.getWarehouseId());
            
            if (!inventory.isValid())
            {
                LOG(LogLevel::Error) << "Inventory not found for material_id: " 
                                    << record.getMaterialId() 
                                    << " and warehouse_id: " << record.getWarehouseId();
                db.rollback();
                return 0;
            }

            // 检查库存是否足够
            if (inventory.getSystemQuantity() < record.getQuantity())
            {
                LOG(LogLevel::Error) << "Insufficient inventory for outbound. Current: " 
                                    << inventory.getSystemQuantity() 
                                    << ", Required: " << record.getQuantity();
                db.rollback();
                return 0;
            }

            // 3. 设置出库记录的其他信息
            record.setPersonId(person.getId());
            record.setPersonBalanceAtOutbound(person.getBalance());
            record.setStockBeforeOutbound(inventory.getSystemQuantity());
            record.setStockAfterOutbound(inventory.getSystemQuantity() - record.getQuantity());
            record.setUnitPriceAtOutbound(inventory.getUnitPrice());
            record.calculateTotalPrice();

            // 4. 保存出库记录
            int outboundId = outboundRepo.save(record);
            if (outboundId <= 0)
            {
                LOG(LogLevel::Error) << "Failed to save outbound record";
                db.rollback();
                return 0;
            }

            record.setId(outboundId);

            // 5. 更新库存数量
            if (!inventoryRepo.decreaseQuantity(inventory.getId(), record.getQuantity()))
            {
                LOG(LogLevel::Error) << "Failed to decrease inventory quantity";
                db.rollback();
                return 0;
            }
            // 6. 更新库存总价
            if (!inventoryRepo.updateTotalPrice(inventory.getId(), record.getTotalPrice()))
            {
                LOG(LogLevel::Error) << "Failed to update inventory total price";
                db.rollback();
                return 0;
            }
            // 7. 更新人员余额
            double newBalance = person.getBalance() - record.getTotalPrice();
            if (!personRepo.updateBalance(person.getId(), newBalance))
            {
                LOG(LogLevel::Error) << "Failed to update person balance";
                db.rollback();
                return 0;
            }

            // 提交事务
            if (!db.commit())
            {
                LOG(LogLevel::Error) << "Failed to commit transaction";
                db.rollback();
                return 0;
            }

            return outboundId;
        }
        catch (const std::exception &e)
        {
            LOG(LogLevel::Error) << "Exception in createOutbound: " << e.what();
            db.rollback();
            return 0;
        }
    }

    // 删除出库记录（带回滚）
    bool deleteOutbound(int id)
    {
        // 查询出库记录
        OutboundModel record = outboundRepo.findById(id);
        if (!record.isValid())
        {
            LOG(LogLevel::Error) << "Outbound record not found for deletion";
            return false;
        }

        // 开启事务
        if (!db.beginTransaction())
        {
            LOG(LogLevel::Error) << "Failed to begin transaction for deletion";
            return false;
        }

        try
        {
            // 1. 回滚库存数量
            InventoryDetailModel inventory = inventoryRepo.findByMaterialAndWarehouse(
                record.getMaterialId(), record.getWarehouseId());
            
            if (inventory.isValid())
            {
                // 增加库存数量
                if (!inventoryRepo.increaseQuantity(inventory.getId(), record.getQuantity()))
                {
                    LOG(LogLevel::Error) << "Failed to rollback inventory";
                    db.rollback();
                    return false;
                }
            }

            // 2. 回滚人员余额
            PersonModel person = personRepo.findById(record.getPersonId());
            if (person.isValid())
            {
                double newBalance = person.getBalance() + record.getTotalPrice();
                if (!personRepo.updateBalance(person.getId(), newBalance))
                {
                    LOG(LogLevel::Error) << "Failed to rollback person balance";
                    db.rollback();
                    return false;
                }
            }

            // 3. 删除出库记录
            if (!outboundRepo.remove(id))
            {
                LOG(LogLevel::Error) << "Failed to delete outbound record";
                db.rollback();
                return false;
            }

            // 提交事务
            if (!db.commit())
            {
                LOG(LogLevel::Error) << "Failed to commit transaction for deletion";
                db.rollback();
                return false;
            }

            return true;
        }
        catch (const std::exception &e)
        {
            LOG(LogLevel::Error) << "Exception in deleteOutbound: " << e.what();
            db.rollback();
            return false;
        }
    }

    // 批量出库
    int batchCreateOutbound(std::vector<OutboundModel> &records)
    {
        if (records.empty())
        {
            LOG(LogLevel::Error) << "Empty records for batch create";
            return 0;
        }

        int successCount = 0;
        
        if (!db.beginTransaction())
        {
            LOG(LogLevel::Error) << "Failed to begin transaction for batch create";
            return 0;
        }

        try
        {
            for (auto &record : records)
            {
                // 验证记录
                if (!validateOutboundRecord(record))
                {
                    LOG(LogLevel::Error) << "Invalid record in batch";
                    continue;
                }

                // 单独处理每条记录（调用 createOutbound 的逻辑）
                // 但使用统一的事务
                PersonModel person = personRepo.findByCardNumber(record.getPersonCardNumber());
                if (!person.isValid())
                {
                    LOG(LogLevel::Error) << "Person not found in batch: " << record.getPersonCardNumber();
                    continue;
                }

                InventoryDetailModel inventory = inventoryRepo.findByMaterialAndWarehouse(
                    record.getMaterialId(), record.getWarehouseId());
                
                if (!inventory.isValid() || inventory.getSystemQuantity() < record.getQuantity())
                {
                    LOG(LogLevel::Error) << "Insufficient inventory in batch";
                    continue;
                }

                // 检查人员余额
                if (person.getBalance() < record.getTotalPrice())
                {
                    LOG(LogLevel::Error) << "Insufficient balance in batch";
                    continue;
                }

                // 设置记录信息
                record.setPersonId(person.getId());
                record.setPersonBalanceAtOutbound(person.getBalance());
                record.setStockBeforeOutbound(inventory.getSystemQuantity());
                record.setStockAfterOutbound(inventory.getSystemQuantity() - record.getQuantity());
                record.setUnitPriceAtOutbound(inventory.getUnitPrice());
                record.calculateTotalPrice();

                // 保存出库记录
                int outboundId = outboundRepo.save(record);
                if (outboundId <= 0)
                {
                    LOG(LogLevel::Error) << "Failed to save record in batch";
                    continue;
                }

                record.setId(outboundId);

                // 更新库存
                if (!inventoryRepo.decreaseQuantity(inventory.getId(), record.getQuantity()))
                {
                    LOG(LogLevel::Error) << "Failed to update inventory in batch";
                    outboundRepo.remove(outboundId);
                    continue;
                }

                // 更新人员余额
                double newBalance = person.getBalance() - record.getTotalPrice();
                if (!personRepo.updateBalance(person.getId(), newBalance))
                {
                    LOG(LogLevel::Error) << "Failed to update person balance in batch";
                    // 回滚库存
                    inventoryRepo.increaseQuantity(inventory.getId(), record.getQuantity());
                    outboundRepo.remove(outboundId);
                    continue;
                }

                successCount++;
            }

            if (successCount > 0)
            {
                if (!db.commit())
                {
                    LOG(LogLevel::Error) << "Failed to commit batch transaction";
                    db.rollback();
                    return 0;
                }
                return successCount;
            }
            else
            {
                db.rollback();
                return 0;
            }
        }
        catch (const std::exception &e)
        {
            LOG(LogLevel::Error) << "Exception in batchCreateOutbound: " << e.what();
            db.rollback();
            return 0;
        }
    }

    // 检查是否可以出库
    bool checkOutboundAvailability(int material_id, int warehouse_id, int quantity)
    {
        InventoryDetailModel inventory = inventoryRepo.findByMaterialAndWarehouse(material_id, warehouse_id);
        if (!inventory.isValid())
        {
            return false;
        }
        return inventory.getSystemQuantity() >= quantity;
    }

    // 检查人员是否可以领用
    bool checkPersonAvailability(const std::string &card_number, double required_amount)
    {
        PersonModel person = personRepo.findByCardNumber(card_number);
        if (!person.isValid() || person.getStatus() != "正常")
        {
            return false;
        }
        return person.getBalance() >= required_amount;
    }

    // 获取出库预估信息
    Json::Value getOutboundEstimation(int material_id, int warehouse_id, int quantity, const std::string &card_number)
    {
        Json::Value result;

        // 检查库存
        InventoryDetailModel inventory = inventoryRepo.findByMaterialAndWarehouse(material_id, warehouse_id);
        if (!inventory.isValid())
        {
            result["error"] = "库存不存在";
            return result;
        }

        if (inventory.getSystemQuantity() < quantity)
        {
            result["error"] = "库存不足";
            result["available_quantity"] = inventory.getSystemQuantity();
            return result;
        }

        // 检查人员
        PersonModel person = personRepo.findByCardNumber(card_number);
        if (!person.isValid())
        {
            result["error"] = "人员不存在";
            return result;
        }

        if (person.getStatus() != "正常")
        {
            result["error"] = "人员状态异常: " + person.getStatus();
            return result;
        }

        // 计算金额
        double total_price = quantity * inventory.getUnitPrice();
        double balance_after = person.getBalance() - total_price;

        // 返回预估信息
        result["material_name"] = inventory.getMaterialName();
        result["unit_price"] = inventory.getUnitPrice();
        result["total_price"] = total_price;
        result["person_name"] = person.getName();
        result["current_balance"] = person.getBalance();
        result["balance_after"] = balance_after;
        result["can_outbound"] = (balance_after >= 0);
        result["available_quantity"] = inventory.getSystemQuantity();

        return result;
    }

private:
    /**************************************************************************
     * 验证出库记录数据
     **************************************************************************/
    bool validateOutboundRecord(const OutboundModel &record)
    {
        if (record.getMaterialId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid material ID: " << record.getMaterialId();
            return false;
        }
        
        if (record.getWarehouseId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid warehouse ID: " << record.getWarehouseId();
            return false;
        }
        
        if (record.getQuantity() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid quantity: " << record.getQuantity();
            return false;
        }
        
        if (record.getPersonCardNumber().empty())
        {
            LOG(LogLevel::Error) << "Person card number is empty";
            return false;
        }
        
        if (record.getPersonName().empty())
        {
            LOG(LogLevel::Error) << "Person name is empty";
            return false;
        }
        
        if (record.getOperatorName().empty())
        {
            LOG(LogLevel::Error) << "Operator name is empty";
            return false;
        }
        
        return true;
    }
};