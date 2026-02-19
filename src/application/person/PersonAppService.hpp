// application/person/PersonAppService.hpp
#pragma once
#include "../../domain/person/IPersonRepository.hpp"

namespace Application::PersonUseCases {

    using namespace Domain::PersonModule;

    class PersonAppService {
    private:
        IPersonRepository& personRepo_;

    public:
        explicit PersonAppService(IPersonRepository& repo)
            : personRepo_(repo) {}

        Person GetById(Person::IdType id) {
            return personRepo_.FindById(id);
        }

        std::vector<Person> GetAll(const std::string& keyword = "") {
            if (keyword.empty()) return personRepo_.FindAll();
            return personRepo_.FindByName(keyword);
        }

        Person GetByCardNumber(const std::string& cardNumber) {
            return personRepo_.FindByCardNumber(cardNumber);
        }

        Person Create(const std::string& name,
                      const std::string& cardNumber,
                      const std::string& department,
                      const std::string& workCenter)
        {
            if (personRepo_.ExistsByCardNumber(cardNumber)) {
                return Person();
            }

            Person person = Person::CreateNew(name, cardNumber, department, workCenter);
            personRepo_.Add(person);
            return person;
        }

        bool Update(const Person& person) {
            personRepo_.Update(person);
        }

        bool Delete(Person::IdType id) {
            return personRepo_.Remove(id);
        }
    };

} // namespace Application::PersonUseCases
