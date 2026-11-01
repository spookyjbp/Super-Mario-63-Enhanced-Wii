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
   if(_root.PlacingTile == _root.BlankTile && _root.sideframe == "PlaceTiles" || _root.PlaceMode == "ItemErase" && _root.sideframe == "PlaceItems")
   {
      sc = Math.max(110,sc);
      gotoAndStop(3);
   }
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _parent.moved.Selected = undefined;
      if(_root.sideframe == "PlaceTiles")
      {
         _root.PlaceMode = "Tile";
         _root.PlacingTile = _root.BlankTile;
      }
      else
      {
         _root.PlaceMode = "ItemErase";
      }
      _root.M.Window1frame = undefined;
      _root.M.Window1._y = 1000;
      _root.M.Window1.gotoAndStop("blank");
   }
};
