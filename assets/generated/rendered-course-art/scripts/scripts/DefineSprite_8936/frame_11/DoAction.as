widt = 170;
heigh = 140;
speed = undefined;
FunctionContinue = null;
FunctionExit = null;
OEF = null;
FunctionContinue = function()
{
   speed = 0;
   OEF = function()
   {
      speed += 1;
      this._y -= speed;
      speed *= 0.95;
      if(_Y < -1000)
      {
         speed = undefined;
         frame = undefined;
         gotoAndStop("blank");
         OEF = null;
      }
   };
};
FunctionQuit = function()
{
   this._y = 1000;
   frame = undefined;
   _root.gotoAndStop("LDMenu");
   _root.RemoveLevelDesigner();
};
