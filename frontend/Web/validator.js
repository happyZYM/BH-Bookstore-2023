const RPCClient = require("@alicloud/pop-core");

console.log(process.env['ALIBABA_CLOUD_ACCESS_KEY_ID'])
console.log(process.env['ALIBABA_CLOUD_ACCESS_KEY_SECRET'])


  // 注意，此处实例化的client请尽可能重复使用，避免重复建立连接，提升检测性能。
let client;
if(process.env['VALIDING']=='True'){
  client=new RPCClient({
      /**
  * 阿里云账号AccessKey拥有所有API的访问权限，建议您使用RAM用户进行API访问或日常运维。
  * 强烈建议不要把AccessKey ID和AccessKey Secret保存到工程代码里，否则可能导致AccessKey泄露，威胁您账号下所有资源的安全。
  * 常见获取环境变量方式：
  * 获取RAM用户AccessKey ID：process.env['ALIBABA_CLOUD_ACCESS_KEY_ID']
  * 获取RAM用户AccessKey Secret：process.env['ALIBABA_CLOUD_ACCESS_KEY_SECRET']
  */
      accessKeyId: process.env['ALIBABA_CLOUD_ACCESS_KEY_ID'],
      accessKeySecret: process.env['ALIBABA_CLOUD_ACCESS_KEY_SECRET'],
      // 接入区域和地址请根据实际情况修改
      endpoint: "https://green-cip.cn-beijing.aliyuncs.com",
      apiVersion: '2022-03-02',
      // 设置http代理
      // httpProxy: "http://xx.xx.xx.xx:xxxx",
      // 设置https代理
      // httpsProxy: "https://username:password@xxx.xxx.xxx.xxx:9999",
  });
}
async function IsValid(text) {
  console.log("IsValid called");
  console.log("cheching text: "+text);
  // 去除text中的空格、tab、无意义特殊符号（!@#$%^&*()_+_=~`{}|[]\;':",./<>?）等。
  //text = text.replace(/\s+/g, "");
  text = text.replace(/[!@#$%^&*()_+_=~`{}|[\]\\;':",./<>?]+/g, "");
  // 再去除换行和回车
  //text = text.replace(/[\r\n]/g, "");
  console.log("after replace: "+text);
  // 通过以下代码创建API请求并设置参数。
  const params = {
    // 文本检测service：内容安全控制台文本增强版规则配置的serviceCode，示例：chat_detection
    "Service": "chat_detection",
    "ServiceParameters": JSON.stringify({
      //待检测文本内容。
      "content": text
    }),
  };

  const requestOption = {
    method: 'POST',
    formatParams: false,
  };
  let response;
  try {
    // 调用接口获取检测结果。
    response = await client.request('TextModeration', params, requestOption);
    // 自动路由。
    if (response.Code === 500) {
      // 区域切换到cn-beijing。
      client.endpoint = "https://green-cip.cn-shanghai.aliyuncs.com";
      response = await client.request('TextModeration', params, requestOption);
    }
  } catch (err) {
    console.log(err);
  }
  if(response['Message']=='OK')
  {
    console.log(response['Data']['reason']);
    return response['Data']['reason']=="";
  }
  return false;
}

module.exports=IsValid;