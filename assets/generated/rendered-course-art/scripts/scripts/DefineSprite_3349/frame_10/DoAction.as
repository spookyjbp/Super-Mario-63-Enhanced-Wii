if(_parent._parent.bounce == true)
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         play();
      }
      else
      {
         stop();
      }
   };
   if(_root.Course.Char.attackFrame == "EJump" && _root.Course.Char.attack == true)
   {
      _root.Course.Char.attack = false;
      if(_root.Course.Char.landtime == 1)
      {
         _root.Course.Char.yspeed -= 10;
      }
      else if(_root.Course.Char.landtime == 2)
      {
         _root.Course.Char.yspeed -= 12;
      }
      else
      {
         _root.Course.Char.yspeed -= 7;
      }
   }
   else
   {
      _root.Course.Char.yspeed -= 9;
   }
}
