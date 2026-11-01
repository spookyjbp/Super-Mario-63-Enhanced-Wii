if(random(2) == 1)
{
   bounce = true;
}
else
{
   bounce = false;
}
if(_parent._parent._xscale > 0)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire1" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x - 25,_y:_parent._parent._y - 40,rotation:-45,bounce:bounce,frame:2});
}
else
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire1" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + 25,_y:_parent._parent._y - 40,rotation:-135,bounce:bounce,frame:2});
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
