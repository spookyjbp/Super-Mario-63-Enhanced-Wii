rot = (_rotation + 50) * 3.141592653589793 / 180;
if(_root.Course.Char._xscale > 0)
{
   _root.Course.FrontGFX.Bowser.xspeed = (- Math.cos(rot)) * 14;
   _root.Course.FrontGFX.Bowser.yspeed = (- Math.sin(rot)) * 15;
}
else
{
   _root.Course.FrontGFX.Bowser.xspeed = Math.cos(rot) * 14;
   _root.Course.FrontGFX.Bowser.yspeed = (- Math.sin(rot)) * 15;
}
if(_root.Course.FrontGFX.Bowser.yspeed < 0)
{
   _root.Course.FrontGFX.Bowser.yspeed *= 0.5;
}
if(_root.Course.FrontGFX.Bowser.xspeed > 0)
{
   _root.Course.FrontGFX.Bowser._xscale = -100;
}
else
{
   _root.Course.FrontGFX.Bowser._xscale = 100;
}
_root.Course.FrontGFX.Bowser.attack = true;
_root.Course.FrontGFX.Bowser.attackFrame = "tossed";
_root.Course.FrontGFX.Bowser._x = _root.Course.Char._x - _root.Course.FrontGFX._x + _root.Course.FrontGFX.Bowser.xspeed * 2;
_root.Course.FrontGFX.Bowser._y = _root.Course.Char._y - _root.Course.FrontGFX._y + _root.Course.FrontGFX.Bowser.yspeed * 2;
_root.Course.FrontGFX.Bowser.gettingbackoncourse = false;
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
};
