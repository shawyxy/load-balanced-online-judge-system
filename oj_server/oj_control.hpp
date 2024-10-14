#pragma once

#include <iostream>
#include <string>
#include <mutex>
#include <vector>
#include <cassert>
#include <fstream>
#include <jsoncpp/json/json.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "../comm/httplib.h"
#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_control
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;
    using namespace ns_view;
    using namespace httplib;

    // 提供服务的主机
    class Machine
    {
    public:
        std::string _ip;  // 编译服务主机IP
        int _port;        // 编译服务端口
        uint64_t _load;   // 编译服务的负载（需要保证一致性）
        std::mutex *_mtx; // 互斥锁的地址
    public:
        Machine() : _ip(""), _port(0), _load(0), _mtx(nullptr)
        {
        }
        ~Machine() {}

    public:
        // 增加主机的负载
        void IncLoad()
        {
            if (_mtx)
                _mtx->lock();
            ++_load;
            if (_mtx)
                _mtx->unlock();
        }
        // 减少主机的负载
        void DecLoad()
        {
            if (_mtx)
                _mtx->lock();
            --_load;
            if (_mtx)
                _mtx->unlock();
        }
        // 重置主机的负载
        void ResetLoad()
        {
            if (_mtx)
                _mtx->lock();
            _load = 0;
            if (_mtx)
                _mtx->unlock();
        }
        // 获取当前的负载值
        uint64_t Load()
        {
            uint64_t load_value  = 0;
            if (_mtx)
                _mtx->lock();
            load_value  = _load;
            if (_mtx)
                _mtx->unlock();

            return load_value ;
        }
    };

    // 提供服务的主机列表
    const std::string service_machine = "./conf/service_machine.conf";

    // 负载均衡模块
    class LoadBalance
    {
    private:
        std::vector<Machine> _machines; // 提供编译服务的所有的主机，下标对应主机编号id
        std::vector<int> _online;       // 在线主机id
        std::vector<int> _offline;      // 离线主机id
        std::mutex _mtx;                // 保证LoadBlance的数据一致性

    public:
        LoadBalance()
        {
            assert(LoadConf(service_machine));
            LOG(INFO) << "主机[" << service_machine << "]加载成功\n";
        }
        ~LoadBalance()
        {
        }

    public:
        // 加载主机集群配置文件
        bool LoadConf(const std::string &machine_conf)
        {
            // 打开配置文件
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG(FATAL) << " 加载：" << machine_conf << " 失败\n";
                return false;
            }
            // 按行读取文件
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens;
                // 提取IP和端口
                StringUtil::SplitString(line, &tokens, ":");
                if (tokens.size() != 2)
                {
                    LOG(WARNING) << " 切分 " << line << " 失败\n";
                    continue;
                }
                // 构建主机对象
                Machine machine;
                machine._ip = tokens[0];
                machine._port = atoi(tokens[1].c_str());
                machine._load = 0;
                machine._mtx = new std::mutex();
                // 将主机加入到负载均衡模块的主机列表和在线主机的ID列表中
                _online.push_back(_machines.size());
                _machines.push_back(machine);
            }

            in.close();
            return true;
        }

        // 负载均衡选择：遍历所有在线的机器列表，找出负载最小的机器
        // 输出型参数：
        //     id：被选择的主机ID
        //     machine：被选择的主机的地址
        bool SelectLeastLoaded(int *id, Machine **machine)
        {
            _mtx.lock();

            int online_num = _online.size();
            if (online_num == 0)
            {
                _mtx.unlock();
                LOG(FATAL) << "：后端所有编译主机全部离线！\n";
                return false;
            }
            // 遍历主机列表，选择负载最小的主机
            *id = _online[0];
            *machine = &_machines[_online[0]];
            uint64_t min_load = _machines[_online[0]].Load();
            for (int i = 1; i < online_num; i++)
            {
                uint64_t curr_load = _machines[_online[i]].Load();
                if (min_load > curr_load) // 找到更小负载，更新返回值
                {
                    min_load = curr_load;
                    *id = _online[i];
                    *machine = &_machines[_online[i]];
                }
            }

            _mtx.unlock();
            return true;
        }

        // 主机上下线管理
        void OfflineMachine(int id)
        {
            _mtx.lock();
            // 从在线列表中找到要离线的主机的ID
            for (auto it = _online.begin(); it != _online.end(); it++)
            {
                if (*it == id)
                {
                    _machines[id].ResetLoad(); // 重置负载
                    _online.erase(it);         // 从在线列表中移除
                    _offline.push_back(id);    // 加入到离线列表
                    break;
                }
            }
            _mtx.unlock();
        }
        // 当在线列表为空时，将离线列表中的所有主机加入到在线列表
        void OnlineMachine()
        {
            _mtx.lock();
            _online.insert(_online.end(), _offline.begin(), _offline.end());
            _offline.erase(_offline.begin(), _offline.end());
            _mtx.unlock();

            LOG(INFO) << "所有主机已上线\n";
        }

        // for test
        void ShowMachines()
        {
            _mtx.lock();
            std::cout << "当前在线主机列表：";
            for (auto &id : _online)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            std::cout << "当前离线主机列表：";
            for (auto &id : _offline)
            {
                std::cout << id << " ";
            }
            std::cout << "\n";
            _mtx.unlock();
        }
    };

    // 核心业务逻辑的控制器
    class Control
    {
    private:
        // 用于从后台数据获取题目相关的信息
        Model _model;
        // 用于将获取到的数据渲染为 HTML 页面
        View _view;
        // 用于负载均衡式地判题
        LoadBalance _load_balance;

    public:
        // 获取所有题目的信息，并生成一个显示所有题目的网页
        // /template_html/all_problems.html
        bool AllProblems(std::string *html) // 输出型参数
        {
            bool ret = true;
            std::vector<struct Problem> all;
            if (this->_model.GetAllProblems(&all))
            {
                // 根据题号排序
                sort(all.begin(), all.end(), [](const struct Problem &x, const struct Problem &y)
                     { return atoi(x.id.c_str()) < atoi(y.id.c_str()); });
                // 获取题目信息成功，将所有题目数据构建成网页
                _view.AllExpandHtml(all, html);
            }
            else
            {
                *html = "获取网页失败，获取题目列表失败";
                ret = false;
            }
            return ret;
        }

        // 获取单个题目的详细信息，并生成该题目的 HTML 页面
        // /template_html/one_problems.html
        bool Problem(const std::string &id, std::string *html)
        {
            bool ret = true;
            struct Problem p;
            if (_model.GetOneProblem(id, &p))
            {
                // 获取指定题目成功
                _view.OneExpandHtml(p, html);
            }
            else
            {
                *html = "指定题目，编号：" + id + "不存在";
                ret = false;
            }
            return ret;
        }

        // 负载均衡式判题
        // 参数：
        //     id：题号
        //     in_json：用户提交代码和输入
        // 返回值：
        //     out_json：「编译与运行服务」的结果
        void Judge(const std::string &id, const std::string in_json, std::string *out_json)
        {
            // 0. 根据题号id获取详细信息
            struct Problem p;
            _model.GetOneProblem(id, &p);

            // 1. 反序列化：提取用户提交的代码和输入数据
            Json::Reader reader;
            Json::Value in_value;
            reader.parse(in_json, in_value);
            std::string code = in_value["code"].asString();

            // 2. 重新拼接用户代码和题目测试代码
            Json::Value compile_value;
            compile_value["input"] = in_value["input"].asString();
            compile_value["code"] = code + "\n" + p.tail; // 用户提交代码+题目测试代码
            compile_value["cpu_limit"] = p.cpu_limit;
            compile_value["mem_limit"] = p.mem_limit;
            Json::FastWriter writer;
            std::string compile_string = writer.write(compile_value);

            // 3. 负载均衡选择主机
            while (true)
            {
                int machine_id = 0;         // 主机编号
                Machine *machine = nullptr; // 主机信息
                if (!_load_balance.SelectLeastLoaded(&machine_id, &machine))
                {
                    break; // 所有主机都不可用时，退出循环
                }

                // 4. 请求编译和运行服务
                // 用选择的主机IP和端口构建Client对象
                Client cli(machine->_ip, machine->_port);
                // 该主机负载增加
                machine->IncLoad();
                LOG(INFO) << "：选择主机成功：[" << machine_id << "][" << machine->_ip << ":" << machine->_port << "]，当前负载：" << machine->Load() << "\n";
                // 发起 HTTP 请求，检查主机是否正常响应
                if (auto res = cli.Post("/compile_and_run" /*请求的服务*/, compile_string /*请求的参数*/, "application/json;charset=utf-8" /*请求的数据类型*/))
                {
                    if (res->status == 200) // 请求成功
                    {
                        *out_json = res->body; // 将请求包含的数据作为JSON字符串返回（输出型参数）
                        machine->DecLoad();    // 请求成功，减少主机负载
                        break; // 成功完成任务，退出循环
                    }
                    else
                    {
                        // 请求失败，减少主机负载，重新选择其他主机
                        machine->DecLoad();
                    }
                }
                else // 如果请求失败，标记主机离线并选择其他主机（没有收到cli.Post的响应）
                {
                    LOG(ERROR) << "状态码："<< res->status << "：无法连接到主机[" << machine->_ip << ":" << machine->_port << "]，可能已离线\n";
                    _load_balance.OfflineMachine(machine_id); // 离线这台主机
                    _load_balance.ShowMachines();             // for test
                }
            }
        }

        // 当所有主机离线则重新上线
        void RecoveryMachine()
        {
            _load_balance.OnlineMachine();
        }
    };
} // namespace ns_control