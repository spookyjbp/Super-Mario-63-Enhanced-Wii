randomnum = random(2) + 1;
if(randomnum == 1)
{
   if(_parent._parent._xscale > 0)
   {
      _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x - 25,_y:_parent._parent._y - 30,bounce:true,frame:2,rotation:random(10) - 5});
   }
   else
   {
      _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + 25,_y:_parent._parent._y - 30,bounce:true,frame:2,rotation:180 + random(10) - 5});
   }
}
else if(_parent._parent._xscale > 0)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x - 25,_y:_parent._parent._y - 30,bounce:false,rotation:random(20) - 15});
}
else
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + 25,_y:_parent._parent._y - 30,bounce:false,rotation:180 + random(20) - 15});
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
