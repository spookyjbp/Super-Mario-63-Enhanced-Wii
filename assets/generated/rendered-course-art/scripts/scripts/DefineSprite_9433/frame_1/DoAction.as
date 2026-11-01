stop();
HL = undefined;
Move = null;
if(_root.m.MenuRedTrans._x !== undefined && _root.m.MenuRedTrans.YesF !== null)
{
   Mode = "deleting";
}
else
{
   Mode = "";
}
onEnterFrame = function()
{
   Move();
   if(HL !== undefined)
   {
      if(_currentframe < 2)
      {
         bach.StopCoding();
         Move = function()
         {
            nextFrame();
         };
      }
   }
   else if(_currentframe > 22)
   {
      bach.RestartCoding();
      Move = function()
      {
         prevFrame();
      };
   }
};
