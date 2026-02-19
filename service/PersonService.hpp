#pragma once
#include "../repository/PersonRepository.hpp"
#include "../model/UserMode.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <vector>

class PersonService {
private:
    PersonRepository repository;
    
public:
    PersonService() : repository(Database::getInstance()) {}
    
    // 查询
    PersonModel getPersonById(int id)
    {
        return repository.findById(id);
    }
    std::vector<PersonModel> getAllPersons(const std::string& keyword = "")
    {
        if (keyword.empty()) {
            return repository.findAll();
        } else {
            return repository.findByName(keyword);
        }
    }
    PersonModel getPersonByCardNumber(const std::string& cardNumber)
    {
        return repository.findByCardNumber(cardNumber);
    }
    
    // 业务操作
    PersonModel createPerson(const PersonModel& person)
    {
        PersonModel newPerson = person;
        LOG(LogLevel::Debug) << "Creating person with card number: " << person.getCardNumber();
        if(repository.existsByCardNumber(person.getCardNumber())) {
            LOG(LogLevel::Error) << "Card number already exists: " << person.getCardNumber();
            return PersonModel();
        }

        if(repository.save(newPerson) == -1)
        {
            LOG(LogLevel::Error) << "Failed to save new person with card number: " << person.getCardNumber();
            return PersonModel();
        }
        return newPerson;
    }
    bool updatePerson(const PersonModel& person)
    {
        return repository.update(person);
    }
    bool deletePerson(int id)
    {
        return repository.remove(id);
    }
    
    // 财务操作
    PersonModel topupBalance(int personId, double amount)
    {
        PersonModel person = repository.findById(personId); 
        person.topup(amount);
        repository.update(person);
        return person;
    }
    PersonModel deductBalance(int personId, double amount)
    {
        PersonModel person = repository.findById(personId); 
        if (person.canDeduct(amount)) {
            person.deduct(amount);
            repository.update(person);
        }
        return person;
    }
    
    // 状态管理
    bool freezePerson(int personId)
    {
        PersonModel person = repository.findById(personId);
        person.setStatus("禁用");
        return repository.update(person);
    }
    bool activatePerson(int personId)
    {
        PersonModel person = repository.findById(personId);
        person.setStatus("正常");
        return repository.update(person);  
    }
};