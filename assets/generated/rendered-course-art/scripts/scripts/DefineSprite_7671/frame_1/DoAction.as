this.stop();
Start = function()
{
   _X = 0;
   _Y = 0;
   Res();
   i = 0;
   while(i < 5)
   {
      OEF();
      i++;
   }
   trace(_X);
   trace(_Y);
   if(_X > 1)
   {
      _X = -48;
   }
   else if(_X < -1)
   {
      _X = 48;
   }
   else
   {
      _X = 0;
   }
   if(_Y > 1)
   {
      _Y = -48;
   }
   else if(_Y < -1)
   {
      _Y = 48;
   }
   else
   {
      _Y = 0;
   }
   Res();
   onEnterFrame = function()
   {
      OEF();
      if(gonecount > 20)
      {
         Start();
      }
   };
};
Start2 = function()
{
   _X = 0;
   _Y = 0;
   Res();
   _alpha = 50;
   onEnterFrame = function()
   {
      OEF();
      if(gonecount > 20)
      {
         Start2();
      }
   };
};
Res = function()
{
   if(DirectionX == undefined)
   {
      DirectionX = "Right";
   }
   if(DirectionY == undefined)
   {
      DirectionY = "Up";
   }
   if(saveDX !== undefined)
   {
      DirectionX = saveDX;
      DirectionY = saveDY;
   }
   saveDX = DirectionX;
   saveDY = DirectionY;
   if(distanceX == undefined)
   {
      distanceX = 25;
   }
   if(distanceY == undefined)
   {
      distanceY = 0;
   }
   if(accelX == undefined)
   {
      accelX = 1;
   }
   if(accelY == undefined)
   {
      accelY = 0;
   }
   if(speedX == undefined)
   {
      speedX = 5;
   }
   if(speedY == undefined)
   {
      speedY = 0;
   }
   if(Xoffstart == undefined)
   {
      Xoffstart = 0;
   }
   if(Yoffstart == undefined)
   {
      Yoffstart = 0;
   }
   if(scale == undefined)
   {
      scale = 2;
   }
   this.gotoAndStop(scale);
   if(DirectionY == "Up")
   {
      _Y = _Y - speedY;
   }
   else
   {
      _Y = _Y + speedY;
   }
   if(DirectionY == "Right")
   {
      _X = _X - speedX;
   }
   else
   {
      _X = _X + speedX;
   }
   currentspeedX = 0;
   distancecountX = speedX + Xoffstart;
   currentspeedY = 0;
   distancecountY = speedY + Yoffstart;
   gonecount = 0;
   count = 0;
   _alpha = 100;
   _visible = true;
};
OEF = function()
{
   if(touchngo == true)
   {
      count++;
      if(count > disapearcount)
      {
         gonecount++;
         _alpha = _alpha - 20;
         if(_alpha < 0)
         {
            _alpha = 0;
            _visible = false;
         }
      }
   }
   distancecountX++;
   if(distancecountX >= distanceX)
   {
      distancecountX = 0;
      if(DirectionX == "Right")
      {
         DirectionX = "Left";
      }
      else
      {
         DirectionX = "Right";
      }
   }
   if(DirectionX == "Right")
   {
      currentspeedX += accelX;
   }
   else
   {
      currentspeedX -= accelX;
   }
   distancecountY++;
   if(distancecountY >= distanceY)
   {
      distancecountY = 0;
      if(DirectionY == "Up")
      {
         DirectionY = "Down";
      }
      else
      {
         DirectionY = "Up";
      }
   }
   if(DirectionY == "Up")
   {
      currentspeedY -= accelY;
   }
   else
   {
      currentspeedY += accelY;
   }
   currentspeedX = Math.max(- speedX,currentspeedX);
   currentspeedX = Math.min(speedX,currentspeedX);
   currentspeedY = Math.max(- speedY,currentspeedY);
   currentspeedY = Math.min(speedY,currentspeedY);
   _X = _X + currentspeedX;
   _Y = _Y + currentspeedY;
};
