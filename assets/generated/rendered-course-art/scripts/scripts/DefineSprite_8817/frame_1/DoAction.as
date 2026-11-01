onEnterFrame = function()
{
   if(_parent.xdirection == "none")
   {
      gotoAndStop(2);
      _parent.xspeednum = 0;
      _parent.xlengthnum = 0;
      _parent.xoffsetnum = 0;
      _parent.xaccel = 0;
   }
   else
   {
      gotoAndStop(1);
   }
};
stop();
