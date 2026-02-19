#pragma once
#include "../../../database/Database.hpp"
#include "../../../shared/util/log.hpp"
#include "../../domain/person/IPersonRepository.hpp"

namespace Infrastructure::Persistence
{

    using namespace Domain::PersonModule;

    class PersonRepository : public IPersonRepository
    {
    private:
        Database &db_;

    public:
        explicit PersonRepository(Database &db)
            : db_(db) {}

        Person FindById(Person::IdType id) override
        {
            Person p;
            if (id <= 0)
                return p;
            std::string sql = "SELECT * FROM persons WHERE id = " + std::to_string(id);
            Json::Value result = db_.query(sql);
            if (result.size() > 0)
            {
                p = Person::fromJson(result[0]);
            }
            return p;
        }

        Person FindByCardNumber(const std::string &cardNumber) override
        {
            Person p;
            if (cardNumber.empty())
                return p;
            std::string sql = "SELECT * FROM persons WHERE card_number = '" + db_.escape(cardNumber) + "'";
            Json::Value result = db_.query(sql);
            if (result.size() > 0)
            {
                p = Person::fromJson(result[0]);
            }   
            return p;   
        }

        std::vector<Person> FindByName(const std::string &name) override
        {
            std::vector<Person> list;
            if (name.empty())
                return list;
            std::string sql = "SELECT * FROM persons WHERE name LIKE '%" + db_.escape(name) + "%'";
            Json::Value result = db_.query(sql);
            for (const auto &item : result)
            {
                list.push_back(Person::fromJson(item));
            }
            return list;
        }

        std::vector<Person> FindAll() override
        {
            std::vector<Person> list;
            std::string sql = "SELECT * FROM persons ORDER BY id";
            Json::Value result = db_.query(sql);
            for (const auto &item : result)
            {
                list.push_back(Person::fromJson(item));
            }
            return list;
        }
        bool ExistsByCardNumber(const std::string &cardNumber) override
        {
            if (cardNumber.empty())
                return false;
            std::string sql = "SELECT COUNT(*) AS count FROM persons WHERE card_number = '" + db_.escape(cardNumber) + "'";
            Json::Value result = db_.query(sql);
            if (result.size() > 0)
            {
                int count = std::stoi(result[0]["count"].asString());
                return count > 0;
            }
            return false;
        }
       

        void Add(Person &person) override
        {
            std::string sql = "INSERT INTO persons (name, card_number, balance, status, department, work_center) VALUES ('" +
                              db_.escape(person.Name()) + "', '" +
                              db_.escape(person.CardNumber()) + "', '" +
                              std::to_string(person.Balance()) + "', '" +
                              db_.escape(person.Status()) + "', '" +
                              db_.escape(person.Department()) + "', '" +
                              db_.escape(person.WorkCenter()) + "')";
            if (db_.execute(sql))
            {
                int newId = db_.getLastInsertId();
                person.SetId(newId);
            }
        }

        void Update(const Person &person) override
        {
            if (!person.IsValid())
            {
                throw std::runtime_error("Invalid person id");
            }
            std::string sql = "UPDATE persons SET "
                              "name = '" + db_.escape(person.Name()) + "', "
                              "card_number = '" + db_.escape(person.CardNumber()) + "', "
                              "balance = " + std::to_string(person.Balance()) + ", "
                              "status = '" + db_.escape(person.Status()) + "', "
                              "department = '" + db_.escape(person.Department()) + "', "
                              "work_center = '" + db_.escape(person.WorkCenter()) + "' "
                              "WHERE id = " + std::to_string(person.Id());
            db_.execute(sql);
        }

        bool Remove(Person::IdType id) override
        {
            if (id <= 0)
            {
                throw std::runtime_error("Invalid person id");
            }
            std::string sql = "DELETE FROM persons WHERE id = " + std::to_string(id);
            return db_.execute(sql);
        }
    };

} // namespace Infrastructure::Persistence
