#pragma once
// 文件版本
#include "../comm/util.hpp"
#include "../comm/log.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <cassert>

// 根据problems.list文件，加载所有的题目信息到内存中
// model: 主要用来和数据进行交互，对外提供访问数据的接口

namespace ns_model
{
    using namespace std;
    using namespace ns_log;
    using namespace ns_util;

    // 定义题目的相关属性
    struct Problem
    {
        std::string id; // 题目编号
        std::string title;  // 题目标题
        std::string star;   // 难度：简单/中等/困难
        
        int cpu_limit;      // 时间限制(S)
        int mem_limit;      // 空间限制(KB)

        std::string desc;   // 题目描述
        std::string header; // 题目接口
        std::string tail;   // 题目测试用例和标准代码
    };

    const std::string problems_list = "./problems/problems.list";
    const std::string problems_path = "./problems/";

    class Model
    {
    private:
        // <题号:题目所有信息>
        unordered_map<string, Problem> problems;

    public:
        Model()
        {
            // 加载所有的题目信息到内存中的哈希表中
            assert(LoadProblemList(problems_list));
        }

        bool LoadProblemList(const string &problem_list)
        {
            // 加载配置文件: problems/problems.list + 题目编号文件
            ifstream in(problem_list);
            if (!in.is_open())
            {
                LOG(FATAL) << "：加载题库失败，请检查是否存在题库文件\n";
                return false;
            }

            string line;
            while (getline(in, line))
            {
                vector<string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                // 切割字符串，获取题目属性字段
                // 1 两数之和 简单 1 262144
                if (tokens.size() != 5)
                {
                    LOG(WARNING) << "：加载部分题目失败，请检查文件格式\n";
                    continue;
                }
                // 将属性字段设置到题目对象p中
                Problem p;
                p.id = tokens[0];
                p.title = tokens[1];
                p.star = tokens[2];
                p.cpu_limit = atoi(tokens[3].c_str()); // 数字转字符串
                p.mem_limit = atoi(tokens[4].c_str());

                string path = problems_path;
                path += p.id;
                path += "/";
                // 在题号对应的路径下读取描述和代码文件：./problems/[题号]/
                // 读取的内容被填充到题目对象p中（输出型参数）
                FileUtil::ReadFile(path + "desc.txt", &(p.desc), true);
                FileUtil::ReadFile(path + "header.cpp", &(p.header), true);
                FileUtil::ReadFile(path + "tail.cpp", &(p.tail), true);
                // 将题号和题目对象p插入到哈希表中
                problems.insert({p.id, p});
            }
            LOG(INFO) << "：加载题库...成功\n";
            in.close();

            return true;
        }

        // 获取题目列表，out是输出型参数
        bool GetAllProblems(vector<Problem> *out)
        {
            if (problems.size() == 0)
            {
                LOG(ERROR) << "：用户获取题库失败\n";
                return false;
            }
            // 遍历哈希表
            for (const auto &p : problems)
            {
                out->push_back(p.second);
            }

            return true;
        }

        // 获取题号id对应的题目，参数p是输出型参数
        bool GetOneProblem(const std::string &id, Problem *p)
        {
            const auto &it = problems.find(id);
            if (it == problems.end())
            {
                LOG(ERROR) << "：用户获取题目失败，题目编号：" << id << "\n";
                return false;
            }
            (*p) = it->second;
            return true;
        }
        ~Model()
        {
        }
    };
} // namespace ns_model
