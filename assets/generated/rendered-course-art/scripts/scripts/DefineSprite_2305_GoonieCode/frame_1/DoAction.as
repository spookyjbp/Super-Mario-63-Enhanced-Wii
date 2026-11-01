if(speedY == undefined)
{
   speedY = -0.7 - Math.random() * 0.25;
}
if(moveup == undefined)
{
   moveup = 250;
}
if(movedown == undefined)
{
   movedown = 45;
}
if(_xscale > 0)
{
   if(speedX == undefined)
   {
      speedX = -2 - Math.random() * 0.5;
   }
}
else if(speedX == undefined)
{
   speedX = 2 + Math.random() * 0.5;
}
upcount = random(moveup);
downcount = 0;
frame = "fly";
depth = _root.Course.BPlatforms.getNextHighestDepth();
_root.Course.BPlatforms.attachMovie("GooniePL","GooniePL" + depth,depth,{_x:_X,_y:_Y,speedY:speedY,speedX:speedX,landedon:false,_xscale:_xscale});
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(frame == "fly")
      {
         upcount++;
         if(upcount > moveup && clip.wingdive == true)
         {
            frame = "dive";
            upcount = 0;
            downcount = random(movedown / 2);
         }
      }
      else if(frame == "dive")
      {
         downcount++;
         if(downcount > movedown)
         {
            clip.play();
            upcount = random(moveup / 2);
            downcount = 0;
         }
      }
      tellTarget(_root.Course.BPlatforms["GooniePL" + depth])
      {
         currentspeedX = speedX;
         if(this.frame == "ride")
         {
            currentspeedY = _root.gravity / 1.5;
         }
         else if(this.frame == "dive")
         {
            currentspeedY = _root.gravity * 1.5;
            currentspeedX = speedX * 1.5;
         }
         else
         {
            currentspeedY = speedY;
         }
         if(currentspeedY < 0)
         {
            _Y = _Y - currentspeedY;
         }
         if(_root.playpause == false)
         {
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _root.Course.Char._x += currentspeedX;
               _root.Course.Char._y += currentspeedY;
               _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.3,1.1);
               if(landedon == false)
               {
                  _root.Course.Char.xspeed -= currentspeedX;
                  this.frame = "ride";
               }
               landedon = true;
            }
            else
            {
               if(landedon == true)
               {
                  this.frame = "fly";
                  _root.Course.Char.xspeed += currentspeedX;
                  _root.Course.Char.yspeed += currentspeedY / 2;
               }
               landedon = false;
            }
            if(currentspeedY < 0)
            {
               _Y = _Y + currentspeedY;
            }
         }
         count = 0;
         this._y += currentspeedY;
         this._x += currentspeedX;
         bounds = _root.Course.CamEdge.getBounds(_root.Course.CamEdge);
         coursewidth = bounds.xMax - bounds.xMin;
         courseheight = bounds.yMax - bounds.yMin;
         if(this._x > bounds.xMax + 5)
         {
            this._x -= coursewidth;
         }
         if(this._x < bounds.xMin - 5)
         {
            this._x += coursewidth;
         }
         if(this._y > bounds.yMax + 5)
         {
            this._y -= courseheight;
         }
         if(this._y < bounds.yMin - 5)
         {
            this._y += courseheight;
         }
         if(_root.playpause == false)
         {
            _Y = this._y;
            _X = this._x;
         }
         _xscale = this._xscale;
      }
      gotoAndStop(frame);
   }
};
