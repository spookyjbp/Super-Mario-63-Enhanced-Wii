if(_parent._parent._xscale > 0)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire1" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x - 15,_y:_parent._parent._y - 50,rotation:70 + random(20) - 10,bounce:true,speed:11,frame:2});
}
else
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire1" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + 15,_y:_parent._parent._y - 50,rotation:-110 + random(20) - 10,bounce:true,speed:11,frame:2});
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
