widt = 170;
heigh = 140;
speed = undefined;
FunctionKeep = null;
FunctionDelete = null;
OEF = null;
red.swapDepths(0);
red.removeMovieClip();
_root.M.attachMovie("LDWResetScreenFilter","LDWResetScreenFilter",this.getDepth() + 1,{aa:25,_alpha:0});
FunctionKeep = function()
{
   speed = 0;
   _root.M.LDWResetScreenFilter.aa = 0;
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
FunctionDelete = function()
{
   _root.LDMenuFrame = 1;
   speed = 0;
   _root.M.LDWResetScreenFilter.aa = 100;
   OEF = function()
   {
      speed += 1;
      this._y += speed;
      speed *= 0.95;
      if(_Y > 1000)
      {
         speed = undefined;
         frame = undefined;
         gotoAndStop("blank");
         OEF = null;
      }
   };
};
