# ICMP 数据包发送与接收程序

该项目包含两个C语言编写的程序，用于演示如何在Linux系统上发送和接收ICMP（Internet控制消息协议）数据包。ICMP协议通常用于网络设备之间发送错误信息和操作信息。

## 文件说明:

1. **send_icmp.c**：发送ICMP数据包（如ping请求）到指定目标的程序。
2. **recev_icmp.c**：监听并接收ICMP数据包的程序，并打印其详细信息。

### 前置条件:

- 一个Linux系统（需要管理员/root权限来创建原始套接字）。
- GCC或类似的C语言编译器。
- 需要`sudo`或root权限来运行使用原始套接字的程序。

### 编译方法:

使用以下命令编译两个程序：

```
gcc -o send_icmp send_icmp.c
gcc -o listen_icmp listen_icmp.c
```

### 使用说明:

#### 发送ICMP数据包

要发送ICMP数据包，请运行以下命令：

```
sudo ./send_icmp <目标IP> <类型> <标识符> <序列号>
```

- `<目标IP>`：目标机器的IP地址。
- `<类型>`：ICMP数据包类型（例如，8表示Echo Request，即ping请求）。
- `<标识符>`：数据包的唯一标识符。
- `<序列号>`：数据包的序列号。

示例：

```
sudo ./send_icmp 192.168.1.1 8 12345 1
```

此命令将向`192.168.1.1`发送一个标识符为`12345`、序列号为`1`的ping请求（ICMP Echo Request）。

#### 接收ICMP数据包

要监听ICMP数据包，请运行以下命令：

```
sudo ./listen_icmp
```

该程序将监听接收到的ICMP数据包，并打印每个收到的数据包的信息，包括源IP、ICMP标识符、序列号以及数据包的十六进制转储。

### 注意事项:

- 由于程序创建的是原始套接字，运行时需要管理员权限。请使用`sudo`或以root用户身份执行。
- 请确保防火墙规则允许ICMP流量，否则可能无法接收到ICMP数据包。
- 有些网络环境可能严格按照echo 和 reply的方式允许icmp流量，并且时间间隔很短，这种情况请注意关闭接收方icmp响应。

### icmp数据包类型(参考)：

| 类型 | 名称                        | 描述                                                         |
| ---- | --------------------------- | ------------------------------------------------------------ |
| 0    | **Echo Reply**              | 回显应答（ping响应），用于响应Echo Request数据包。           |
| 3    | **Destination Unreachable** | 目标不可达，当目标主机或网络不可到达时发送此信息。           |
| 4    | **Source Quench**           | 源端抑制，用于告诉发送方减少发送速率（已废弃）。             |
| 5    | **Redirect**                | 重定向，用于告诉发送方有更好的路由路径。                     |
| 8    | **Echo Request**            | 回显请求（ping请求），用于测试目标主机的可达性。             |
| 9    | **Router Advertisement**    | 路由器广告，用于通告路由器存在的信息。                       |
| 10   | **Router Solicitation**     | 路由器请求，主机用来请求路由器发送Router Advertisement数据包。 |
| 11   | **Time Exceeded**           | 超时时间到达，用于通知数据包在网络中的生存时间（TTL）已过期。 |
| 12   | **Parameter Problem**       | 参数问题，用于报告数据包头部的某个字段出现问题。             |
| 13   | **Timestamp Request**       | 时间戳请求，用于请求时间戳信息。                             |
| 14   | **Timestamp Reply**         | 时间戳应答，响应时间戳请求。                                 |
| 15   | **Information Request**     | 信息请求，用于请求某些网络信息（已废弃）。                   |
| 16   | **Information Reply**       | 信息应答，响应Information Request（已废弃）。                |
| 17   | **Address Mask Request**    | 地址掩码请求，用于请求子网掩码。                             |
| 18   | **Address Mask Reply**      | 地址掩码应答，响应Address Mask Request。                     |
| 30   | **Traceroute**              | 路由跟踪，主要用于Traceroute工具，测试网络路径。             |