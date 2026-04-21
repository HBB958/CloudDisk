# CloudDisk 云盘系统

> 基于 Qt C++ 开发的轻量级云盘服务端，支持多客户端并发连接、文件上传下载等功能。

## 功能特点

- 支持多客户端并发连接（线程池 / I/O 多路复用）
- 文件分片上传
- 用户认证与权限管理
- 数据库持久化存储（MySQL）

## 技术栈

- **开发框架**：Qt 4.11.1
- **编程语言**：C++ 
- **数据库**：MySQL
- **网络通信**：Qt Network（QTcpServer / QTcpSocket）
- **并发处理**：QThreadPool

## 项目结构

├── src/
│   ├── core/          # 核心业务逻辑（服务器主类、客户端处理）
│   ├── network/       # 网络通信层（TCP 服务器、协议解析）
│   ├── database/      # 数据库操作层（连接池）
│   ├── models/        # 数据模型（User、FileInfo 实体类）
│   └── utils/         # 工具类（配置读取、日志记录）
├── ui/                # 服务端管理界面（Qt Designer 生成的 .ui 文件）
├── resources/         # 资源文件（配置文件、图标、启动脚本）
├── CloudDiskServer.pro # Qt 工程文件
└── README.md          # 项目说明文档