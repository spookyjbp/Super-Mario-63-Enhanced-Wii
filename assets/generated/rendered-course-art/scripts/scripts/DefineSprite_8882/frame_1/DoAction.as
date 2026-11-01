th.stop();
scrollspeed = _root.ScrollSpeed;
ss = "Scroll Speed: " + scrollspeed;
th._x = (scrollspeed - 1) / 0.61875 - 80;
onEnterFrame = function()
{
   if(_parent.greyness._alpha < 50)
   {
      if(th.hitTest(_root._xmouse,_root._ymouse,false))
      {
         th._xscale += (110 - th._xscale) / 3;
      }
      else
      {
         th._xscale += (100 - th._xscale) / 3;
      }
      th._yscale = th._xscale;
      if(th.drag == true)
      {
         if(th._currentframe < 5)
         {
            th.nextFrame();
         }
         th._x = Math.min(Math.max(this._xmouse,-80),80);
      }
      else if(th._currentframe > 1)
      {
         th.prevFrame();
      }
      scrollspeed = Math.round((th._x + 80) * 0.61875) + 1;
      _root.ScrollSpeed = scrollspeed;
      ss = "Scroll Speed: " + scrollspeed;
   }
};
onMouseDown = function()
{
   if(th.hitTest(_root._xmouse,_root._ymouse,false) && _parent.greyness._alpha < 50)
   {
      th.drag = true;
   }
};
onMouseUp = function()
{
   th.drag = false;
};
