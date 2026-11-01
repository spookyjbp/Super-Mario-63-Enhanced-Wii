if(bounce == true)
{
   disapearcount = 0;
   bouncefunction = function()
   {
      if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         yspeed += _root.gravity / 3;
         yspeed *= 0.95;
         xspeed *= 0.95;
      }
      else
      {
         yspeed += _root.gravity;
         if(yspeed > 0)
         {
            yspeed *= 0.97;
         }
      }
      _Y = _Y + Math.min(Math.max(yspeed,-8),8);
      _X = _X + Math.min(Math.max(xspeed,-7),7);
      if(yspeed > 0 && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         yspeed = _root.groundFriction(- yspeed,0.5,1.2);
         xspeed = _root.groundFriction(xspeed,0.3,1.1);
      }
      if(xspeed < 0 && _root.Course.Platforms.hitTest((_X - 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         xspeed = (- xspeed) * 0.6;
         yspeed = _root.groundFriction(yspeed,0.5,1.1);
      }
      if(xspeed > 0 && _root.Course.Platforms.hitTest((_X + 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         xspeed = (- xspeed) * 0.6;
         yspeed = _root.groundFriction(yspeed,0.5,1.1);
      }
      if(yspeed < 0 && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         yspeed = _root.groundFriction(- yspeed,0.5,1.5);
         xspeed *= 0.95;
      }
      while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y--;
      }
      while(_root.Course.Platforms.hitTest((_X - 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X++;
      }
      while(_root.Course.Platforms.hitTest((_X + 7) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X--;
      }
      while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y++;
      }
      disapearcount++;
      if(disapearcount > 12)
      {
         hitcoin = true;
      }
      else
      {
         hitcoin = false;
      }
      if(disapearcount > 288)
      {
         _alpha = random(100);
         if(disapearcount > 384)
         {
            this.removeMovieClip(this);
         }
      }
   };
}
