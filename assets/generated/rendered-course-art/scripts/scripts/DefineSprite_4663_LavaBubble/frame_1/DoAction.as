wave = random(90);
X = this._x;
Y = 0;
aspeed = 50 / (Yheight / speed + 5);
Yheight *= Math.random() * 0.3 + 0.85;
onEnterFrame = function()
{
   this._y -= speed;
   Y += speed;
   wave += 0.1;
   _alpha = _alpha - aspeed;
   this._x = X + Math.sin(wave) * 10;
   if(Y > Yheight)
   {
      onEnterFrame = function()
      {
         _alpha = _alpha - 6;
         this._xscale *= 1.1;
         this._yscale *= 1.1;
         if(_alpha < 10)
         {
            this.removeMovieClip();
            gotoAndStop(3);
         }
      };
   }
};
