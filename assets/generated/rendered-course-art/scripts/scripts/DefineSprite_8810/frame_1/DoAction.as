onEnterFrame = function()
{
   if(_parent.ydirection == "none")
   {
      gotoAndStop(2);
      _parent.yspeednum = 0;
      _parent.ylengthnum = 0;
      _parent.yoffsetnum = 0;
      _parent.yaccel = 0;
   }
   else
   {
      gotoAndStop(1);
   }
};
stop();
