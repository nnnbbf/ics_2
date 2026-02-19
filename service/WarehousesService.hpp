// warehouseservice.h
#pragma once
#include "../repository/WarehousesRepository.hpp"
#include "../model/WarehousesModel.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class WarehouseService {
private:
    WarehousesRepository repository;
    
public:
    WarehouseService() : repository(Database::getInstance()) {}
    
    // 查询
    WarehousesModel getWarehouseById(int id)
    {
        auto results = repository.findById(id);
        if (!results.empty()) {
            return results[0];
        }
        return WarehousesModel();
    }
    
    WarehousesModel getWarehouseByName(const std::string& name)
    {
        auto results = repository.findByName(name);
        if (!results.empty()) {
            return results[0];
        }
        return WarehousesModel();
    }
    
    std::vector<WarehousesModel> getAllWarehouses(const std::string& keyword = "")
    {
        if (keyword.empty()) {
            return repository.findAll();
        } else {
            // 可以按名称或位置搜索
            std::vector<WarehousesModel> byName = repository.findByName(keyword);
            std::vector<WarehousesModel> byLocation = repository.findByLocation(keyword);
            
            // 合并结果，去重
            std::vector<WarehousesModel> allResults = byName;
            for (const auto& warehouse : byLocation) {
                bool exists = false;
                for (const auto& existing : allResults) {
                    if (existing.getId() == warehouse.getId()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    allResults.push_back(warehouse);
                }
            }
            return allResults;
        }
    }
    
    // 创建仓库
    WarehousesModel createWarehouse(const WarehousesModel& warehouse)
    {
        // 验证仓库名称
        if (warehouse.getName().empty()) {
            LOG(LogLevel::Error) << "Warehouse name cannot be empty";
            return WarehousesModel();
        }
        
        // 检查仓库名称是否已存在
        if (!repository.findByName(warehouse.getName()).empty()) {
            LOG(LogLevel::Error) << "Warehouse name already exists: " << warehouse.getName();
            return WarehousesModel();
        }
        
        // 验证位置长度
        if (warehouse.getLocation().length() > 200) {
            LOG(LogLevel::Error) << "Warehouse location too long";
            return WarehousesModel();
        }
        
        WarehousesModel newWarehouse = warehouse;
        if (repository.save(newWarehouse) == -1) {
            LOG(LogLevel::Error) << "Failed to save new warehouse: " << warehouse.getName();
            return WarehousesModel();
        }
        return newWarehouse;
    }
    
    // 更新仓库
    bool updateWarehouse(const WarehousesModel& warehouse)
    {
        if (warehouse.getId() <= 0) {
            LOG(LogLevel::Error) << "Invalid warehouse ID for update";
            return false;
        }
        
        // 验证仓库名称
        if (warehouse.getName().empty()) {
            LOG(LogLevel::Error) << "Warehouse name cannot be empty";
            return false;
        }
        
        // 检查仓库名称是否被其他仓库使用
        auto existingWarehouses = repository.findByName(warehouse.getName());
        if (!existingWarehouses.empty()) {
            for (const auto& existing : existingWarehouses) {
                if (existing.getId() != warehouse.getId()) {
                    LOG(LogLevel::Error) << "Warehouse name already used by another warehouse";
                    return false;
                }
            }
        }
        
        // 验证位置长度
        if (warehouse.getLocation().length() > 200) {
            LOG(LogLevel::Error) << "Warehouse location too long";
            return false;
        }
        
        return repository.update(warehouse);
    }
    
    // 删除仓库
    bool deleteWarehouse(int id)
    {
        if (id <= 0) {
            LOG(LogLevel::Error) << "Invalid warehouse ID for deletion";
            return false;
        }
        
        // TODO: 检查仓库是否有库存记录
        // 可以先查询是否有库存，如果有则不能删除
        
        return repository.remove(id);
    }
    
    bool deleteWarehouseByName(const std::string& name)
    {
        if (name.empty()) {
            LOG(LogLevel::Error) << "Warehouse name cannot be empty";
            return false;
        }
        
        auto warehouse = getWarehouseByName(name);
        if (warehouse.getId() <= 0) {
            LOG(LogLevel::Error) << "Warehouse not found: " << name;
            return false;
        }
        
        // TODO: 检查仓库是否有库存记录
        
        return repository.removeByName(name);
    }
    
    // 统计信息
    int getTotalWarehouseCount()
    {
        return getAllWarehouses().size();
    }
    
    // 批量操作
    std::vector<WarehousesModel> createWarehouses(const std::vector<WarehousesModel>& warehouses)
    {
        std::vector<WarehousesModel> createdWarehouses;
        for (const auto& warehouse : warehouses) {
            WarehousesModel created = createWarehouse(warehouse);
            if (created.getId() > 0) {
                createdWarehouses.push_back(created);
            }
        }
        return createdWarehouses;
    }
    
    bool updateWarehouses(const std::vector<WarehousesModel>& warehouses)
    {
        bool allSuccess = true;
        for (const auto& warehouse : warehouses) {
            if (!updateWarehouse(warehouse)) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
};