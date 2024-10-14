#pragma once
// MySQL 版本
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "include/mysql.h"

#include <string>
#include <vector>

// model: 主要用来和数据进行交互，对外提供访问数据的接口

namespace ns_model
{
    using namespace std;
    using namespace ns_log;
    using namespace ns_util;

    // 定义题目的相关属性
    struct Problem
    {
        std::string id;    // 题目编号
        std::string title; // 题目标题
        std::string star;  // 难度：简单/中等/困难

        int cpu_limit; // 时间限制(S)
        int mem_limit; // 空间限制(KB)

        std::string desc;   // 题目描述
        std::string header; // 题目接口
        std::string tail;   // 题目测试用例和标准代码
    };

    const std::string db = "oj";
    const std::string oj_problems = "oj_problems";
    const std::string host = "127.0.0.1";
    const std::string user = "oj_admin";
    const std::string passwd = "Man9Oo.top";
    const int port = 3306;

    class Model
    {
    public:
        Model() {}
        ~Model() {}

    public:
        // 执行指定的 SQL 查询并将结果存储到输出参数中
        // 参数：
        //   sql - 要执行的 SQL 查询字符串
        //   out - 用于存储查询结果的 Problem 对象的 vector
        // 返回值：
        //   如果查询成功，返回 true；否则返回 false
        bool QueryMySql(const std::string &sql, vector<Problem> *out)
        {
            // 创建 MySQL 句柄（数据库连接对象）
            MYSQL *my = mysql_init(nullptr);
            // 尝试连接到 MySQL 数据库
            if (nullptr == mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0))
            {
                // 如果连接失败，记录错误日志并返回 false
                LOG(FATAL) << "连接数据库失败\n";
                return false;
            }
            // 设置连接的字符集为 UTF-8，以防止中文等字符出现乱码问题
            mysql_set_character_set(my, "utf8");

            // 记录成功连接数据库的日志
            LOG(INFO) << "连接数据库成功\n";

            // 执行 SQL 查询
            if (0 != mysql_query(my, sql.c_str()))
            {
                // 如果执行失败，记录警告日志并返回 false
                LOG(WARNING) << sql << "执行失败\n";
                return false;
            }

            // 获取查询结果
            MYSQL_RES *res = mysql_store_result(my);
            // 分析查询结果的行数
            int rows = mysql_num_rows(res);

            Problem p; // 用于存储每一行数据的 Problem 对象

            // 遍历每一行结果
            for (int i = 0; i < rows; i++)
            {
                // 获取当前行
                MYSQL_ROW row = mysql_fetch_row(res);

                // 提取每一列的值并存储到 Problem 对象中
                p.id = row[0];              // 题目编号
                p.title = row[1];           // 题目标题
                p.star = row[2];            // 题目难度
                p.desc = row[3];            // 题目描述
                p.header = row[4];          // 题目头文件
                p.tail = row[5];            // 题目测试代码
                p.cpu_limit = atoi(row[6]); // CPU 限制
                p.mem_limit = atoi(row[7]); // 内存限制

                // 添加到输出
                out->push_back(p);
            }

            free(res);
            mysql_close(my);
            return true;
        }

        // 获取所有题目列表，out 是输出型参数，用于存储查询结果
        bool GetAllProblems(vector<Problem> *out)
        {
            // 构建 SQL 查询语句，获取所有题目
            std::string sql = "select * from ";
            sql += oj_problems; // oj_problems 是题目表的名称

            // 执行查询
            return QueryMySql(sql, out);
        }

        // 获取题号 id 对应的题目，参数 p 是输出型参数，用于存储查询结果
        bool GetOneProblem(const std::string &id, Problem *p)
        {
            bool res = false; // 用于标记查询是否成功
            std::string sql = "select * from ";
            sql += oj_problems;  // oj_problems 是题目表的名称
            sql += " where id="; // 添加查询条件：根据题目 ID 查询
            sql += id;           // 将题目 ID 添加到 SQL 查询中

            vector<Problem> result; // 用于存储查询结果

            // 执行查询
            if (QueryMySql(sql, &result))
            {
                // 检查查询结果是否成功并且只返回一条记录
                if (result.size() == 1)
                {
                    *p = result[0];
                    res = true;
                }
            }

            return res;
        }
    };
} // namespace ns_model
