stop();
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      this._xscale += (130 - this._xscale) / 3;
   }
   else
   {
      this._xscale += (100 - this._xscale) / 3;
   }
   this._yscale = this._xscale;
};
