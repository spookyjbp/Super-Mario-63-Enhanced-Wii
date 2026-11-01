stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      rotation += rotspeed;
      _rotation = rotation;
      _X = _X + xspeed;
      _Y = _Y + yspeed;
      if(this.body.hitTest(_root.Course.Char.charClip.body))
      {
         rotation = 0;
         _rotation = 0;
         myColor = new Color(this);
         colorTrans = new Object();
         colorTrans = explodecolor;
         myColor.setTransform(colorTrans);
         gotoAndStop(explodetype);
      }
      while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y--;
         explode = true;
         rotation = 0;
         _rotation = 0;
      }
      while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y++;
         explode = true;
         rotation = 180;
         _rotation = 180;
      }
      while(_root.Course.Platforms.hitTest((_X + 5) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X--;
         explode = true;
         rotation = -90;
         _rotation = -90;
      }
      while(_root.Course.Platforms.hitTest((_X - 5) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X++;
         explode = true;
         rotation = 90;
         _rotation = 90;
      }
      count++;
      if(count > explodetimer || explode == true)
      {
         myColor = new Color(this);
         colorTrans = new Object();
         colorTrans = explodecolor;
         myColor.setTransform(colorTrans);
         if(explodetype == "small" && explode == true)
         {
            explodetype = "ground";
         }
         gotoAndStop(explodetype);
      }
   }
};
