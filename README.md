## 使用方法

1. 安装所有依赖（在文档中 ctrl+F 搜索关键字）：
   - make
   - Boost库
   - cpp-httplib
   - jsoncpp
   - 安装并升级g++（不要忽略）
   - ctemplate
   - MySQL（Linux）
   - MySQL 开发包（非必要，MySQL默认会安装）
   - MySQL Workbench（Windows）
2. 进入项目根目录`/OnlineJudge`
3. `make`
4. `make output`
5. 进入`output`目录
6. 分别进入：
   - `compile_server`目录：执行`./compile_server 8081`。
   - `oj_server`目录：执行`./oj_server`。
   - 其中`compile_server`的端口在配置文件`OnlineJudge/oj_server/conf/service_machine.conf`中指定。
7. 在浏览器打开`localhost:8080`。
8. 最后中根目录`/OnlineJudge`下使用`make clean`清理临时文件。