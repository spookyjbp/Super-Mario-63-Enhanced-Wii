ptnum = num + _parent.Tab * 50;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse) && _root.MWindow1 == true && _Y + _parent._y > -95 && _Y + _parent._y < 35 && _parent._parent.tabchange !== true && _parent.tabbing !== true || num == _parent.Selected)
   {
      _xscale = _xscale + (110 - _xscale) / 3;
      grey._alpha += (- grey._alpha) / 3;
   }
   else
   {
      _xscale = _xscale + (100 - _xscale) / 3;
      grey._alpha += (50 - grey._alpha) / 3;
   }
   if(num == _parent.Selected)
   {
      hl._alpha += (100 - hl._alpha) / 3;
   }
   else
   {
      hl._alpha += (- hl._alpha) / 3;
   }
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse) && _xscale > 102 && _parent.tabbing !== true)
   {
      _root.PlacingTile = ptnum + 100;
      _root.PlaceMode = "Tile";
      _parent.Selected = num;
   }
};
