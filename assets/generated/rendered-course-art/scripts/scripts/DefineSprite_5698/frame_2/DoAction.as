_root.Course.Char.attack = true;
_root.Course.Char.attackFrame = "SlingStar";
_root.Course.Char.charClip.frame.gotoAndPlay("fling");
_root.Course.Char._xscale = -100;
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
