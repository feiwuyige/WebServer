const config_module = require("./config.json");
const Redis = require("ioredis");

//创建redis客户端实例
const RedisCli = new Redis({
    host: config_module.redis.host,
    port: config_module.redis.port,
    password: config_module.redis.passwd,
});

//监听错误信息，出错时调用回调
RedisCli.on("error", function(err){
    console.log("RedisCli connect error");
    RedisCli.quit();
});

//根据 key 值获取 value
async function GetRedis(key) {
    try{
        //get 返回的是一个 promise，类似于 cpp 中的future，不代表该操作执行完毕
        //要等待，所以要使用 await 阻塞
        //要使用 await 要将函数声明为 async
        const result = await RedisCli.get(key);
        if(result == null){
            console.log('result:','<'+result+'>', 'This key cannot be find...');
            return null;
        }
        console.log('Result:','<'+result+'>','Get key success!...');
        return result;
    }
    catch(error){
        console.log('GetRedis error is', error);
        return null;
    }
}

//查询 key 值是否存在
async function QueryRedis(key) {
    try{
        const result = await RedisCli.exists(key)
        //  判断该值是否为空 如果为空返回null
        if (result === 0) {
          console.log('result:<','<'+result+'>','This key is null...');
          return null;
        }
        console.log('Result:','<'+result+'>','With this value!...');
        return result;
    }
    catch(error){
        console.log('QueryRedis error is', error);
        return null;
    }
}

//设置 key value 以及过期时间
async function SetRedisExpire(key, value, exptime){
    try{
        // 设置键和值
        await RedisCli.set(key,value)
        // 设置过期时间（以秒为单位）
        await RedisCli.expire(key, exptime);
        return true;
    }
    catch(error){
        console.log('SetRedisExpire error is', error);
        return false;
    }
}

//退出
function Quit(){
    RedisCli.quit();
}

module.exports = {GetRedis, QueryRedis, Quit, SetRedisExpire}