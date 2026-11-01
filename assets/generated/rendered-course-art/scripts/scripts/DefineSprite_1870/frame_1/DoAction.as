stop();
count = -1;
count2 = _root.Course.Char.landtime;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(count++ > count2 || _root.Course.Char.air == true)
      {
         _root.Course.Char.attack = false;
      }
      if(_root.KeyUp() && animation._currentframe < 5)
      {
         animation.gotoAndPlay("land");
      }
   }
};
