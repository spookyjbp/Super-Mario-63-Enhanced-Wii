_root.LDCourseBG = _root.LDCourseBG;
gotoAndStop(_root.LDCourseBG + 1);
min = 2;
max = 16;
onEnterFrame = function()
{
   if(_parent.greyness.aa == 0)
   {
      _root.LDCourseBG = _currentframe - 1;
      if(_currentframe == min)
      {
         tri1._alpha += (- tri1._alpha) / 3;
      }
      else
      {
         tri1._alpha += (100 - tri1._alpha) / 3;
      }
      if(_currentframe == max)
      {
         tri2._alpha += (- tri2._alpha) / 3;
      }
      else
      {
         tri2._alpha += (100 - tri2._alpha) / 3;
      }
      if(tri1.hitTest(_root._xmouse,_root._ymouse,false) && tri1._alpha >= 99)
      {
         tri1.sc = 130;
      }
      else
      {
         tri1.sc = 100;
      }
      tri1._xscale += (tri1.sc - tri1._xscale) / 3;
      tri1._yscale = tri1._xscale;
      if(tri2.hitTest(_root._xmouse,_root._ymouse,false) && tri2._alpha >= 99)
      {
         tri2.sc = 130;
      }
      else
      {
         tri2.sc = 100;
      }
      tri2._xscale += (tri2.sc - tri2._xscale) / 3;
      tri2._yscale = tri2._xscale;
      if(this.hitTest(_root._xmouse,_root._ymouse,false))
      {
         bg._alpha += (50 - bg._alpha) / 3;
      }
      else
      {
         bg._alpha += (- bg._alpha) / 3;
      }
   }
   _root.LDCourseBG = _root.LDCourseBG;
};
onMouseDown = function()
{
   if(tri2.sc == 130 && _currentframe < max)
   {
      nextFrame();
   }
   else if(tri1.sc == 130 && _currentframe > min)
   {
      prevFrame();
   }
};
