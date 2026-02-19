// domain/person/IPersonRepository.hpp
#pragma once
#include <vector>
#include <string>
#include "Person.hpp"

namespace Domain::PersonModule {

    class IPersonRepository {
    public:
        virtual ~IPersonRepository() = default;

        virtual Person FindById(Person::IdType id) = 0;
        virtual Person FindByCardNumber(const std::string& cardNumber) = 0;
        virtual std::vector<Person> FindByName(const std::string& name) = 0;
        virtual std::vector<Person> FindAll() = 0;
        virtual bool ExistsByCardNumber(const std::string& cardNumber) = 0;
        virtual void Add(Person& person) = 0;      // 新建，自增ID

        virtual void Update(const Person& person) = 0;
        virtual bool Remove(Person::IdType id) = 0;
    };

} 
