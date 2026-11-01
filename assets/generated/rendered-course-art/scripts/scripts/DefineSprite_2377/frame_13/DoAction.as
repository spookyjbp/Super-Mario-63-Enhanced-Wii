stop();
onEnterFrame = null;
if(_parent._parent.battleArena == true)
{
   _root.Course.BackGFX.defeatedcount++;
   _parent._parent.removeMovieClip(_parent._parent);
}
