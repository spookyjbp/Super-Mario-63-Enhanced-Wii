if(color == undefined)
{
   color = "red";
}
if(txt !== "" && txt !== undefined)
{
   count = 0;
   speed = - (random(3) + 18);
   onEnterFrame = function()
   {
      count++;
      if(count > 60)
      {
         count = -500;
         speed = 0.3;
         _root.m.messageonscreen--;
      }
      else if(speed < 0)
      {
         speed *= 0.9;
      }
      else
      {
         speed *= 1.1;
      }
      _Y = _Y + speed;
      if(this._y > 800)
      {
         this.removeMovieClip();
      }
   };
}
else
{
   this.swapDepths(0);
   this.removeMovieClip();
}
gotoAndStop(color);
