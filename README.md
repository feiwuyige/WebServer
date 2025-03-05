# WebServer
使用C++ 编写的全栈即时通讯项目，前端基于QT实现仿微信布局，后端采用分布式设计，分为GateServer 网关服务，多个ChatServer聊天服务，StatusServer 状态服务以及 VerifyServer验证服务。 支持添加好友、好友通信、聊天记录展示等功能。

## 用户登录注册界面

该界面支持密码隐藏和展示功能，同时可以跳转至注册界面以及修改密码界面。

<center class="half">
    <img src="./img/login_pass.jpg" width="200"/>
    <img src="./img/login_nopass.jpg" width="200"/>
    <img src="./img/register.jpg" width="200"/>
    <img src="./img/forgetpassword.jpg" width="200" />
</center>

## 聊天界面

用户点击登录按钮后进入聊天界面，此时后端会根据服务器的负载将用户登录到连接数比较少的服务器上。

聊天界面展示：
<center class="half">
    <img src="./img/chat.jpg" width="300"/>
    <img src="./img/user.jpg" width="300"/>
    <img src="./img/contact.jpg" width="300"/>
</center>

多用户登录时服务器日志展示：

<center class="half">
    <img src="./img/server.jpg" Height = "400" width="800"/>
</center>

## 添加好友功能

在搜索栏中搜索好友用户名，搜到后弹出添加好友用户界面：
<center class="half">
    <img src="./img/search.jpg"  width="400"/>
    <img src="./img/add.jpg"  width="400"/>
</center>

用户登陆后自动加载申请列表，同意后联系人列表显示对方相关信息：

<center class="half">
    <img src="./img/apply.jpg"  width="300"/>
    <img src="./img/addsucc.jpg"  width="300"/>
    <img src="./img/addsuccess.jpg"  width="300"/>
</center>

## 聊天功能

支持文本消息发送

<center class="half">
    <img src="./img/zyl.jpg"  width="400"/>
    <img src="./img/ckx.jpg"  width="400"/>
</center>
