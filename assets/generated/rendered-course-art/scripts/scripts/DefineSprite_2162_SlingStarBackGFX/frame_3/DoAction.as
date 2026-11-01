stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      star.base._rotation += 0.5;
      star2.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
      count++;
      if(count > 32)
      {
         actualfunction();
      }
   }
};
actualfunction = function()
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         star.base._rotation++;
         star2.base._rotation = star.base._rotation;
         _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
         _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
         if(this.box.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attack == false || _root.Course.Char.attack == true && (_root.Course.Char.attackFrame == "SlingStar" || _root.Course.Char.attackFrame == "Spin" || _root.Course.Char.attackFrame == "Dive" || _root.Course.Char.attackFrame == "frontflip")) && _root.Course.Char.air == true && _root.Course.Char.yspeed > -7)
         {
            savecamzoom = _root.CameraZoom;
            _root.CameraZoom = 50;
            _root.Course.Char.charClip.speed = 0;
            if(_root.Course.Char.attack == true && _root.Course.Char.attackFrame == "SlingStar")
            {
               _root.Course.Char.charClip.frame.gotoAndPlay(1);
            }
            _root.Course.Char.attack = true;
            _root.Course.Char.attackFrame = "SlingStar";
            onEnterFrame = function()
            {
               if(_root.PauseGame == false)
               {
                  _root.Course.Char._rotation = rotation;
                  star.base._rotation += 7;
                  star2.base._rotation = star.base._rotation;
                  _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
                  _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
                  X = _X + _parent._x;
                  _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
                  Y = _Y + _parent._y;
                  _root.Course.Char._y += (Y - _root.Course.Char._y) / 5;
                  _root.Course.Char.yspeed *= 0.6;
                  _root.Course.Char.xspeed *= 0.6;
                  if(_root.Course.Char.attack == false || _root.Course.Char.attackFrame !== "SlingStar")
                  {
                     _root.CameraZoom = savecamzoom;
                     onEnterFrame = function()
                     {
                        star.base._rotation += 1;
                        star2.base._rotation = star.base._rotation;
                        _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
                        _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
                     };
                     gotoAndPlay(1);
                  }
                  else if(Math.abs(_root.Course.Char.charClip.speed) > 10 && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "SlingStar" && _root.Course.Char.charClip.frame.count <= 25)
                  {
                     _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
                     _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
                     play();
                     onEnterFrame = function()
                     {
                        if(_root.PauseGame == false)
                        {
                           _root.Course.Char._rotation = rotation;
                           star.base._rotation += 15;
                           star2.base._rotation = star.base._rotation;
                           _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
                           _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
                           X = _X + _parent._x;
                           _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
                           Y = _Y + _parent._y;
                           _root.Course.Char._y += (Y - _root.Course.Char._y) / 5;
                           play();
                        }
                        else
                        {
                           stop();
                        }
                     };
                  }
               }
            };
         }
      }
   };
};
