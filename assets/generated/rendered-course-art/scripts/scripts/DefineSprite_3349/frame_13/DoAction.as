if(_root["coindata" + _root.playingcourse][_parent._parent.coinnum] !== 1)
{
   _root["coindata" + _root.playingcourse][_parent._parent.coinnum] = 1;
   _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x,_y:_parent._parent._y - 12,bounce:true});
}
