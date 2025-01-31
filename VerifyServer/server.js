const message_proto = require("./proto");
const grpc = require("@grpc/grpc-js")
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid')
const emaileMoudle = require('./email')

//call 相当于 proto 中的 req, callback 相当于 proto 中的 res
async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try {
        uniqueId = uuidv4();
        console.log("uuiqueId is ", uniqueId)
        let text_str = "您的验证码为" + uniqueId + "请三分钟内完成注册"
            //发送邮件
        let mailOptions = {
            from: "2549976860@qq.com",
            to: call.request.email,
            subject: "验证码",
            text: text_str,
        };

        //使用await等待sendmail完成
        let send_res = await emaileMoudle.SendMail(mailOptions);
        console.log("send res is ", send_res)

        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Success
        });
    } catch (error) {
        console.log("catch error is ", error)

        callback(null, {
            email: call.request.email,
            error: const_module.Errors.Exception
        });
    }
}

function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync("0.0.0.0:50051", grpc.ServerCredentials.createInsecure(), () => {
        console.log("grpc server started")
    })
}

main()