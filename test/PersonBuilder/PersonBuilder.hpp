#include <string>
#include <vector>
#include <memory>

class SqlBuilder
{
private:
    std::string select_;
    std::string from_;
    std::vector<std::string> whereConditions_;
    std::string orderBy_;
    std::string groupBy_;
    int limit_ = 0;
    int offset_ = 0;
    bool distinct_ = false;

public:
    // 构造函数
    SqlBuilder() : select_("SELECT *") {}

    // ============ SELECT 相关方法 ============

    // 选择所有列
    SqlBuilder &selectAll()
    {
        select_ = "SELECT *";
        distinct_ = false;
        return *this;
    }

    // 选择特定列
    SqlBuilder &select(const std::string &columns)
    {
        select_ = "SELECT " + columns;
        distinct_ = false;
        return *this;
    }

    // 选择多个列
    SqlBuilder &select(const std::vector<std::string> &columns)
    {
        select_ = "SELECT ";
        for (size_t i = 0; i < columns.size(); ++i)
        {
            if (i > 0)
                select_ += ", ";
            select_ += columns[i];
        }
        distinct_ = false;
        return *this;
    }

    // DISTINCT
    SqlBuilder &distinct()
    {
        distinct_ = true;
        return *this;
    }

    // ============ FROM 相关方法 ============

    SqlBuilder &from(const std::string &table)
    {
        from_ = " FROM " + table;
        return *this;
    }

    SqlBuilder &from(const std::string &table, const std::string &alias)
    {
        from_ = " FROM " + table + " AS " + alias;
        return *this;
    }

    // ============ WHERE 相关方法 ============

    // 基础WHERE条件
    SqlBuilder &where(const std::string &condition)
    {
        if (!condition.empty())
        {
            whereConditions_.push_back(condition);
        }
        return *this;
    }

    // 相等条件
    SqlBuilder &whereEqual(const std::string &column, const std::string &value)
    {
        whereConditions_.push_back(column + " = '" + value + "'");
        return *this;
    }

    SqlBuilder &whereEqual(const std::string &column, int value)
    {
        whereConditions_.push_back(column + " = " + std::to_string(value));
        return *this;
    }

    SqlBuilder &whereEqual(const std::string &column, double value)
    {
        whereConditions_.push_back(column + " = " + std::to_string(value));
        return *this;
    }

    // LIKE条件
    SqlBuilder &whereLike(const std::string &column, const std::string &value)
    {
        if (!value.empty())
        {
            whereConditions_.push_back(column + " LIKE '%" + value + "%'");
        }
        return *this;
    }

    // IN条件
    SqlBuilder &whereIn(const std::string &column, const std::vector<std::string> &values)
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
            whereConditions_.push_back(inClause);
        }
        return *this;
    }

    // BETWEEN条件
    SqlBuilder &whereBetween(const std::string &column,
                             const std::string &start,
                             const std::string &end)
    {
        whereConditions_.push_back(column + " BETWEEN '" + start + "' AND '" + end + "'");
        return *this;
    }

    // ============ ORDER BY 相关方法 ============

    SqlBuilder &orderBy(const std::string &column, bool descending = false)
    {
        orderBy_ = " ORDER BY " + column + (descending ? " DESC" : " ASC");
        return *this;
    }

    // ============ GROUP BY 相关方法 ============

    SqlBuilder &groupBy(const std::string &column)
    {
        groupBy_ = " GROUP BY " + column;
        return *this;
    }

    // ============ LIMIT 相关方法 ============

    SqlBuilder &limit(int count)
    {
        limit_ = count;
        return *this;
    }

    SqlBuilder &limit(int offset, int count)
    {
        offset_ = offset;
        limit_ = count;
        return *this;
    }

    SqlBuilder &page(int pageNum, int pageSize)
    {
        if (pageNum > 0 && pageSize > 0)
        {
            offset_ = (pageNum - 1) * pageSize;
            limit_ = pageSize;
        }
        return *this;
    }

    // ============ 构建方法 ============

    std::string build()
    {
        std::string sql;
        // 构建SELECT子句
        if (distinct_ && select_.find("SELECT ") == 0)
        {
            sql = "SELECT DISTINCT " + select_.substr(7);
        }
        else
        {
            sql = select_;
        }

        // 添加FROM
        sql += from_;

        // 添加WHERE
        if (!whereConditions_.empty())
        {
            sql += " WHERE " + whereConditions_[0];
            for (size_t i = 1; i < whereConditions_.size(); ++i)
            {
                sql += " AND " + whereConditions_[i];
            }
        }

        // 添加GROUP BY
        if (!groupBy_.empty())
        {
            sql += groupBy_;
        }

        // 添加ORDER BY
        if (!orderBy_.empty())
        {
            sql += orderBy_;
        }

        // 添加LIMIT
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

    // 重置Builder
    SqlBuilder &reset()
    {
        select_ = "SELECT *";
        from_.clear();
        whereConditions_.clear();
        orderBy_.clear();
        groupBy_.clear();
        limit_ = 0;
        offset_ = 0;
        distinct_ = false;
        return *this;
    }

    // 获取当前状态（用于调试）
    std::string toString() const
    {
        std::string state = "SqlBuilder State:\n";
        state += "  SELECT: " + select_ + "\n";
        state += "  FROM: " + from_ + "\n";
        state += "  WHERE conditions: " + std::to_string(whereConditions_.size()) + "\n";
        for (const auto &cond : whereConditions_)
        {
            state += "    - " + cond + "\n";
        }
        state += "  ORDER BY: " + orderBy_ + "\n";
        state += "  GROUP BY: " + groupBy_ + "\n";
        state += "  LIMIT: " + std::to_string(limit_) + "\n";
        state += "  OFFSET: " + std::to_string(offset_) + "\n";
        state += "  DISTINCT: " + std::string(distinct_ ? "true" : "false") + "\n";
        return state;
    }
};