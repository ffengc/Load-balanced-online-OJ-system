

#ifndef __YUFC_OJ_CONTROL_HPP__
#define __YUFC_OJ_CONTROL_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <jsoncpp/json/json.h>

#include "./oj_model.hpp"
#include "./oj_view.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "../comm/httplib.h"

#define __FOR_TEST__ true

namespace ns_control
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;
    using namespace ns_view;

    /* class Machine 是提供服务的主机 */
    class Machine
    {
    public:
        std::string __ip;  // 编译服务的ip
        int __port;        // 编译服务的port
        uint64_t __load;   // 当前编译服务的负载大小
        std::mutex *__mtx; // C++的mutex是禁止拷贝的，所以要定义成指针
    public:
        Machine() : __ip(""), __port(0), __load(0), __mtx(nullptr) {}
        ~Machine() {}

    public:
        // +负载
        void IncLoad()
        {
            if (__mtx)
                __mtx->lock();
            __load++;
            if (__mtx)
                __mtx->unlock();
        }
        // -负载
        void DecLoad()
        {
            if (__mtx)
                __mtx->lock();
            __load--;
            if (__mtx)
                __mtx->unlock();
        }
        // 获取主机负载
        uint64_t GetLoad()
        {
            uint64_t cur_load = 0;
            if (__mtx)
                __mtx->lock();
            cur_load = this->__load;
            if (__mtx)
                __mtx->unlock();
            return cur_load;
        }
        // 重置负载
        void ResetLoad()
        {
            if (__mtx)
                __mtx->lock();
            __load = 0;
            if (__mtx)
                __mtx->unlock();
        }
    };

    /* 负载均衡模块 */
    const std::string service_machine_path = "./conf/service_machine.conf";
    class LoadBalance
    {
    private:
        // 每一台主机都有自己的下标，我们用下标充当主机的id
        std::vector<Machine> __machines; // 所有的主机
        std::vector<int> __online;       // 在线主机的id
        std::vector<int> __offline;      // 离线主机id
        std::mutex __mtx;

    public:
        LoadBalance()
        {
            assert(this->LoadConf(service_machine_path));
            LOG(INFO) << "加载: " << service_machine_path << " 成功!"
                      << "\n";
        }
        ~LoadBalance() {}

    public:
        bool LoadConf(const std::string &machine_conf)
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG(FATAL) << "加载: " << machine_conf << "失败"
                           << "\n";
                return false;
            }
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, ":");
                if (tokens.size() != 2)
                {
                    LOG(WARNING) << "切分" << line << " 失败"
                                 << "\n";
                    continue;
                }
                Machine m;
                m.__ip = tokens[0];
                m.__port = atoi(tokens[1].c_str());
                m.__load = 0;
                m.__mtx = new std::mutex();
                __online.push_back(__machines.size()); // 主机都是启动的
                __machines.push_back(m);               // 放到主机列表中去
            }
            in.close();
            return true;
        }
        // 智能选择
        bool IntelligentSelect(int *id, Machine **m)
        {
            // 1. 使用选择好的主机(更新主机的负载)
            // 2. 我们可能离线该主机
            /*
                id: 输出参数
                m: 选择好的主机
            */
            __mtx.lock();
            // 负载均衡的算法
            // 1. 随机数法
            // 2. 轮询+hash
            int online_num = __online.size();
            if (online_num == 0)
            {
                __mtx.unlock();
                LOG(FATAL) << "所有后端编译主机已经全部离线，请运维同时尽快查看"
                           << "\n";
                return false;
            }
            // 初始化要返回的东西
            *id = __online[0];
            *m = &__machines[__online[0]];
            uint64_t min_load = __machines[__online[0]].GetLoad();
            for (int i = 0; i < online_num; i++)
            {
                // 通过遍历的方式，找到所有负载最小的机器
                uint64_t temp_load = __machines[__online[i]].GetLoad();
                if (min_load > temp_load)
                {
                    min_load = temp_load; // 更新负载信息
                    *id = __online[i];
                    *m = &__machines[__online[i]];
                }
            }
            __mtx.unlock();
            return true;
        }
        void OfflineMachine(int which)
        {
            __mtx.lock();

            for (auto iter = __online.begin(); iter != __online.end(); iter++)
            {
                if (*iter == which)
                {
                    __machines[which].ResetLoad();
                    // 要离线的主机已经找到了
                    __online.erase(iter);
                    __offline.push_back(which);
                    break; // 因为break，所以我们暂时不考虑迭代器失效的问题
                }
            }
            __mtx.unlock();
        }
        void OnlineMachine()
        {
            // 当所有主机都离线的时候，我们统一上线
            // 后面统一解决
            __mtx.lock();
            __online.insert(__online.end(), __offline.begin(), __offline.end());
            __offline.erase(__offline.begin(), __offline.end());
            __mtx.unlock();
            LOG(INFO) << "所有的主机上线啦！"
                      << "\n";
        }
#if __FOR_TEST__
        // for test
        // 看看在线的离线和分别有哪些
        void ShowMachinesStatus()
        {
            __mtx.lock();
            std::cout << "当前在线主机列表: ";
            for (auto &id : __online)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            std::cout << "当前离线主机列表: ";
            for (auto &id : __offline)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            __mtx.unlock();
        }
#endif
    };

    /* class Control 是核心业务逻辑*/
    class Control
    {
    private:
        Model __model;
        View __view;
        LoadBalance __load_balance; // 负载均衡器
    public:
        void RecoveryMachine()
        {
            __load_balance.OnlineMachine();
        }

    public:
        // 根据题目数据构建网页 html是输出型参数
        bool AllQuestions(std::string *html)
        {
            bool ret = true;
            std::vector<struct Question> all;
            if (this->__model.GetAllQuestions(&all))
            {
                // 对vector排序
                std::sort(all.begin(), all.end(), [](const struct Question &q1, const struct Question &q2)
                          { return atoi(q1.__number.c_str()) < atoi(q2.__number.c_str()); });
                // 获取题目信息成功，将所有题目数据构建成网页
                __view.AllExpandHtml(all, html);
            }
            else
            {
                *html = "获取网页失败，获取题目列表失败";
                ret = false;
            }
            return ret;
        }
        bool Question(const std::string &number, std::string *html)
        {
            bool ret = true;
            struct Question q;
            if (__model.GetOneQuestions(number, &q))
            {
                // 获取指定题目成功
                __view.OneExpandHtml(q, html);
            }
            else
            {
                *html = "指定题目, 编号: " + number + " 不存在";
                ret = false;
            }
            return ret;
        }

        void Judge(const std::string &number, const std::string in_json, std::string *out_json)
        {
            /*
                in_json包含这些东西:
                # 1. code: #include ...
                # 2. input: ""
            */
            // 0. 根据题目编号，直接拿到对应的题目细节
            struct Question q;
            __model.GetOneQuestions(number, &q);
            // 1. 反序列化 in_json
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value);
            std::string code = in_value["code"].asString();
            // 2. 重新拼接用户代码+测试用例代码，形成新的代码
            Json::Value compile_value; // 将来要将它发到cr上的
            compile_value["input"] = in_value["input"].asString();
            compile_value["code"] = code + q.__tail; // 拼接代码！
            compile_value["cpu_limit"] = q.__cpu_limit;
            compile_value["mem_limit"] = q.__mem_limit;
            Json::FastWriter writer;
            std::string compile_str = writer.write(compile_value); // compile_str就是准备发送给cr的
            // 3. 选择负载最低的主机，然后发起http请求，得到结果
            // 规则: 一直选择，直到主机可用，否则，就是全部挂掉
            while (true)
            {
                int id = 0;
                Machine *m = nullptr;
                if (!__load_balance.IntelligentSelect(&id, &m))
                {
                    break; // IntelligentSelect 里面有日志了
                }
                // 发起请求
                httplib::Client cli(m->__ip, m->__port);
                m->IncLoad(); // 增加这台主机的负载
                LOG(INFO) << "选择主机成功, 主机id: " << id << "详情: "
                          << m->__ip << ":" << m->__port << " 当前主机负载是: "
                          << m->GetLoad() << "\n";
                if (auto res = cli.Post("/compile_and_run", compile_str, "application/json;charset=utf-8"))
                {
                    // res 其实就是 Result 类型
                    if (res->status == 200)
                    {
                        *out_json = res->body;
                        m->DecLoad(); // 请求完毕，减少负载
                        LOG(INFO) << "请求cr服务成功 ... "
                                  << "\n";
                        break;
                    }
                    m->DecLoad();
                }
                else
                {
                    // 请求失败
                    LOG(INFO) << "当前请求的主机id: " << id << "详情: "
                              << m->__ip << ":" << m->__port << " 可能已经离线"
                              << "\n";                 // 没必要
                    __load_balance.OfflineMachine(id); // 离线这台机器(这里面也会将负载清0的)
#if __FOR_TEST__
                    __load_balance.ShowMachinesStatus();
#endif
                }
            }
        }
    };
} // namespace ns_control

#endif