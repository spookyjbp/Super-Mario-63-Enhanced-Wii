speed = 6;
if(_parent._parent.HP == 2)
{
   speed = 7;
}
if(_parent._parent.HP == 1)
{
   speed = 8;
}
if(_parent._parent._xscale > 0)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire1" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x - 25,_y:_parent._parent._y - 30,speed:speed,rotation:random(10) - 5});
}
else
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire1" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + 25,_y:_parent._parent._y - 30,speed:speed,rotation:180 + random(10) - 5});
}
if(_parent._parent.HP == 2)
{
   gotoAndStop("skip1");
   play();
}
if(_parent._parent.HP == 1)
{
   gotoAndStop("skip2");
   play();
}
