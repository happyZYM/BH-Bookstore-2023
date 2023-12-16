var SessionReadyEvent = new Event('SessionReady');
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
  document.dispatchEvent(SessionReadyEvent);
  console.log("Sent event Session Ready");
})();