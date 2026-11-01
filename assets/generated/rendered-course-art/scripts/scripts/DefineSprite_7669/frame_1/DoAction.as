cl = "";
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && _root.MenuAccess !== true)
   {
      _root.MenuAccess = true;
      ma = true;
   }
   else if(ma == true && !this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      ma = false;
      _root.MenuAccess = false;
   }
};
