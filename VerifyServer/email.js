const nodemailer = require("nodemailer")
const config_module = require("./config")

//创建发送邮箱的代理
let transport = nodemailer.createTransport({
    host: "smtp.qq.com",
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user,
        pass: config_module.email_pass
    }
});

//发送邮件的函数
function SendMail(mailOptions_) {
    //promise 相当于cpp 中的future，将一个异步转化为同步阻塞
    return new Promise(function(resolve, reject) {
        transport.sendMail(mailOptions_, function(error, info) {
            if (error) {
                console.log(error);
                reject(error);
            } else {
                console.log("成功发送邮件：" + info.response);
                resolve(info.response)
            }
        })
    })
}

module.exports.SendMail = SendMail