stop();
if(_root.Course.Char.attackFrame !== "Respawn")
{
   _root.Course.Char.attack = true;
   _root.Course.Char.attackFrame = "Dive";
   _root.Course.Char._x = -250;
   _root.Course.Char._y -= 50;
   _root.Course.Char.yspeed = -14;
   _root.Course.Char.xspeed = 40;
}
