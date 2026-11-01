if(_root.BossCheckpoint !== true)
{
   _visible = false;
   _root.Course.Char._x = -400;
   _root.Course.Char._y = -500;
   _root.Course.Char._xscale = -100;
   _root.CharHP = 8;
   _root.Course.Char.attack = true;
   _root.Course.Char.attackFrame = "SlingStar";
}
else
{
   gotoAndStop("end");
}
