#pragma once
#include "../repository/MaterialRepository.hpp"
#include "../model/MaterialModel.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class MaterialService
{
private:
    MaterialRepository repository;
public:
    MaterialService():repository(Database::getInstance()){}
    //判断材料是否存在
    
    MaterialModel createMaterial(MaterialModel& material)
    {

        MaterialModel newmaterial = material;
        //判断材料是否存在
        if (repository.save(newmaterial) == -1)
        {
            LOG(LogLevel::Error) << "Failed to save new user: " << newmaterial.getName();
            return MaterialModel();//创建用户失败
        }
        return newmaterial;
    }

    std::vector<MaterialModel> getAllMaterials()
    {
        return repository.findAll();
    }

    bool    deleteMaterial(int id)
    {
        return repository.remove(id);
    }

    bool updateMaterial(const MaterialModel& material)
    {
        return repository.update(material);
    }
};
