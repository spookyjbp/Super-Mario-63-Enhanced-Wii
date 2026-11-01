_root.Course.Char.charClip.frame.gotoAndPlay("spin");
_root.Course.FrontGFX[otherstar].play();
if(Math.abs(rotation) > 3)
{
   if(rotation > 0)
   {
      _root.Course.Char._xscale = -100;
   }
   else
   {
      _root.Course.Char._xscale = 100;
   }
}
if(_root.Course.Char.attack == false)
{
   gotoAndPlay(1);
   onEnterFrame = null;
}
if(_root.Course.Char.attackFrame !== "SlingStar")
{
   gotoAndPlay(1);
   onEnterFrame = null;
}
