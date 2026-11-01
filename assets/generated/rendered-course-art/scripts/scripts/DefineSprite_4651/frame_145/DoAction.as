if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 1500 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 1500)
{
   _root.Course.BackGFX.attachMovie("ExplodeBomb-ombCode","ExplodeBomb-ombCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X + 50,_y:_Y - 20,xspeed:12 + random(5),yspeed:-7 - random(8)});
}
