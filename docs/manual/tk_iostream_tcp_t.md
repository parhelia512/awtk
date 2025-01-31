## tk\_iostream\_tcp\_t
### 概述
![image](images/tk_iostream_tcp_t_0.png)

基于TCP实现的输入输出流。
----------------------------------
### 函数
<p id="tk_iostream_tcp_t_methods">

| 函数名称 | 说明 | 
| -------- | ------------ | 
| <a href="#tk_iostream_tcp_t_tk_iostream_tcp_create">tk\_iostream\_tcp\_create</a> | 创建iostream对象。 |
| <a href="#tk_iostream_tcp_t_tk_iostream_tcp_create_client">tk\_iostream\_tcp\_create\_client</a> | 创建iostream对象(客户端)。 |
| <a href="#tk_iostream_tcp_t_tk_iostream_tcp_create_client_ex">tk\_iostream\_tcp\_create\_client\_ex</a> | 创建iostream对象(客户端)。 |
#### tk\_iostream\_tcp\_create 函数
-----------------------

* 函数功能：

> <p id="tk_iostream_tcp_t_tk_iostream_tcp_create">创建iostream对象。

* 函数原型：

```
tk_iostream_t* tk_iostream_tcp_create (int sock);
```

* 参数说明：

| 参数 | 类型 | 说明 |
| -------- | ----- | --------- |
| 返回值 | tk\_iostream\_t* | 返回iostream对象。 |
| sock | int | socket。 |
#### tk\_iostream\_tcp\_create\_client 函数
-----------------------

* 函数功能：

> <p id="tk_iostream_tcp_t_tk_iostream_tcp_create_client">创建iostream对象(客户端)。

* 函数原型：

```
tk_iostream_t* tk_iostream_tcp_create_client (const char* host, int port);
```

* 参数说明：

| 参数 | 类型 | 说明 |
| -------- | ----- | --------- |
| 返回值 | tk\_iostream\_t* | 返回iostream对象。 |
| host | const char* | 服务器地址。 |
| port | int | 服务器端口。 |
#### tk\_iostream\_tcp\_create\_client\_ex 函数
-----------------------

* 函数功能：

> <p id="tk_iostream_tcp_t_tk_iostream_tcp_create_client_ex">创建iostream对象(客户端)。

* 函数原型：

```
tk_iostream_t* tk_iostream_tcp_create_client_ex (const char* host, int port, int timeout, void* opts);
```

* 参数说明：

| 参数 | 类型 | 说明 |
| -------- | ----- | --------- |
| 返回值 | tk\_iostream\_t* | 返回iostream对象。 |
| host | const char* | 服务器地址。 |
| port | int | 服务器端口。 |
| timeout | int | 连接超时（毫秒）设置，设置为0代表按系统默认超时。 |
| opts | void* | 保留参数设置，当前只接受NULL值。 |
