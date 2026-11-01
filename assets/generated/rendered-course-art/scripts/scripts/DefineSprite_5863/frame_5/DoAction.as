tellTarget(_parent._parent)
{
   idlecount = 0;
   idlecounts = 0;
   shockwave = true;
   yspeed = -12;
   X = _X + _root.Course.FrontGFX._x;
   X = (X - _root.Course.Char._x) / 40;
   X = Math.min(X,1);
   X = Math.max(X,-1);
   attack = false;
}
