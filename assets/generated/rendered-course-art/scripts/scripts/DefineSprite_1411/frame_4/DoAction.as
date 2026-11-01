if(_root.CharHP > 0)
{
   _root.Course.Char.yspeed += _root.Course.Char.setYspeed;
   _root.Course.Char.xspeed += _root.Course.Char.setXspeed;
}
else
{
   _root.Course.Char.yspeed += _root.Course.Char.setYspeed - 3;
   _root.Course.Char.xspeed += _root.Course.Char.setXspeed * 0.8;
   gotoAndStop("falling");
   play();
}
