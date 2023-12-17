function showDropdown() {
  document.querySelector('.dropdown-content').style.display = 'block';
}

function hideDropdown() {
  document.querySelector('.dropdown-content').style.display = 'none';
}
function ChangeUsername(newUsername) {
  document.getElementById('username').textContent = newUsername;
}
async function UpdateUserInfo(){
  let nm=await GetMyName();
  let pri=await GetMyPrivilege();
  if(nm=='')
  {
    await RefreshSession();
    nm=await GetMyName();
    pri=await GetMyPrivilege();
  }
  if(pri==0)
  {
    ChangeUsername("[Guest]");
  }
  else if(pri==1)
  {
    ChangeUsername(nm);
  }
  else if(pri==3)
  {
    ChangeUsername(nm+" [Worker]");
  }
  else if(pri==7)
  {
    ChangeUsername(nm+" [Admin]");
  }
}