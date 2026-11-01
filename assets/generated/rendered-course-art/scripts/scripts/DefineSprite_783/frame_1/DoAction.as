Y = _Y;
ysin = Math.random() * 10;
R = _rotation;
rsin = Math.random() * 10;
al = 50;
_alpha = 50;
_Y = Y + Math.sin(ysin) * 5;
_rotation = R + Math.sin(rsin) * 5;
onEnterFrame = function()
{
   if(_parent.sc == 100)
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
      if(_root.BGquality == _name)
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
      ysin += 0.03;
      _Y = Y + Math.sin(ysin) * 5;
      rsin += 0.05;
      _rotation = R + Math.sin(rsin) * 5;
   }
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _xscale = 140;
      _yscale = 140;
      _root.BGquality = _name;
   }
};
if(_root.BGquality == undefined)
{
   _root.BGquality = "MEDIUM";
}
if(_root.BGquality == _name)
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
