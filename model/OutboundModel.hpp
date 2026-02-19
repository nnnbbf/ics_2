#pragma once

#include <string>
#include <jsoncpp/json/json.h>

class OutboundModel
{
private:
    int id;                                 // 出库记录ID
    
    // 人员信息（冗余存储）
    int person_id;                          // 人员ID
    std::string person_name;                // 人员姓名
    std::string person_card_number;         // 卡号
    std::string person_department;          // 部门
    double person_balance_at_outbound;      // 出库时的余额快照
    
    // 物料信息（冗余存储）
    int material_id;                        // 物料ID
    std::string material_name;              // 物料名称
    std::string material_category;          // 物料分类
    std::string material_sub_category;      // 物料子分类
    std::string material_unit;              // 物料单位
    std::string material_specification;     // 物料规格
    
    // 仓库信息（冗余存储）
    int warehouse_id;                       // 仓库ID
    std::string warehouse_name;             // 仓库名称
    std::string shelf_name;                 // 货架名称
    std::string placement_layer;            // 放置层数
    
    // 价格信息（冗余存储）
    double unit_price_at_outbound;          // 出库时的单价
    int quantity;                           // 出库数量
    double total_price;                     // 批次总价
    
    // 库存状态
    int stock_before_outbound;              // 出库前库存
    int stock_after_outbound;               // 出库后库存
    
    // 操作信息
    std::string operator_name;              // 操作人
    
    // 时间信息
    std::string outbound_date;              // 出库日期
    
    // 其他信息
    std::string remarks;                    // 备注

public:
    OutboundModel() : id(0), person_id(0), material_id(0), warehouse_id(0),
                      person_balance_at_outbound(0.0), unit_price_at_outbound(0.0),
                      quantity(0), total_price(0.0), stock_before_outbound(0),
                      stock_after_outbound(0) {}

    /**************************************************************************
     * 静态工厂方法：从JSON创建对象
     **************************************************************************/
    static OutboundModel fromJson(const Json::Value &json)
    {
        OutboundModel model;

        // 所有数字字段都需要从字符串转换
        if (json.isMember("id") && !json["id"].asString().empty())
            model.id = std::stoi(json["id"].asString());

        if (json.isMember("person_id") && !json["person_id"].asString().empty())
            model.person_id = std::stoi(json["person_id"].asString());

        if (json.isMember("material_id") && !json["material_id"].asString().empty())
            model.material_id = std::stoi(json["material_id"].asString());

        if (json.isMember("warehouse_id") && !json["warehouse_id"].asString().empty())
            model.warehouse_id = std::stoi(json["warehouse_id"].asString());

        // 人员信息
        if (json.isMember("person_name"))
            model.person_name = json["person_name"].asString();

        if (json.isMember("person_card_number"))
            model.person_card_number = json["person_card_number"].asString();

        if (json.isMember("person_department"))
            model.person_department = json["person_department"].asString();

        if (json.isMember("person_balance_at_outbound") && !json["person_balance_at_outbound"].asString().empty())
            model.person_balance_at_outbound = std::stod(json["person_balance_at_outbound"].asString());

        // 物料信息
        if (json.isMember("material_name"))
            model.material_name = json["material_name"].asString();

        if (json.isMember("material_category"))
            model.material_category = json["material_category"].asString();

        if (json.isMember("material_sub_category"))
            model.material_sub_category = json["material_sub_category"].asString();

        if (json.isMember("material_unit"))
            model.material_unit = json["material_unit"].asString();

        if (json.isMember("material_specification"))
            model.material_specification = json["material_specification"].asString();

        // 仓库信息
        if (json.isMember("warehouse_name"))
            model.warehouse_name = json["warehouse_name"].asString();

        if (json.isMember("shelf_name"))
            model.shelf_name = json["shelf_name"].asString();

        if (json.isMember("placement_layer"))
            model.placement_layer = json["placement_layer"].asString();

        // 出库数量与价格
        if (json.isMember("quantity") && !json["quantity"].asString().empty())
            model.quantity = std::stoi(json["quantity"].asString());

        if (json.isMember("unit_price_at_outbound") && !json["unit_price_at_outbound"].asString().empty())
            model.unit_price_at_outbound = std::stod(json["unit_price_at_outbound"].asString());

        // 字符串字段直接赋值
        if (json.isMember("operator_name"))
            model.operator_name = json["operator_name"].asString();

        if (json.isMember("remarks"))
            model.remarks = json["remarks"].asString();

        if (json.isMember("outbound_date"))
            model.outbound_date = json["outbound_date"].asString();

        // 库存状态
        if (json.isMember("stock_before_outbound") && !json["stock_before_outbound"].asString().empty())
            model.stock_before_outbound = std::stoi(json["stock_before_outbound"].asString());

        if (json.isMember("stock_after_outbound") && !json["stock_after_outbound"].asString().empty())
            model.stock_after_outbound = std::stoi(json["stock_after_outbound"].asString());

        // 计算总价
        model.calculateTotalPrice();

        return model;
    }

    /**************************************************************************
     * 转换为JSON对象
     **************************************************************************/
    Json::Value toJson() const
    {
        Json::Value json;

        json["id"] = std::to_string(id);
        
        // 人员信息
        json["person_id"] = std::to_string(person_id);
        json["person_name"] = person_name;
        json["person_card_number"] = person_card_number;
        json["person_department"] = person_department;
        json["person_balance_at_outbound"] = std::to_string(person_balance_at_outbound);
        
        // 物料信息
        json["material_id"] = std::to_string(material_id);
        json["material_name"] = material_name;
        json["material_category"] = material_category;
        json["material_sub_category"] = material_sub_category;
        json["material_unit"] = material_unit;
        json["material_specification"] = material_specification;
        
        // 仓库信息
        json["warehouse_id"] = std::to_string(warehouse_id);
        json["warehouse_name"] = warehouse_name;
        json["shelf_name"] = shelf_name;
        json["placement_layer"] = placement_layer;
        
        // 出库数量与价格
        json["unit_price_at_outbound"] = std::to_string(unit_price_at_outbound);
        json["quantity"] = std::to_string(quantity);
        json["total_price"] = std::to_string(total_price);
        
        // 库存状态
        json["stock_before_outbound"] = std::to_string(stock_before_outbound);
        json["stock_after_outbound"] = std::to_string(stock_after_outbound);
        
        // 操作信息
        json["operator_name"] = operator_name;
        
        // 时间信息
        json["outbound_date"] = outbound_date;
        
        // 其他信息
        json["remarks"] = remarks;

        return json;
    }

    /**************************************************************************
     * Getter方法
     **************************************************************************/
    int getId() const { return id; }
    
    // 人员信息
    int getPersonId() const { return person_id; }
    const std::string &getPersonName() const { return person_name; }
    const std::string &getPersonCardNumber() const { return person_card_number; }
    const std::string &getPersonDepartment() const { return person_department; }
    double getPersonBalanceAtOutbound() const { return person_balance_at_outbound; }
    
    // 物料信息
    int getMaterialId() const { return material_id; }
    const std::string &getMaterialName() const { return material_name; }
    const std::string &getMaterialCategory() const { return material_category; }
    const std::string &getMaterialSubCategory() const { return material_sub_category; }
    const std::string &getMaterialUnit() const { return material_unit; }
    const std::string &getMaterialSpecification() const { return material_specification; }
    
    // 仓库信息
    int getWarehouseId() const { return warehouse_id; }
    const std::string &getWarehouseName() const { return warehouse_name; }
    const std::string &getShelfName() const { return shelf_name; }
    const std::string &getPlacementLayer() const { return placement_layer; }
    
    // 出库数量与价格
    double getUnitPriceAtOutbound() const { return unit_price_at_outbound; }
    int getQuantity() const { return quantity; }
    double getTotalPrice() const { return total_price; }
    
    // 库存状态
    int getStockBeforeOutbound() const { return stock_before_outbound; }
    int getStockAfterOutbound() const { return stock_after_outbound; }
    
    // 操作信息
    const std::string &getOperatorName() const { return operator_name; }
    
    // 时间信息
    const std::string &getOutboundDate() const { return outbound_date; }
    
    // 其他信息
    const std::string &getRemarks() const { return remarks; }

    /**************************************************************************
     * Setter方法
     **************************************************************************/
    void setId(int newId) { id = newId; }
    
    // 人员信息
    void setPersonId(int newPersonId) { person_id = newPersonId; }
    void setPersonName(const std::string &newPersonName) { person_name = newPersonName; }
    void setPersonCardNumber(const std::string &newPersonCardNumber) { person_card_number = newPersonCardNumber; }
    void setPersonDepartment(const std::string &newPersonDepartment) { person_department = newPersonDepartment; }
    void setPersonBalanceAtOutbound(double newPersonBalance) { person_balance_at_outbound = newPersonBalance; }
    
    // 物料信息
    void setMaterialId(int newMaterialId) { material_id = newMaterialId; }
    void setMaterialName(const std::string &newMaterialName) { material_name = newMaterialName; }
    void setMaterialCategory(const std::string &newMaterialCategory) { material_category = newMaterialCategory; }
    void setMaterialSubCategory(const std::string &newMaterialSubCategory) { material_sub_category = newMaterialSubCategory; }
    void setMaterialUnit(const std::string &newMaterialUnit) { material_unit = newMaterialUnit; }
    void setMaterialSpecification(const std::string &newMaterialSpecification) { material_specification = newMaterialSpecification; }
    
    // 仓库信息
    void setWarehouseId(int newWarehouseId) { warehouse_id = newWarehouseId; }
    void setWarehouseName(const std::string &newWarehouseName) { warehouse_name = newWarehouseName; }
    void setShelfName(const std::string &newShelfName) { shelf_name = newShelfName; }
    void setPlacementLayer(const std::string &newPlacementLayer) { placement_layer = newPlacementLayer; }
    
    // 出库数量与价格
    void setQuantity(int newQuantity)
    {
        quantity = newQuantity;
        calculateTotalPrice();
    }
    
    void setUnitPriceAtOutbound(double newUnitPrice)
    {
        unit_price_at_outbound = newUnitPrice;
        calculateTotalPrice();
    }
    
    void setTotalPrice(double newTotalPrice) { total_price = newTotalPrice; }
    
    // 库存状态
    void setStockBeforeOutbound(int newStockBefore) { stock_before_outbound = newStockBefore; }
    void setStockAfterOutbound(int newStockAfter) { stock_after_outbound = newStockAfter; }
    
    // 操作信息
    void setOperatorName(const std::string &newOperatorName) { operator_name = newOperatorName; }
    
    // 时间信息
    void setOutboundDate(const std::string &newOutboundDate) { outbound_date = newOutboundDate; }
    
    // 其他信息
    void setRemarks(const std::string &newRemarks) { remarks = newRemarks; }

    /**************************************************************************
     * 业务逻辑方法
     **************************************************************************/
    void calculateTotalPrice()
    {
        total_price = quantity * unit_price_at_outbound;
    }

    // 计算库存变化
    void calculateStockChanges(int currentStock)
    {
        stock_before_outbound = currentStock;
        stock_after_outbound = currentStock - quantity;
    }

    // 验证出库记录是否有效
    bool isValid() const
    {
        return material_id > 0 &&
               warehouse_id > 0 &&
               quantity > 0 &&
               unit_price_at_outbound >= 0 &&
               !person_name.empty() &&
               !person_card_number.empty() &&
               !material_name.empty() &&
               !warehouse_name.empty() &&
               !operator_name.empty() &&
               stock_before_outbound >= quantity;  // 库存足够
    }

    // 检查库存是否足够
    bool isStockSufficient(int currentStock) const
    {
        return currentStock >= quantity;
    }

    // 获取出库摘要信息
    std::string getSummary() const
    {
        return person_name + " 领用 " + material_name + 
               " " + std::to_string(quantity) + material_unit + 
               "，扣款 ¥" + std::to_string(total_price);
    }

    // 检查是否为高价值出库
    bool isHighValue(double threshold = 1000.0) const
    {
        return total_price > threshold;
    }

    // 检查是否为大批量出库
    bool isBulkOutbound(int threshold = 100) const
    {
        return quantity > threshold;
    }

    // 获取扣款后的余额
    double getBalanceAfterDeduction() const
    {
        return person_balance_at_outbound - total_price;
    }

    // 设置并计算所有字段
    void setAndCalculateAll(int currentStock, double personBalance)
    {
        stock_before_outbound = currentStock;
        stock_after_outbound = currentStock - quantity;
        person_balance_at_outbound = personBalance;
        calculateTotalPrice();
    }

    // 验证出库前后库存一致性
    bool isStockConsistent() const
    {
        return stock_after_outbound == (stock_before_outbound - quantity);
    }
};