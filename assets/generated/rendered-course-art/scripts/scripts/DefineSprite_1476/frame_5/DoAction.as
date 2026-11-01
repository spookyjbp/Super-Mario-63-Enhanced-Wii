stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.KeySPACE() || _root.KeySPIN())
      {
         _parent.speed += 15;
         _parent.speed *= 0.8;
      }
      _root.Course.Char.yspeed = - _root.gravity;
      _root.Course.Char.xspeed = 0;
      if(_root.KeyDOWN())
      {
         count += 2;
      }
      count++;
      if(count > 32)
      {
         go = false;
         play();
      }
   }
};
