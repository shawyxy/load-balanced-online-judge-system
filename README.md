## 项目演示

首页：

![image-20241017202619397](https://raw.githubusercontent.com/xyshawy/IMG/main/MD/202410172029370.png)

题目列表：

![image-20241017202642255](https://raw.githubusercontent.com/xyshawy/IMG/main/MD/202410172029371.png)

题目详情：

![image-20241017202746869](https://raw.githubusercontent.com/xyshawy/IMG/main/MD/202410172029372.png)

判题：

![image-20241017202848298](https://raw.githubusercontent.com/xyshawy/IMG/main/MD/202410172029373.png)


## 项目文档

转至[负载均衡在线判题系统](./docs/负载均衡在线判题系统.md)

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
7. 在浏览器打开`localhost:8080`（请自行查阅物理机如何访问虚拟机的端口），如果使用云服务器，则使用服务器IP，另外MySQL的具体配置请参看项目文档。
8. 最后中根目录`/OnlineJudge`下使用`make clean`清理临时文件。