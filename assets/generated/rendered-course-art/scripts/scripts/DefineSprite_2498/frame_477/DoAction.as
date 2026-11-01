mo = _parent._parent.Mood;
onEnterFrame = function()
{
   if(mo !== _parent._parent.Mood)
   {
      ChangeFrame();
      onEnterFrame = null;
   }
   mo = _parent._parent.Mood;
};
