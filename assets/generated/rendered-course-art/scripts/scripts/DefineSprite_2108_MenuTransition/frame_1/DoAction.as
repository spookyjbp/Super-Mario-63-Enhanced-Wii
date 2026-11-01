if(trans !== true)
{
   this.swapDepths(0);
   this.removeMovieClip();
}
onEnterFrame = function()
{
   this._y -= 70;
   if(_Y <= -150 && uy == undefined)
   {
      _parent.GSF();
      uy = "defined";
   }
   if(_Y < -500)
   {
      this.removeMovieClip();
   }
};
gotoAndStop(1);
