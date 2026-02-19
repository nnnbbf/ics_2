// domain/material/IMaterialRepository.hpp
#pragma once
#include "Material.hpp"
#include <vector>
#include <string>

namespace Domain::MaterialModule {
    
    class IMaterialRepository {
    public:
        virtual ~IMaterialRepository() = default;
        
        // ============ 基础CRUD操作 ============
        
        // 根据ID查找材料
        virtual Material FindById(Material::IdType id) = 0;
        
        // 查找所有材料
        virtual std::vector<Material> FindAll() = 0;
        
        // 添加新材料
        virtual void Add(Material& material) = 0;
        
        // 更新材料
        virtual void Update(const Material& material) = 0;
        
        // 删除材料
        virtual bool Remove(Material::IdType id) = 0;
        
        // 根据名称模糊查询
        virtual std::vector<Material> FindByNameLike(const std::string& nameKeyword) = 0;
        
        // 根据分类查找
        virtual std::vector<Material> FindByCategory(const std::string& category) = 0;
        
        // 分页查找所有材料
        // page: 页码（从1开始）
        // pageSize: 每页记录数
        virtual std::vector<Material> FindAllPaginated(int page, int pageSize) = 0;
    
    
        // 检查名称-规格是否存在
        virtual bool ExistsBy(const std::string& name, const std::string& specKeyword) = 0;


    };
    
} // namespace Domain::MaterialModule