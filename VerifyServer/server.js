const message_proto = require("./proto");
const grpc = require("@grpc/grpc-js")
const const_module = require('./const')
const { v4: uuidv4 } = require('uuid')
const emaileMoudle = require('./email')
const redis_module = require("./redis")

//call 相当于 proto 中的 req, callback 相当于 proto 中的 res
async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try {
        //先查询是否给这个用户发送了验证码
        let query_res = await redis_module.GetRedis(const_module.code_prefix+call.request.email);
        let uniqueId = query_res;
        if(query_res == null){
            //没有生成验证码，所以生成
            uniqueId = uuidv4();
            if(uniqueId.length > 4){
                uniqueId = uniqueId.substring(0, 4);
            }
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix+call.request.email, uniqueId, 180);
            if(!bres){
                callback(null, {
                    email: call.request.email,
                    error:const_module.Errors.RedisErr
                });
                return;
            }
        }
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
        if(!send_res){
            callback(null, {
                email: call.request.email,
                error:const_module.Errors.RedisErr
            });
            return;
        }
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