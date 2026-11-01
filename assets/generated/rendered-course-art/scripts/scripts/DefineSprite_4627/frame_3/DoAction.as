onEnterFrame = null;
stop();
i = 0;
while(i < 5)
{
   _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x,_y:_parent._parent._y});
   i++;
}
