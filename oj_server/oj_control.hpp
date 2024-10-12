#pragma once

#include <iostream>
#include <string>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_control
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;
    using namespace ns_view;

    class Control
    {
    private:
        // 用于从后台数据获取题目相关的信息
        Model _model;
        // 用于将获取到的数据渲染为 HTML 页面
        View _view;

    public:
        // 获取所有题目的信息，并生成一个显示所有题目的网页
        // /template_html/all_problems.html
        bool AllProblems(std::string *html) // 输出型参数
        {
            bool ret = true;
            std::vector<struct Problem> all;
            if (this->_model.GetAllProblems(&all))
            {
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
        bool Problem(const std::string &number, std::string *html)
        {
            bool ret = true;
            struct Problem p;
            if (_model.GetOneProblem(number, &p))
            {
                // 获取指定题目成功
                _view.OneExpandHtml(p, html);
            }
            else
            {
                *html = "指定题目，编号：" + number + "不存在";
                ret = false;
            }
            return ret;
        }
    };
} // namespace ns_control