onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body) && _root.Invincible !== true && _root.Invisible !== true)
      {
         _root.Course.Char.xspeed -= 2;
         _root.Course.Char.xspeed *= 0.8;
      }
   }
};
