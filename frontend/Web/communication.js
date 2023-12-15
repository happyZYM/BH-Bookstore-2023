function generateRandomString(length) {
  let result = '';
  const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const charactersLength = characters.length;

  for (let i = 0; i < length; i++) {
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
  }

  return result;
}
const socket = io();
let __raw_response="";
let operation_count=localStorage.getItem("operation_count");
let session_token=localStorage.getItem("session_token");
let outhentication_key=localStorage.getItem("outhentication_key");
socket.on('response', (msg) => {
  __raw_response=msg;
});
function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}
async function RawRequest(raw_request){
  socket.emit('request', raw_request);
  while(true)
  {
    if(__raw_response!=""){
      let response=__raw_response;
      __raw_response="";
      return response;
    }
    await sleep(100);
  }
}
async function ShutDownWholeSystem()
{
  await RawRequest("#ShutDownSystem "+session_token+" "+operation_count+" "+outhentication_key);
  return "You Guess";
}
async function Request(req)
{
  operation_count=localStorage.getItem("operation_count");
  session_token=localStorage.getItem("session_token");
  outhentication_key=localStorage.getItem("outhentication_key");
  operation_count++;
  localStorage.setItem("operation_count", operation_count);
  let raw_request="#Request "+session_token+" "+operation_count+" "+outhentication_key+"\n"+req;
  let raw_response=await RawRequest(raw_request);
  console.log("get raw_response:"+raw_response);
  let line_numbers_of_res=parseInt(raw_response.split('\n')[0].split(' ')[2]);
  if(line_numbers_of_res==-1)
  {
    console.log("Session expired");
    let tmp_channel=generateRandomString(10);
    let ret=await RawRequest('#OpenSession '+tmp_channel);
    operation_count=0;
    session_token=ret.split('\n')[1].split(' ')[0];
    outhentication_key=ret.split('\n')[1].split(' ')[1];
    localStorage.setItem("session_token", session_token);
    localStorage.setItem("outhentication_key", outhentication_key);
    localStorage.setItem("operation_count", operation_count);
    return await Request(req);
  }
  let response="";
  if(line_numbers_of_res>=1) response=raw_response.split('\n')[1];
  for(let i=0;i<line_numbers_of_res-1;i++){
    response+="\n"+raw_response.split('\n')[i+2];
  }
  localStorage.setItem("operation_count", operation_count);
  return response;
}
(async () => {
  if(session_token==null||outhentication_key==null||operation_count==null){
    let tmp_channel=generateRandomString(10);
    let ret=await RawRequest('#OpenSession '+tmp_channel);
    operation_count=0;
    session_token=ret.split('\n')[1].split(' ')[0];
    outhentication_key=ret.split('\n')[1].split(' ')[1];
    localStorage.setItem("session_token", session_token);
    localStorage.setItem("outhentication_key", outhentication_key);
    localStorage.setItem("operation_count", operation_count);
  }
})();