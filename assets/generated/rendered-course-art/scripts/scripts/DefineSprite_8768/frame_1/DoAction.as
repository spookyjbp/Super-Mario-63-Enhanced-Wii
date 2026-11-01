min = 1;
max = 9;
onEnterFrame = function()
{
   if(_parent.greyness.aa == 0)
   {
      if(_parent.blocktype == min)
      {
         tri1._alpha += (- tri1._alpha) / 3;
      }
      else
      {
         tri1._alpha += (100 - tri1._alpha) / 3;
      }
      if(_parent.blocktype == max)
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
   }
};
onMouseDown = function()
{
   if(tri2.sc == 130 && _parent.fra < max)
   {
      _parent.fra = _parent.fra + 1;
   }
   else if(tri1.sc == 130 && _parent.fra > min)
   {
      _parent.fra = _parent.fra - 1;
   }
};
