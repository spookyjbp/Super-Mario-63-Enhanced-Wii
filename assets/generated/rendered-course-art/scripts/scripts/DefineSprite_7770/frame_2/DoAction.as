_parent.bar.tabbing = false;
_parent.moved.tabbing = false;
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse))
   {
      play();
      _parent.bar.tabbing = true;
      _parent.moved.tabbing = true;
      onMouseDown = null;
   }
};
stop();
