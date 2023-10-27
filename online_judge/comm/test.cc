
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>

#if false
void handler(int signo)
{
    std::cout << "signo: " << signo << std::endl;
    exit(1);
}

int main()
{
    // 捕捉信号
    for (int i = 1; i <= 31; i++)
    {
        signal(i, handler);
    }
    // 限制运行时长
#if true
    struct rlimit r;
    r.rlim_cur = 1;
    r.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CPU, &r);
    while(1)
    {
        ;
    }
#endif
#if false
    // 限制内存大小
    struct rlimit r;
    r.rlim_cur = 1024 * 1024 * 20; // 20M
    r.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_AS, &r);
    int count = 0;
    while (true)
    {
        int *p = new int[1024 * 1024]; // 一次申请1mb
        std::cout << "size: " << count++ << std::endl;
        sleep(1);
    }
#endif
    return 0;
}
#endif

#if 0
#include <jsoncpp/json/json.h>
#include <string>

int main()
{
    // 序列化工作
    // Value 是一个Json的中间类，可以填充kv值
    Json::Value root;
    root["code"] = "mycode";
    root["user"] = "whb";
    root["age"] = "19";

    Json::StyledWriter writer;
    std::string str = writer.write(root);
    std::cout << str << std::endl;
    return 0;
}

#endif

// boost库
#if false
#include <boost/algorithm/string.hpp>
#include <vector>
int main()
{
    std::vector<std::string> tokens;
    const std::string str = "1:判断回文数:简单:1::::30000";
    boost::split(tokens, str, boost::is_any_of(":"), boost::algorithm::token_compress_on);
    /*
        boost::algorithm::token_compress_on 表示启用压缩
        就是如果两个分割符之间啥都没有 就直接丢弃
        如果是 off 就会保留两个分割符之间的空串 
    */
    for (const auto &it : tokens)
    {
        std::cout << it << std::endl;
    }
    return 0;
}
#endif

#include <ctemplate/template.h>

int main()
{
    std::string in_html = "./test.html";
    std::string value = "bitejiuyeke";

    // 形成数据字典
    ctemplate::TemplateDictionary root("test"); // 类似unordered_map
    root.SetValue("key", value);                // test.insert({})

    // 获取被渲染网页对象
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(in_html, ctemplate::DO_NOT_STRIP);

    // 添加字典数据到网页中
    std::string out_html;
    tpl->Expand(&out_html, &root);
    std::cout << out_html << std::endl;
    return 0;
}