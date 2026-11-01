alfa = _alpha;
rot = random(5) - 2;
if(speed == undefined)
{
   speed = 2;
}
count = 0;
stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count++;
      _alpha = _alpha - alfa / 10;
      _rotation = _rotation + rot;
      this._xscale *= 0.98;
      this._yscale *= 0.98;
      if(count >= speed)
      {
         nextFrame();
         count = 0;
      }
   }
   else
   {
      stop();
   }
};
