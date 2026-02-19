// infrastructure/persistence/MaterialRepository.hpp
#pragma once
#include "../../../database/Database.hpp"
#include "../../domain/material/IMaterialRepository.hpp"
#include "../../domain/material/Material.hpp"
#include "../../../shared/util/log.hpp"
#include "../common/CommandBuilder.hpp"
#include "../common/QueryBuilder.hpp"
#include <jsoncpp/json/json.h>
#include <memory>
#include <stdexcept>
#include <vector>

namespace Infrastructure::Persistence
{

    using namespace Domain::MaterialModule;
    using Infrastructure::Common::CommandBuilder;
    using Infrastructure::Common::QueryBuilder;
    class MaterialRepository : public IMaterialRepository
    {
    private:
        Database &db_;

    public:
        explicit MaterialRepository(Database &db) : db_(db) {}

        // ============ 实现IMaterialRepository接口 ============

        // 查找所有材料
        std::vector<Material> FindAll() override
        {
            QueryBuilder queryBuilder;
            queryBuilder.select("*")
                .from("materials")
                .orderBy("id", true);
        }

        // 添加新材料
        void Add(Material &material) override
        {
            if (material.IsValid())
            {
                CommandBuilder cmdBuilder;
                cmdBuilder.insertInto("materials")
                    .value("name", material.Name())
                    .value("category", material.Category())
                    .value("sub_category", material.SubCategory())
                    .value("unit", material.Unit())
                    .value("specification", material.Specification());

                std::string sql = cmdBuilder.build();
                LOG(LogLevel::Debug) << "Insert SQL: " << sql;

                if (db_.execute(sql))
                {
                    int newId = db_.getLastInsertId();
                    material.SetId(newId);
                    LOG(LogLevel::Info) << "Material added with ID: " << newId;
                }
                else
                {
                    throw std::runtime_error("Failed to add material to database");
                }
            }
            else
            {
                throw std::invalid_argument("Invalid material data");
            }
        }

        // 更新材料
        void Update(const Material &material) override
        {
            if (!material.IsValid())
            {
                throw std::invalid_argument("Invalid material data for update");
            }

            CommandBuilder cmdBuilder;
            cmdBuilder.update("materials")
                .set("name", material.Name())
                .set("category", material.Category())
                .set("sub_category", material.SubCategory())
                .set("unit", material.Unit())
                .set("specification", material.Specification())
                .where("id = " + std::to_string(material.Id()));

            std::string sql = cmdBuilder.build();
            LOG(LogLevel::Debug) << "Update SQL: " << sql;
            if (!db_.execute(sql))
            {
                throw std::runtime_error("Failed to update material with ID: " + std::to_string(material.Id()));
            }
        }

        // 删除材料
        bool Remove(Material::IdType id) override
        {
            if (id <= 0)
            {
                LOG(LogLevel::Error) << "Invalid material ID for deletion: " << id;
                return false;
            }

            CommandBuilder cmdBuilder;
            cmdBuilder.deleteFrom("materials")
                .where("id = " + std::to_string(id));
            std::string sql = cmdBuilder.build();
            LOG(LogLevel::Debug) << "Delete SQL: " << sql;
            return db_.execute(sql);
        }

        // 根据名称模糊查询
        std::vector<Material> FindByNameLike(const std::string &nameKeyword) override
        {
            if (nameKeyword.empty())
                return {};
            QueryBuilder queryBuilder;
            queryBuilder.select("*")
                .from("materials")
                .where("name LIKE '%" + db_.escape(nameKeyword) + "%'")
                .orderBy("name", true);
        }

        // 根据分类查找
        std::vector<Material> FindByCategory(const std::string &category) override
        {
            if (category.empty())
                return {};
            QueryBuilder queryBuilder;
            queryBuilder.select("*")
                .from("materials")
                .whereEqual("category", category)
                .orderBy("name", true);
        }

        // 分页查找所有材料
        std::vector<Material> FindAllPaginated(int page, int pageSize) override
        {
            if (page <= 0 || pageSize <= 0)
                return {};
            QueryBuilder queryBuilder;
            queryBuilder.select("*")
                .from("materials")
                .orderBy("id", true)
                .page(page, pageSize);
        }

        // 检查名称-规格是否存在
        bool ExistsBy(const std::string &name, const std::string &specKeyword) override
        {
            if (name.empty())
                return false;

            QueryBuilder queryBuilder;
            queryBuilder.select("COUNT(*) as cnt")
                .from("materials")
                .whereEqual("name", name);

            if (!specKeyword.empty())
            {
                queryBuilder.where("specification LIKE '%" + db_.escape(specKeyword) + "%'");
            }

            std::string sql = queryBuilder.build();
            Json::Value result = db_.query(sql);

            if (result.size() > 0 && result[0].isMember("cnt"))
            {
                return std::stoi(result[0]["cnt"].asString()) > 0;
            }

            return false;
        }
    };

} // namespace Infrastructure::Persistence