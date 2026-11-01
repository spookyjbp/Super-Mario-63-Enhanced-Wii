onClipEvent(enterFrame){
   sind += 0.1;
   _alpha = Math.abs(Math.sin(sind) * 50);
   this._xscale = Math.abs(Math.sin(sind) * 50) + 90;
   this._yscale = this._xscale;
}
