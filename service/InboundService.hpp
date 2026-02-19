// inboundservice.h
#pragma once
#include "../database/Database.hpp"
#include "../repository/InboundRepository.hpp"
#include "../repository/InventoryRepository.hpp"
#include "../model/InboundModel.hpp"
#include "../model/InventoryDetailModel.hpp"
#include <vector>
#include <string>

class InboundService
{
private:
    Database &db;
    InboundRepository inboundRepo;
    InventoryRepository inventoryRepo;

public:
    InboundService() 
        : db(Database::getInstance()),
          inboundRepo(db),
          inventoryRepo(db)
    {}

    /**************************************************************************
     * 查询操作
     **************************************************************************/

    // 获取所有入库记录
    std::vector<InboundModel> getAllInbound()
    {
        return inboundRepo.findAll();
    }

    // 根据ID获取入库记录
    InboundModel getInboundById(int id)
    {
        return inboundRepo.findById(id);
    }

    // 根据物料ID获取入库记录
    std::vector<InboundModel> getInboundByMaterialId(int material_id)
    {
        return inboundRepo.findByMaterialId(material_id);
    }

    // 根据仓库ID获取入库记录
    std::vector<InboundModel> getInboundByWarehouseId(int warehouse_id)
    {
        return inboundRepo.findByWarehouseId(warehouse_id);
    }

    // 组合条件查询入库记录
    std::vector<InboundModel> getInboundByConditions(
        const std::string &warehouse_name = "",
        const std::string &material_name_keyword = "",
        const std::string &supplier = "",
        const std::string &purchaser = "",
        const std::string &registrant = "",
        const std::string &start_date = "",
        const std::string &end_date = "")
    {
        return inboundRepo.findByConditions(
            warehouse_name, material_name_keyword, 
            supplier, purchaser, registrant, 
            start_date, end_date
        );
    }

    // 获取入库统计
    Json::Value getInboundStatistics(const std::string &start_date = "",
                                     const std::string &end_date = "")
    {
        return inboundRepo.getInboundStatistics(start_date, end_date);
    }

    // 获取月度统计
    Json::Value getMonthlyStatistics(int year)
    {
        return inboundRepo.getMonthlyStatistics(year);
    }

    // 获取供应商统计
    Json::Value getSupplierStatistics()
    {
        return inboundRepo.getSupplierStatistics();
    }

    /**************************************************************************
     * 业务操作
     **************************************************************************/

    // 创建入库记录（核心业务逻辑）
    int createInbound(InboundModel &record)
    {
        // 验证数据
        if (!validateInboundRecord(record))
        {
            LOG(LogLevel::Error) << "Invalid inbound record data";
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
            // 计算总价
            record.calculateTotalPrice();

            // 保存入库记录
            int inboundId = inboundRepo.save(record);
            if (inboundId <= 0)
            {
                LOG(LogLevel::Error) << "Failed to save inbound record";
                db.rollback();
                return 0;
            }

            record.setId(inboundId);

            // 更新或创建库存
            if (!updateOrCreateInventory(record))
            {
                LOG(LogLevel::Error) << "Failed to update inventory";
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

            return inboundId;
        }
        catch (const std::exception &e)
        {
            LOG(LogLevel::Error) << "Exception in createInbound: " << e.what();
            db.rollback();
            return 0;
        }
    }

    // 更新入库记录
    bool updateInbound(const InboundModel &record)
    {
        // 验证
        if (!record.isValid() || record.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid inbound record for update";
            return false;
        }

        // 开启事务
        if (!db.beginTransaction())
        {
            LOG(LogLevel::Error) << "Failed to begin transaction for update";
            return false;
        }

        try
        {
            // 查询原记录
            InboundModel oldRecord = inboundRepo.findById(record.getId());
            if (!oldRecord.isValid())
            {
                LOG(LogLevel::Error) << "Original inbound record not found";
                db.rollback();
                return false;
            }

            // 检查物料或仓库是否改变
            if (oldRecord.getMaterialId() != record.getMaterialId() ||
                oldRecord.getWarehouseId() != record.getWarehouseId())
            {
                LOG(LogLevel::Error) << "Cannot change material or warehouse in update";
                db.rollback();
                return false;
            }

            // 1. 回滚原记录对库存的影响
            if (!rollbackInventory(oldRecord))
            {
                LOG(LogLevel::Error) << "Failed to rollback old inventory";
                db.rollback();
                return false;
            }

            // 2. 更新入库记录
            if (!inboundRepo.update(record))
            {
                // 恢复原记录的影响
                updateOrCreateInventory(oldRecord);
                db.rollback();
                return false;
            }

            // 3. 应用新记录对库存的影响
            if (!updateOrCreateInventory(record))
            {
                LOG(LogLevel::Debug) <<  "updateOrCreateInventory Failed";
                // 恢复原状态
                rollbackInventory(record);
                updateOrCreateInventory(oldRecord);
                inboundRepo.update(oldRecord);
                db.rollback();
                return false;
            }

            // 提交事务
            if (!db.commit())
            {
                LOG(LogLevel::Error) << "Failed to commit transaction for update";
                db.rollback();
                return false;
            }

            return true;
        }
        catch (const std::exception &e)
        {
            LOG(LogLevel::Error) << "Exception in updateInbound: " << e.what();
            db.rollback();
            return false;
        }
    }

    // 删除入库记录
    bool deleteInbound(int id)
    {
        // 查询入库记录
        InboundModel record = inboundRepo.findById(id);
        if (!record.isValid())
        {
            LOG(LogLevel::Error) << "Inbound record not found for deletion";
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
            // 回滚库存
            if (!rollbackInventory(record))
            {
                LOG(LogLevel::Error) << "Failed to rollback inventory";
                db.rollback();
                return false;
            }

            // 删除入库记录
            if (!inboundRepo.remove(id))
            {
                LOG(LogLevel::Error) << "Failed to delete inbound record";
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
            LOG(LogLevel::Error) << "Exception in deleteInbound: " << e.what();
            db.rollback();
            return false;
        }
    }

    // 批量入库
    int batchCreateInbound(std::vector<InboundModel> &records)
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
                if (!validateInboundRecord(record))
                {
                    LOG(LogLevel::Error) << "Invalid record in batch";
                    continue;
                }

                // 计算总价
                record.calculateTotalPrice();

                // 保存入库记录
                int inboundId = inboundRepo.save(record);
                if (inboundId <= 0)
                {
                    LOG(LogLevel::Error) << "Failed to save record in batch";
                    continue;
                }

                record.setId(inboundId);

                // 更新库存
                if (!updateOrCreateInventory(record))
                {
                    LOG(LogLevel::Error) << "Failed to update inventory in batch";
                    // 回滚已插入的记录
                    inboundRepo.remove(inboundId);
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
            LOG(LogLevel::Error) << "Exception in batchCreateInbound: " << e.what();
            db.rollback();
            return 0;
        }
    }

private:
    /**************************************************************************
     * 验证入库记录数据
     **************************************************************************/
    bool validateInboundRecord(const InboundModel &record)
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
        
        if (record.getUnitPrice() < 0)
        {
            LOG(LogLevel::Error) << "Invalid unit price: " << record.getUnitPrice();
            return false;
        }
        
        if (record.getMaterialName().empty())
        {
            LOG(LogLevel::Error) << "Material name is empty";
            return false;
        }
        
        if (record.getWarehouseName().empty())
        {
            LOG(LogLevel::Error) << "Warehouse name is empty";
            return false;
        }
        
        return true;
    }

    /**************************************************************************
     * 更新或创建库存记录（入库时调用）
     **************************************************************************/
    bool updateOrCreateInventory(const InboundModel &record)
    {
        // 1. 检查库存是否已存在
        InventoryDetailModel existing = inventoryRepo.findByMaterialAndWarehouse(
            record.getMaterialId(), record.getWarehouseId());
        
        if (existing.isValid())
        {
            // 2. 库存已存在，计算加权平均
            int currentQty = existing.getSystemQuantity();
            double currentPrice = existing.getUnitPrice();
            double currentTotal = existing.getTotalPrice();
            
            int inboundQty = record.getQuantity();
            double inboundPrice = record.getUnitPrice();
            double inboundTotal = inboundQty * inboundPrice;
            
            int totalQty = currentQty + inboundQty;
            double weightedPrice = (currentTotal + inboundTotal) / totalQty;
            double newTotal = totalQty * weightedPrice;
            
            // 更新库存记录
            InventoryDetailModel updated = existing;
            updated.setSystemQuantity(totalQty);
            updated.setActualQuantity(totalQty);
            updated.setUnitPrice(weightedPrice);
            updated.setTotalPrice(newTotal);
            
            return inventoryRepo.update(updated);
        }
        else
        {
            // 3. 库存不存在，创建新记录
            InventoryDetailModel newInventory;
            newInventory.setMaterialId(record.getMaterialId());
            newInventory.setMaterialName(record.getMaterialName());
            newInventory.setMaterialCategory(record.getMaterialCategory());
            newInventory.setMaterialSubCategory(record.getMaterialSubCategory());
            newInventory.setMaterialUnit(record.getMaterialUnit());
            newInventory.setMaterialSpecification(record.getMaterialSpecification());
            
            newInventory.setWarehouseId(record.getWarehouseId());
            newInventory.setWarehouseName(record.getWarehouseName());
            
            newInventory.setSystemQuantity(record.getQuantity());
            newInventory.setActualQuantity(record.getQuantity());
            newInventory.setShelfName(record.getShelfName());
            newInventory.setPlacementLayer(record.getPlacementLayer());
            newInventory.setUnitPrice(record.getUnitPrice());
            
            // 计算总价
            newInventory.calculateTotalPrice();
            
            // 保存库存记录
            return inventoryRepo.save(newInventory) > 0;
        }
    }

    /**************************************************************************
     * 回滚库存（删除入库记录时调用）
     **************************************************************************/
    bool rollbackInventory(const InboundModel &record)
    {
        // 查询当前库存
        InventoryDetailModel inventory = inventoryRepo.findByMaterialAndWarehouse(
            record.getMaterialId(), record.getWarehouseId());
        
        if (!inventory.isValid())
        {
            // 库存已不存在，直接返回成功
            return true;
        }
        
        int currentQty = inventory.getSystemQuantity();
        int inboundQty = record.getQuantity();
        int newQty = currentQty - inboundQty;
        
        if (newQty <= 0)
        {
            // 如果回滚后数量为0或负数，删除库存记录
            return inventoryRepo.remove(inventory.getId());
        }
        else
        {
            // 更新库存数量（单价保持不变）
            InventoryDetailModel updated = inventory;
            updated.setSystemQuantity(newQty);
            updated.setActualQuantity(newQty);
            updated.calculateTotalPrice();
            
            return inventoryRepo.update(updated);
        }
    }
};