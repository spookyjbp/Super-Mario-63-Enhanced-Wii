al = 50;
_alpha = 50;
onEnterFrame = function()
{
   if(_parent.greyness.aa == 0)
   {
      if(this.hitTest(_root._xmouse,_root._ymouse,false))
      {
         sc = 120;
         al = 100;
      }
      else
      {
         sc = 100;
         al = 50;
      }
      if(_parent.dir == _name)
      {
         gotoAndStop(_name + "2");
         sc = 125;
         al = 100;
      }
      else
      {
         gotoAndStop(_name);
      }
      _xscale = _xscale + (sc - _xscale) / 2;
      _yscale = _xscale;
      _alpha = _alpha + (al - _alpha) / 2;
   }
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _xscale = 140;
      _yscale = 140;
      _parent.dir = _name;
   }
};
if(_parent.dir == undefined)
{
   _parent.dir = "Both";
}
if(_parent.dir == _name)
{
   gotoAndStop(_name + "2");
   _alpha = 100;
   _xscale = 120;
   _yscale = 120;
}
else
{
   gotoAndStop(_name);
}
