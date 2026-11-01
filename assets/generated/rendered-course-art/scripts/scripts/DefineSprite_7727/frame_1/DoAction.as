onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse) && _root.MWindow1 == true && _parent.held !== true && _parent.tabbing !== true)
   {
      sc = 110;
      arr._alpha += (100 - arr._alpha) / 2;
   }
   else
   {
      sc = 100;
      arr._alpha += (50 - arr._alpha) / 2;
   }
   if(held == true)
   {
      _parent.drag._y += -2 * this._yscale / 100;
   }
};
onMouseDown = function()
{
   if(sc == 110)
   {
      held = true;
   }
};
onMouseUp = function()
{
   held = false;
};
