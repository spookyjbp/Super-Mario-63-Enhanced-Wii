onEnterFrame = function()
{
   gotoAndStop(2);
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _root.MWindow1 == true)
   {
      sc = 120;
   }
   else
   {
      sc = 100;
   }
   if(_root.PlaceMode == "TileSelect" && _root.sideframe == "PlaceTiles" || _root.PlaceMode == "ItemSelect" && _root.sideframe == "PlaceItems")
   {
      sc = Math.max(110,sc);
      gotoAndStop(3);
   }
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
   txt._xscale += (sc - txt._xscale) / 3;
   txt._yscale = txt._xscale;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _parent.moved.Selected = undefined;
      if(_root.sideframe == "PlaceTiles")
      {
         _root.PlaceMode = "TileSelect";
         _root.PlacingTile = undefined;
      }
      else
      {
         _root.PlaceMode = "ItemSelect";
      }
      _root.M.Window1frame = undefined;
      _root.M.Window1._y = 1000;
      _root.M.Window1.gotoAndStop("blank");
   }
};
