Y = -110;
drag._y = -40;
onEnterFrame = function()
{
   if(tabbing !== true)
   {
      if(held == true)
      {
         drag._y = Math.max(-40,Math.min(_ymouse,40));
         Y = (drag._y + 40) / 80 * (- _parent.moved._height + 120) - (80 + _parent.movedoffset);
      }
      else
      {
         drag._y = Math.max(-40,Math.min(drag._y,40));
         Y = (drag._y + 40) / 80 * (- _parent.moved._height + 120) - (80 + _parent.movedoffset);
      }
      _parent.moved._y = Y;
   }
};
onMouseDown = function()
{
   if(bar.hitTest(_root._xmouse,_root._ymouse,false) && _root.MWindow1 == true && tabbing !== true)
   {
      held = true;
   }
};
onMouseUp = function()
{
   held = false;
};
if(_parent.MouseWheel !== true)
{
   MouseWheel = new Object();
   MouseWheel.onMouseWheel = function(delta)
   {
      delta2 = delta <= 0 ? -1 : 1;
      if(_xmouse > -10 && Math.abs(_ymouse) < 60 && _root.MWindow1 == true)
      {
         drag._y += 16 * delta2 * (200 / (- _parent.moved._height + 120));
      }
   };
   Mouse.addListener(MouseWheel);
   _parent.MouseWheel = true;
}
