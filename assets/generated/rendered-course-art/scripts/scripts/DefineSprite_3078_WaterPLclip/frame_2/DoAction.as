depth = _root.Course.BPlatforms.getNextHighestDepth();
_root.Course.BPlatforms.attachMovie("WaterPLGround","WaterPLGround" + depth,depth,{_x:_X,_y:_Y});
olddiffy = _Y;
diffy = 0;
weightY = 0;
yspeed = 0;
onEnterFrame = function()
{
   clip.gotoAndStop(Scale);
   olddiffy = _Y;
   count = 0;
   while(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - count + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      count++;
      if(count > 7)
      {
         break;
      }
   }
   if(!_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      yspeed -= count / 15;
   }
   count = 0;
   while(!_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + count + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      count++;
      if(count > 7)
      {
         break;
      }
   }
   if(!_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      yspeed += count / 15;
   }
   _rotation = _rotation * 0.7;
   count = 0;
   while(_root.Course.Water.hitTest((_X + 25 * Scale) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - count + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      count++;
      if(count > 9)
      {
         break;
      }
   }
   _rotation = _rotation + count / Scale;
   count = 0;
   while(_root.Course.Water.hitTest((_X - 25 * Scale) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - count + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      count++;
      if(count > 9)
      {
         break;
      }
   }
   _rotation = _rotation - count / Scale;
   count = 0;
   _Y = _Y + yspeed;
   yspeed *= 0.8;
   yspeed = Math.max(yspeed,-3);
   yspeed = Math.min(yspeed,3);
   while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 7) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      _Y++;
   }
   while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 7) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      _Y--;
   }
   diffy = olddiffy - _Y;
   tellTarget(_root.Course.BPlatforms["WaterPLGround" + depth])
   {
      _xscale = this.Scale * 100;
      _rotation = this._rotation;
      _Y = this._y;
      _X = this._x;
      if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.diffy + 3) * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Char.yspeed > -3)
      {
         _root.Course.Char._y -= this.yspeed;
         distance = (_root.Course.Char._x - (_X + _parent._x)) / this.Scale;
         distance = Math.min(60,distance);
         distance = Math.max(-60,distance);
         this._rotation += distance / 15;
         if(landedon == false)
         {
            this.yspeed += _root.Course.Char.yspeed / 3 / this.Scale;
         }
         landedon = true;
      }
      else
      {
         this.weightY *= 0.3;
         if(landedon == true)
         {
            this.yspeed -= _root.Course.Char.yspeed / 5 / this.Scale;
            distance = (_root.Course.Char._x - (_X + _parent._x)) / this.Scale;
            distance = Math.min(60,distance);
            distance = Math.max(-60,distance);
            this._rotation += distance * Math.abs(_root.Course.Char.yspeed / 5) / 25;
         }
         landedon = false;
      }
      _rotation = this._rotation;
      _Y = this._y;
      _X = this._x;
   }
};
