/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

#ifndef __YUFC_OJ_CONTROL_HPP__
#define __YUFC_OJ_CONTROL_HPP__

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <mutex>
#include <string>
#include <vector>

#include "../comm/httplib.h"
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "./oj_model.hpp"
#include "./oj_view.hpp"

#define __FOR_TEST__ true

namespace ns_control {
using namespace ns_log;
using namespace ns_util;
using namespace ns_model;
using namespace ns_view;

/* class Machine is the host that provides the service */
class Machine {
public:
    std::string __ip; // Compile service ip
    int __port; // Compile service port
    uint64_t __load; // The current load size of the compilation service
    std::mutex* __mtx; // C++ mutex is prohibited from copying, so it must be defined as a pointer
public:
    Machine()
        : __ip("")
        , __port(0)
        , __load(0)
        , __mtx(nullptr) { }
    ~Machine() { } //
public:
    // add load
    void IncLoad() {
        if (__mtx)
            __mtx->lock();
        __load++;
        if (__mtx)
            __mtx->unlock();
    }
    // decrease load
    void DecLoad() {
        if (__mtx)
            __mtx->lock();
        __load--;
        if (__mtx)
            __mtx->unlock();
    }
    // Get host load
    uint64_t GetLoad() {
        uint64_t cur_load = 0;
        if (__mtx)
            __mtx->lock();
        cur_load = this->__load;
        if (__mtx)
            __mtx->unlock();
        return cur_load;
    }
    // Reset Load
    void ResetLoad() {
        if (__mtx)
            __mtx->lock();
        __load = 0;
        if (__mtx)
            __mtx->unlock();
    }
};

/* Load balancing module */
const std::string service_machine_path = "./conf/service_machine.conf";
class LoadBalance {
private:
    // Each host has its own subscript, which we use as the host id
    std::vector<Machine> __machines; // all host
    std::vector<int> __online; // The ID of the online host
    std::vector<int> __offline; // Offline host id
    std::mutex __mtx;

public:
    LoadBalance() {
        assert(this->LoadConf(service_machine_path));
        LOG(INFO) << "Load: " << service_machine_path << " success" << "\n";
    }
    ~LoadBalance() { } //
public:
    bool LoadConf(const std::string& machine_conf) {
        std::ifstream in(machine_conf);
        if (!in.is_open()) {
            LOG(FATAL) << "Load: " << machine_conf << " failed" << "\n";
            return false;
        }
        std::string line;
        while (std::getline(in, line)) {
            std::vector<std::string> tokens;
            StringUtil::SplitString(line, &tokens, ":");
            if (tokens.size() != 2) {
                LOG(WARNING) << "split string: " << line << " failed" << "\n";
                continue;
            }
            Machine m;
            m.__ip = tokens[0];
            m.__port = atoi(tokens[1].c_str());
            m.__load = 0;
            m.__mtx = new std::mutex();
            __online.push_back(__machines.size()); // The hosts are all started
            __machines.push_back(m); // Add to host list
        }
        in.close();
        return true;
    }
    // Smart Selection
    bool IntelligentSelect(int* id, Machine** m) {
        // 1. Use the selected host (update the host's load)
        // 2. We may take this host offline
        /*
            id: output parameter
            m: select a good host
        */
        __mtx.lock();
        // Load balancing algorithm
        // 1. Random number method
        // 2. Polling + hash
        int online_num = __online.size();
        if (online_num == 0) {
            __mtx.unlock();
            LOG(FATAL) << "All backend compilation hosts are offline. Please check as soon as possible."
                       << "\n";
            return false;
        }
        // Initialize what to return
        *id = __online[0];
        *m = &__machines[__online[0]];
        uint64_t min_load = __machines[__online[0]].GetLoad();
        for (int i = 0; i < online_num; i++) {
            // By traversing, find all machines with the smallest load
            uint64_t temp_load = __machines[__online[i]].GetLoad();
            if (min_load > temp_load) {
                min_load = temp_load; // Update load information
                *id = __online[i];
                *m = &__machines[__online[i]];
            }
        }
        __mtx.unlock();
        return true;
    }
    void OfflineMachine(int which) {
        __mtx.lock();

        for (auto iter = __online.begin(); iter != __online.end(); iter++) {
            if (*iter == which) {
                __machines[which].ResetLoad();
                // The host to be offline has been found
                __online.erase(iter);
                __offline.push_back(which);
                break; // Because of break, we will not consider the problem of iterator invalidation for the time being
            }
        }
        __mtx.unlock();
    }
    void OnlineMachine() {
        // When all hosts are offline, we go online together
        // Solve the problem together later
        __mtx.lock();
        __online.insert(__online.end(), __offline.begin(), __offline.end());
        __offline.erase(__offline.begin(), __offline.end());
        __mtx.unlock();
        LOG(INFO) << "All hosts are online" << std::endl;
    }
#if __FOR_TEST__
    // for test
    // See what are the online and offline
    void ShowMachinesStatus() {
        __mtx.lock();
        std::cout << "List of currently online hosts: ";
        for (auto& id : __online) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
        std::cout << "List of currently offline hosts: ";
        for (auto& id : __offline) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
        __mtx.unlock();
    }
#endif
};

/* class Control is the core business logic */
class Control {
private:
    Model __model;
    View __view;
    LoadBalance __load_balance; // Load Balancer
public:
    void RecoveryMachine() {
        __load_balance.OnlineMachine();
    } //
public:
    // Build a web page based on the topic data. HTML is an output parameter.
    bool AllQuestions(std::string* html) {
        bool ret = true;
        std::vector<struct Question> all;
        if (this->__model.GetAllQuestions(&all)) {
            // Sorting a vector
            std::sort(all.begin(), all.end(), [](const struct Question& q1, const struct Question& q2) { return atoi(q1.__number.c_str()) < atoi(q2.__number.c_str()); });
            // The question information is obtained successfully, and all the question data is constructed into a web page
            __view.AllExpandHtml(all, html);
        } else {
            *html = "Failed to obtain the web page, failed to obtain the topic list";
            ret = false;
        }
        return ret;
    }
    bool Question(const std::string& number, std::string* html) {
        bool ret = true;
        struct Question q;
        if (__model.GetOneQuestions(number, &q)) {
            // Successfully obtained the specified topic
            __view.OneExpandHtml(q, html);
        } else {
            *html = "Assign title, number: " + number + " does not exist";
            ret = false;
        }
        return ret;
    }

    void Judge(const std::string& number, const std::string in_json, std::string* out_json) {
        /*
            in_json:
            # 1. code: #include ...
            # 2. input: ""
        */
        // 0. According to the question number, directly get the corresponding question details
        struct Question q;
        __model.GetOneQuestions(number, &q);
        // 1. Deserialize in_json
        Json::Value in_value;
        Json::Reader reader;
        reader.parse(in_json, in_value);
        std::string code = in_value["code"].asString();
        // 2. Reassemble user code + test case code to form new code
        Json::Value compile_value; // I will post it on cr in the future.
        compile_value["input"] = in_value["input"].asString();
        compile_value["code"] = code + q.__tail; // Stitch the code!
        compile_value["cpu_limit"] = q.__cpu_limit;
        compile_value["mem_limit"] = q.__mem_limit;
        Json::FastWriter writer;
        std::string compile_str = writer.write(compile_value); // compile_str is ready to be sent to cr
        // 3. Select the host with the lowest load, then initiate an http request to get the result
        // Rule: Keep selecting until the host is available, otherwise, all will be shut down.
        while (true) {
            int id = 0;
            Machine* m = nullptr;
            if (!__load_balance.IntelligentSelect(&id, &m))
                break;
            // Making a request
            httplib::Client cli(m->__ip, m->__port);
            m->IncLoad(); // Increase the load on this host
            LOG(INFO) << "Select host successfully, host id: " << id << " detail: "
                      << m->__ip << ":" << m->__port << " The current host load is: "
                      << m->GetLoad() << "\n";
            if (auto res = cli.Post("/compile_and_run", compile_str, "application/json;charset=utf-8")) {
                // res is actually the Result type
                if (res->status == 200) {
                    *out_json = res->body;
                    m->DecLoad(); // Request completed, reduce load
                    LOG(INFO) << "Request cr service successfully ... " << std::endl;
                    break;
                }
                m->DecLoad();
            } else {
                // 请求失败
                LOG(INFO) << "The host id of the current request: " << id << "detail: "
                          << m->__ip << ":" << m->__port << " May be offline"
                          << "\n"; // No need
                __load_balance.OfflineMachine(id); // Offline this machine (the load will also be cleared to 0 here)
#if __FOR_TEST__
                __load_balance.ShowMachinesStatus();
#endif
            }
        }
    }
};
} // namespace ns_control

#endif