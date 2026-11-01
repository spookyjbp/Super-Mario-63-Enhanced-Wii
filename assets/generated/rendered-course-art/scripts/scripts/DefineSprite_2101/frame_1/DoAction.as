url2 = _root._url;
url = url2.split(".");
passed = "Originally obtained from Runouw.com";
i = 0;
while(i < url.length)
{
   trace(url[i].toLowerCase());
   if(url[i].toLowerCase() == "runouw")
   {
      passed = "Runouw.com Version.";
   }
   i++;
}
if(url2.charAt(8).toLowerCase() == "c")
{
   passed = "Running off of computer. Originally obtained from Runouw.com";
}
trace(passed);
