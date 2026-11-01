onEnterFrame = function()
{
   if(_root["File" + _parent._parent.HL + "TotalStars"] == 0 && _root["File" + _parent._parent.HL + "TotalStars"] !== undefined)
   {
      gotoAndStop(1);
   }
   else if(_root["File" + _parent._parent.HL + "TotalStars"] !== undefined)
   {
      gotoAndStop(2);
   }
};
