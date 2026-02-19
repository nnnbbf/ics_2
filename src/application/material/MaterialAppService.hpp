#include "../../domain/material/IMaterialRepository.hpp"
#include "../../domain/material/Material.hpp"
#include "../../../shared/util/log.hpp"

#include <jsoncpp/json/json.h>
#include <memory>
#include <stdexcept>
#include <vector>

namespace Application::MaterialUseCases
{

    using namespace Domain::MaterialModule;
    class MaterialAppService : public IMaterialRepository
    {
    private:
        IMaterialRepository &materialRepo_;

    public:
        explicit MaterialAppService(IMaterialRepository &materialRepo) : materialRepo_(materialRepo_) {}

        // ============ 实现IMaterialRepository接口 ============

        // 查找所有材料
        std::vector<Material> FindAll() override
        {
            return materialRepo_.FindAll();
        }

        // 添加新材料
        void Add(Material &material) override
        {
            materialRepo_.Add(material);
        }

        // 更新材料
        void Update(const Material &material) override
        {
            materialRepo_.Update(material);
        }
        // 删除材料
        bool Remove(Material::IdType id) override
        {
            return materialRepo_.Remove(id);
        }
        // 根据名称模糊查询
        std::vector<Material> FindByNameLike(const std::string &nameKeyword) override
        {
            return materialRepo_.FindByNameLike(nameKeyword);
        }
        // 根据分类查找
        std::vector<Material> FindByCategory(const std::string &category) override
        {
            return materialRepo_.FindByCategory(category);
        }
        // 分页查找所有材料
        std::vector<Material> FindAllPaginated(int page, int pageSize) override
        {
            return materialRepo_.FindAllPaginated(page, pageSize);
        }
        // 检查名称-规格是否存在
        bool ExistsBy(const std::string &name, const std::string &specKeyword) override
        {
            return materialRepo_.ExistsBy(name, specKeyword);
        }
    };
}