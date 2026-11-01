_parent._parent.HP--;
if(_parent._parent.HP <= 0)
{
   _root.Course.Platforms["KingBombombGround" + _parent._parent.depth].removeMovieClip();
   _root["coindata" + _root.playingcourse][_parent._parent.coinnum] = 1;
   _parent._parent.onEnterFrame = null;
   gotoAndStop("fall");
   play();
}
