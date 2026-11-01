if(doortype == undefined)
{
   doortype = 1;
}
if(locked == undefined)
{
   locked = false;
}
if(keytype == undefined)
{
   keytype = "none";
}
if(removelock == undefined)
{
   removelock = false;
}
if(locked == true)
{
   if(removelock == true)
   {
      gotoAndStop(3);
   }
   else
   {
      gotoAndStop(2);
   }
}
else
{
   gotoAndStop(4);
   onEnterFrame = null;
}
