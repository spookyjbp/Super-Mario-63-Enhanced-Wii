stop();
_rotation = 0;
onEnterFrame = function()
{
   if(_parent.seeing == false)
   {
      gotoAndPlay(1);
   }
};
