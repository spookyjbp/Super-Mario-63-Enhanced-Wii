stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.Course.Char.air == false && count++ > 96 && _root.playpause == false)
   {
      play();
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "bosscinema";
      onEnterFrame = function()
      {
         _root.Camcenter = _parent._parent.Target3;
         _root.CamSpeed = 1;
         _root.PauseGame = false;
         _root.playpause = true;
      };
   }
};
