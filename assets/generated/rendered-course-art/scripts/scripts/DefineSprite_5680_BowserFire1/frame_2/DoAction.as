stop();
rotspeed = 0;
rot = 0;
rot2 = 0;
if(rotdiv == undefined)
{
   rotdiv = 10;
}
if(rotmaxspeed == undefined)
{
   rotmaxspeed = 3;
}
hitwall = function(hitwallnum)
{
   if(bounce == false || bouncecount >= bouncemax)
   {
      if(hitwallnum == 0)
      {
         rotation = 0;
         _rotation = 0;
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
         }
      }
      if(hitwallnum == 1)
      {
         rotation = -90;
         _rotation = -90;
         while(_root.Course.Platforms.hitTest((_X + 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X--;
         }
      }
      if(hitwallnum == 2)
      {
         rotation = 90;
         _rotation = 90;
         while(_root.Course.Platforms.hitTest((_X - 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X++;
         }
      }
      if(hitwallnum == 3)
      {
         rotation = 180;
         _rotation = 180;
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 12) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y++;
         }
      }
      myColor = new Color(this);
      colorTrans = new Object();
      colorTrans = explodecolor;
      myColor.setTransform(colorTrans);
      if(explodesize == "small")
      {
         explodesize = "ground";
      }
      gotoAndStop(explodesize);
   }
   else
   {
      bouncecount += random(2) + 1;
      if(hitwallnum == 0)
      {
         yspeed = (- yspeed) * 0.7;
         yspeed -= bounceboost;
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
         }
      }
      if(hitwallnum == 1)
      {
         xspeed = - Math.abs(xspeed * 0.7);
         xspeed -= bounceboost / 4;
         while(_root.Course.Platforms.hitTest((_X + 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X--;
         }
      }
      if(hitwallnum == 2)
      {
         xspeed = Math.abs(xspeed * 0.7);
         xspeed += bounceboost / 4;
         while(_root.Course.Platforms.hitTest((_X - 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X++;
         }
      }
      if(hitwallnum == 3)
      {
         yspeed = (- yspeed) * 0.7;
         yspeed += bounceboost / 4;
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 12) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y++;
         }
      }
      if(follow == true)
      {
         _rotation = Math.atan2(- yspeed,- xspeed) * 180 / 3.141592653589793;
      }
   }
};
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _rotation = rotation;
      if(follow == true)
      {
         rot2 = _root.radiansToAngle(Math.atan2(- (_root.Course.Char._y - (_Y + _parent._y)),- (_root.Course.Char._x - (_X + _parent._x))));
         if(rot > 0)
         {
            rot = rot2;
            if(rot < -90)
            {
               rotation -= 360;
            }
            rotspeed = (rot - rotation) / rotdiv;
         }
         else
         {
            rot = rot2;
            if(rot > 90)
            {
               rotation += 360;
            }
            rotspeed = (rot - rotation) / rotdiv;
         }
         if(rotation > 520)
         {
            rotation -= 360;
         }
         if(rotation < -520)
         {
            rotation += 360;
         }
         rotspeed = _root.groundFriction(rotspeed,0.1,1.15);
         rotspeed = Math.min(rotspeed,rotmaxspeed);
         rotspeed = Math.max(rotspeed,- rotmaxspeed);
         rotation += rotspeed;
         _rotation = rotation;
         yspeed = (- Math.sin(_root.angleToRadians(rotation))) * speed;
         xspeed = (- Math.cos(_root.angleToRadians(rotation))) * speed;
      }
      else if(bounce == false)
      {
         yspeed = (- Math.sin(_root.angleToRadians(rotation))) * speed;
         xspeed = (- Math.cos(_root.angleToRadians(rotation))) * speed;
      }
      else
      {
         yspeed += _root.gravity / 2;
         yspeed = Math.min(yspeed,10);
         rotation = _root.radiansToAngle(Math.atan2(- yspeed,- xspeed));
         _rotation = rotation;
      }
      repeats = Math.floor((Math.abs(yspeed) + Math.abs(xspeed)) / 8) + 1;
      i = 0;
      while(i < repeats)
      {
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            hitwall(0);
         }
         if(_root.Course.Platforms.hitTest((_X + 10) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            hitwall(1);
         }
         if(_root.Course.Platforms.hitTest((_X - 10) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            hitwall(2);
         }
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 10) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            hitwall(3);
         }
         _X = _X + xspeed / repeats;
         _Y = _Y + yspeed / repeats;
         i++;
      }
      if(this.body.hitTest(_root.Course.Char.charClip.body))
      {
         rotation = 0;
         _rotation = 0;
         myColor = new Color(this);
         colorTrans = new Object();
         colorTrans = explodecolor;
         myColor.setTransform(colorTrans);
         gotoAndStop(explodesize);
      }
      count++;
      if(count > _root.framerate * 30)
      {
         this.removeMovieClip(this);
      }
      if(bounce == true)
      {
         if(count > lifetime)
         {
            rotation = 0;
            _rotation = 0;
            myColor = new Color(this);
            colorTrans = new Object();
            colorTrans = explodecolor;
            myColor.setTransform(colorTrans);
            gotoAndStop(explodesize);
         }
      }
   }
};
