chang._alpha = 0;
selecting = false;
if(Tab == undefined)
{
   Tab = 0;
}
tablast = Tab;
onEnterFrame = function()
{
   if(_currentframe > 2)
   {
      _root.MenuAccess = true;
   }
   if(tablast !== Tab)
   {
      _parent.moved.Tab = this.Tab;
      _parent.moved.Reload();
      currenttab.gotoAndStop(Tab + 1);
   }
   if(currenttab._xscale > 105 && _currentframe < 3)
   {
      chang._alpha += (100 - chang._alpha) / 4;
   }
   else
   {
      chang._alpha += (- chang._alpha) / 4;
   }
   tablast = Tab;
};
