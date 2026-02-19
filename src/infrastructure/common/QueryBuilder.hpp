// infrastructure/common/SqlBuilder.hpp
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace Infrastructure::Common
{

    class QueryBuilder 
    {
    private:
        std::string select_;
        std::string from_;
        std::vector<std::string> whereConditions_;
        std::string orderBy_;
        std::string groupBy_;
        std::string having_;
        int limit_ = 0;
        int offset_ = 0;
        bool distinct_ = false;

    public:
        QueryBuilder () : select_("SELECT *") {}

        // ============ 链式构建方法 ============

        // SELECT
        QueryBuilder &select(const std::string &columns)
        {
            select_ = "SELECT " + columns;
            return *this;
        }

        QueryBuilder &select(const std::vector<std::string> &columns)
        {
            select_ = "SELECT ";
            for (size_t i = 0; i < columns.size(); ++i)
            {
                if (i > 0)
                    select_ += ", ";
                select_ += columns[i];
            }
            return *this;
        }

        QueryBuilder &distinct()
        {
            distinct_ = true;
            return *this;
        }

        // FROM
        QueryBuilder &from(const std::string &table)
        {
            from_ = " FROM " + table;
            return *this;
        }

        QueryBuilder &from(const std::string &table, const std::string &alias)
        {
            from_ = " FROM " + table + " AS " + alias;
            return *this;
        }

        // WHERE
        QueryBuilder &where(const std::string &condition)
        {
            if (!condition.empty())
            {
                whereConditions_.push_back(condition);
            }
            return *this;
        }

        QueryBuilder &whereEqual(const std::string &column, const std::string &value)
        {
            return where(column + " = '" + value + "'");
        }

        QueryBuilder &whereEqual(const std::string &column, int value)
        {
            return where(column + " = " + std::to_string(value));
        }

        QueryBuilder &whereLike(const std::string &column, const std::string &value)
        {
            if (!value.empty())
            {
                return where(column + " LIKE '%" + value + "%'");
            }
            return *this;
        }

        QueryBuilder &whereIn(const std::string &column, const std::vector<std::string> &values)
        {
            if (!values.empty())
            {
                std::string inClause = column + " IN (";
                for (size_t i = 0; i < values.size(); ++i)
                {
                    if (i > 0)
                        inClause += ", ";
                    inClause += "'" + values[i] + "'";
                }
                inClause += ")";
                return where(inClause);
            }
            return *this;
        }

        // ORDER BY
        QueryBuilder &orderBy(const std::string &column, bool descending = false)
        {
            orderBy_ = " ORDER BY " + column + (descending ? " DESC" : " ASC");
            return *this;
        }

        // GROUP BY
        QueryBuilder &groupBy(const std::string &column)
        {
            groupBy_ = " GROUP BY " + column;
            return *this;
        }

        // HAVING
        QueryBuilder &having(const std::string &condition)
        {
            having_ = " HAVING " + condition;
            return *this;
        }

        // LIMIT & OFFSET
        QueryBuilder &limit(int count)
        {
            limit_ = count;
            return *this;
        }

        QueryBuilder &offset(int offset)
        {
            offset_ = offset;
            return *this;
        }

        QueryBuilder &page(int pageNum, int pageSize)
        {
            if (pageNum > 0 && pageSize > 0)
            {
                offset_ = (pageNum - 1) * pageSize;
                limit_ = pageSize;
            }
            return *this;
        }

        // ============ 构建方法 ============

        std::string build() const
        {
            std::string sql;

            // SELECT
            if (distinct_ && select_.find("SELECT ") == 0)
            {
                sql = "SELECT DISTINCT " + select_.substr(7);
            }
            else
            {
                sql = select_;
            }

            // FROM
            sql += from_;

            // WHERE
            if (!whereConditions_.empty())
            {
                sql += " WHERE " + whereConditions_[0];
                for (size_t i = 1; i < whereConditions_.size(); ++i)
                {
                    sql += " AND " + whereConditions_[i];
                }
            }

            // GROUP BY
            if (!groupBy_.empty())
            {
                sql += groupBy_;
            }

            // HAVING
            if (!having_.empty())
            {
                sql += having_;
            }

            // ORDER BY
            if (!orderBy_.empty())
            {
                sql += orderBy_;
            }

            // LIMIT & OFFSET
            if (limit_ > 0)
            {
                sql += " LIMIT " + std::to_string(limit_);
                if (offset_ > 0)
                {
                    sql += " OFFSET " + std::to_string(offset_);
                }
            }

            return sql;
        }

        // ============ 辅助方法 ============

        // 构建COUNT查询
        std::string buildCount() const
        {
            std::string sql = "SELECT COUNT(*) as count";
            sql += from_;

            if (!whereConditions_.empty())
            {
                sql += " WHERE " + whereConditions_[0];
                for (size_t i = 1; i < whereConditions_.size(); ++i)
                {
                    sql += " AND " + whereConditions_[i];
                }
            }

            return sql;
        }

        // 重置
        QueryBuilder &reset()
        {
            select_ = "SELECT *";
            from_.clear();
            whereConditions_.clear();
            orderBy_.clear();
            groupBy_.clear();
            having_.clear();
            limit_ = 0;
            offset_ = 0;
            distinct_ = false;
            return *this;
        }
    };

} // namespace Infrastructure::Common