#pragma once
// MySQL 版本
#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "include/mysql.h"

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

// model: 主要用来和数据进行交互，对外提供访问数据的接口

namespace ns_model
{
    using namespace std;
    using namespace ns_log;
    using namespace ns_util;

    class MySqlConnectionPool
    {
    public:
        // 初始化连接池
        MySqlConnectionPool(const std::string &host, const std::string &user, const std::string &passwd, const std::string &db, int port, int poolSize)
            : host_(host), user_(user), passwd_(passwd), db_(db), port_(port), poolSize_(poolSize)
        {
            // 创建 poolSize_ 个 MySQL 连接
            for (int i = 0; i < poolSize_; ++i)
            {
                MYSQL *conn = mysql_init(nullptr);
                if (conn == nullptr || mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0) == nullptr)
                {
                    LOG(FATAL) << "MySQL连接池初始化失败\n";
                    throw std::runtime_error("MySQL连接池初始化失败");
                }
                mysql_set_character_set(conn, "utf8"); // 设置字符集
                connectionPool_.push(conn);
            }
            LOG(INFO) << "MySQL连接池初始化成功\n";

        }

        // 析构函数，释放所有 MySQL 连接
        ~MySqlConnectionPool()
        {
            while (!connectionPool_.empty())
            {
                MYSQL *conn = connectionPool_.front();
                mysql_close(conn);
                connectionPool_.pop();
            }
        }

        // 获取一个可用的连接（如果没有可用连接，则等待）
        MYSQL *getConnection()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (connectionPool_.empty())
            {
                condVar_.wait(lock);
            }
            MYSQL *conn = connectionPool_.front();
            connectionPool_.pop();
            return conn;
        }

        // 归还一个连接到池子
        void returnConnection(MYSQL *conn)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            connectionPool_.push(conn);
            condVar_.notify_one();
        }

    private:
        std::string host_;
        std::string user_;
        std::string passwd_;
        std::string db_;
        int port_;
        int poolSize_;

        std::queue<MYSQL *> connectionPool_;
        std::mutex mutex_;
        std::condition_variable condVar_;
    };

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
    private:
        MySqlConnectionPool *connectionPool_; // 连接池对象指针
    public:
        // 在 Model 初始化时自动创建连接池
        Model(int port = 3306, int poolSize = 10)
        {
            connectionPool_ = new MySqlConnectionPool(host, user, passwd, db, port, poolSize);
        }

        // 释放连接池
        ~Model()
        {
            delete connectionPool_;
        }

    public:
        // 执行指定的 SQL 查询并将结果存储到输出参数中
        // 参数：
        //   sql - 要执行的 SQL 查询字符串
        //   out - 用于存储查询结果的 Problem 对象（输出型参数）
        bool QueryMySql(const std::string &sql, vector<Problem> *out)
        {
            // 从连接池中获取一个连接
            MYSQL *my = connectionPool_->getConnection();

            // 执行 SQL 查询
            if (0 != mysql_query(my, sql.c_str()))
            {
                LOG(WARNING) << sql << "执行失败\n";
                connectionPool_->returnConnection(my);
                return false;
            }

            // 获取查询结果
            MYSQL_RES *res = mysql_store_result(my);
            if (res == nullptr)
            {
                LOG(WARNING) << "查询结果为空\n";
                connectionPool_->returnConnection(my);
                return false;
            }

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

            mysql_free_result(res);
            connectionPool_->returnConnection(my);
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
