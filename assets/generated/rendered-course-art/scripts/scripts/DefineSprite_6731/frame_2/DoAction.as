_root.Course.Char.attack = true;
_root.Course.Char.attackFrame = "SlingStar";
_root.Course.Char.charClip.frame.gotoAndPlay("fling");
_root.Course.Char._x = -1000;
_root.Course.Char._y = -500;
_root.Course.Char._xscale = -100;
_root.Course.BackGFX.StartClip._y = 0;
_root.Course.BackGFX.StartClip._x = 0;
_root.Camcenter = this;
_root.Camspeed = 1;
onEnterFrame = function()
{
   _root.Controll = false;
   if(_root.PauseGame == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
