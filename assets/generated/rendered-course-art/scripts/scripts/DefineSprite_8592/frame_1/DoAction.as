if(filenum == undefined)
{
   onEnterFrame = function()
   {
      gotoAndStop(_root.LDCourseBG + 1);
   };
}
else
{
   onEnterFrame = function()
   {
      gotoAndStop(_parent["file" + filenum + "BG"] + 1);
   };
}
