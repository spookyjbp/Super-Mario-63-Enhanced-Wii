i = 0;
while(i < random(2) + 2)
{
   _root.Course.BackGFX.attachMovie("ExplodeBomb-ombCode","ExplodeBomb-ombCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:425 + random(500),_y:-300 - random(50),xspeed:Math.random() * 6 - 3,yspeed:Math.random() * 4 - 2});
   i++;
}
_root.Course.BackGFX.attachMovie("ExplodeBomb-ombCode","ExplodeBomb-ombCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.Course.Char._x - _root.Course.BackGFX._x + (Math.random() * 6 - 3),_y:-300 - random(50),xspeed:Math.random() * 6 - 3,yspeed:Math.random() * 4 - 2});
_root.Course.BackGFX.attachMovie("ExplodeBomb-ombCode","ExplodeBomb-ombCode" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.Course.Char._x - _root.Course.BackGFX._x + (Math.random() * 6 - 3),_y:-300 - random(50),xspeed:Math.random() * 6 - 3,yspeed:Math.random() * 4 - 2});
