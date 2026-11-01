num = fireball[loops - 1];
if(_parent._parent._xscale > 0)
{
   rot = _rotation * 3.141592653589793 / 180;
   Xup = Math.cos(rot + 1.5707963267948966);
   Xright = Math.cos(rot);
   Yup = Math.sin(rot + 1.5707963267948966);
   Yright = Math.sin(rot);
   X = Xup * -20 + Xright * -45;
   Y = Yup * -20 + Yright * -45;
   rotation = random(20) - 15 + _rotation;
}
else
{
   rot = (- _rotation) * 3.141592653589793 / 180;
   Xup = Math.cos(rot + 1.5707963267948966);
   Xright = Math.cos(rot);
   Yup = Math.sin(rot + 1.5707963267948966);
   Yright = Math.sin(rot);
   X = Xup * -20 + Xright * 45;
   Y = Yup * -20 + Yright * 45;
   rotation = random(20) - 15 - _rotation - 180;
}
if(num == 0)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + X,_y:_parent._parent._y + Y,bounce:false,frame:1,rotation:rotation});
}
else if(num == 1)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + X,_y:_parent._parent._y + Y,bounce:true,frame:2,rotation:rotation});
}
else if(num == 2)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + X,_y:_parent._parent._y + Y,bounce:false,follow:true,frame:3,rotation:rotation});
}
else if(num == 3)
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + X,_y:_parent._parent._y + Y,bounce:true,bouncemax:5 + random(8),bounceboost:7,frame:4,speed:3 + Math.random() * 6,lifetime:960,rotation:rotation});
}
else
{
   _root.Course.BackGFX.attachMovie("BowserFire1","BowserFire" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x + X,_y:_parent._parent._y + Y,bounce:false,frame:5,explodesize:"large",speed:6 + Math.random() * 3,rotation:rotation});
}
