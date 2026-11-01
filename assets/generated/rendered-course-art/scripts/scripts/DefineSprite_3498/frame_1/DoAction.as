sin = 1.5707963267948966;
speed = 1;
count = 0;
stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      OEF();
      i = 0;
      while(i < speed)
      {
         nextFrame();
         sin = (_currentframe - 1 + 22.5) * 4 * 0.017453292519943295;
         this._xscale = Math.sin(sin) * 100;
         i++;
      }
      if(this.body.hitTest(_root.Course.Char.charClip.body) && lastframehit == false && _root.Course.Char.air == false && Math.abs(_root.Course.Char.xspeed) > 2)
      {
         MarioHit(_root.Course.Char.xspeed);
      }
      lastframehit = this.body.hitTest(_root.Course.Char.charClip.body);
   }
};
MarioHit = function(xspeed)
{
   count = 0;
   speed = Math.round((speed - 1) * 0.5);
   speed += Math.round(Math.abs(xspeed * 2));
   OEF = function()
   {
      count += speed;
      if(count > 24)
      {
         count = 0;
         _root.hpcoincount += 5;
      }
      speed -= 0.2;
      if(speed <= 1)
      {
         _root.hpcoincount += 5;
         speed = 1;
         OEF = null;
      }
   };
};
