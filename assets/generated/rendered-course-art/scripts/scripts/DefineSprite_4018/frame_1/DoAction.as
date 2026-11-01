onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
      if(ground !== true)
      {
         if(this.box.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.air == false)
         {
            _root.Course.Char._x -= 3;
            _root.Course.Char.xspeed *= 0.6;
            _root.Course.Char.xspeed -= 0.8;
         }
      }
   }
   else
   {
      stop();
   }
};
