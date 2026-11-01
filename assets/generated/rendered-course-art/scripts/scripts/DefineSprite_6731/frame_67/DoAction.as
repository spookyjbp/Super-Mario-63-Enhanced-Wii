stop();
onEnterFrame = function()
{
   _root.Controll = false;
   if(_root.Course.Char.air == false)
   {
      play();
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "bosscinema";
      _root.playpause = true;
      onEnterFrame = function()
      {
         _root.Camcenter = this;
         _root.PauseGame = false;
      };
   }
};
