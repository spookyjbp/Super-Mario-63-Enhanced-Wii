if(_root.Course.Char.attackFrame == "Flying" && _root.Course.Char.attack == true || _root.Course.Char.yspeed > 3 && _root.KeyUP())
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         play();
      }
      else
      {
         stop();
      }
      if(_root.PowerTimer > 160 || Math.round(_root.PowerTimer / Math.round(_root.PowerTimer / 32)) % 2 == 0)
      {
         _visible = true;
      }
      else
      {
         _visible = false;
      }
   };
}
else
{
   onEnterFrame = function()
   {
      if(_root.Course.Char.attackFrame == "Flying" && _root.Course.Char.attack == true || _root.Course.Char.yspeed > 3 && _root.KeyUP())
      {
         play();
      }
      if(_root.PowerTimer > 160 || Math.round(_root.PowerTimer / Math.round(_root.PowerTimer / 32)) % 2 == 0)
      {
         _visible = true;
      }
      else
      {
         _visible = false;
      }
   };
   stop();
}
