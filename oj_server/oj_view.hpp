#pragma once

#include <iostream>
#include <string>
#include <ctemplate/template.h>

#include "oj_model.hpp"

namespace ns_view
{
    using namespace ns_model;

    // 模板路径：指定了存放 HTML 模板文件的目录
    const std::string template_path = "./template_html/";

    class View
    {
    public:
        View() {}
        ~View() {}

    public:
        // 渲染题目列表
        // 参数:
        //     problems：一个包含所有题目信息的列表
        //                 每个 Problem 结构体包含题目的编号、标题、难度等信息
        //     html：输出参数，存放生成的 HTML 代码
        void AllExpandHtml(const vector<struct Problem> &problems, std::string *html)
        {
            // 1. 形成模板路径
            std::string src_html = template_path + "all_problems.html";
            // 2. 形成模板路径
            // 创建一个字典对象 root，用于存储题目信息
            ctemplate::TemplateDictionary root("all_problems");
            // 每个题目信息会被加入到 question_list，这部分在模板文件中会重复渲染
            for (const auto &p : problems)
            {
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("puestion_list");
                sub->SetValue("id", p.id);       // 题目的编号
                sub->SetValue("title", p.title); // 标题
                sub->SetValue("star", p.star);   // 难度
            }

            // 3. 加载被渲染的html模板
            // 从文件中加载模板，这里使用的是 all_questions.html 模板
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 4. 渲染模板
            // 用数据字典 root 渲染模板，将生成的 HTML 存入 html 字符串
            tpl->Expand(html, &root);
        }

        // 渲染单个题目的详细页面
        void OneExpandHtml(const struct Problem &p, std::string *html)
        {
            // 1. 形成模板路径
            std::string src_html = template_path + "one_puestion.html";

            // 2. 形成模板路径
            ctemplate::TemplateDictionary root("one_puestion");
            root.SetValue("id", p.id);
            root.SetValue("title", p.title);
            root.SetValue("star", p.star);
            root.SetValue("desc", p.desc);
            root.SetValue("pre_code", p.header);

            // 3. 加载被渲染的html模板
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 4. 渲染模板
            tpl->Expand(html, &root);
        }
    };
} // namespace ns_view