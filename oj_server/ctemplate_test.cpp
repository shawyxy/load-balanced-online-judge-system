#include <iostream>
#include <ctemplate/template.h>

int main()
{
    // 1. 形成数据字典
    std::string value = "这是被替换的内容";
    ctemplate::TemplateDictionary root("test");
    root.SetValue("key", value);

    // 2. 形成被渲染网页对象
    std::string in_html = "./test.html";
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(in_html, ctemplate::DO_NOT_STRIP);

    // 3. 添加字典到网页中
    std::string out_html;
    tpl->Expand(&out_html, &root);

    // 4. 完成渲染
    std::cout << out_html << std::endl;

    return 0;
}
