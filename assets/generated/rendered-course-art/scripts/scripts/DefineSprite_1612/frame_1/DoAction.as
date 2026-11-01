frame = 1;
throwcount = 0;
rotspeed = 0;
_root.Course.Char.yspeed = -17;
stayincentercount = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      throwcount++;
      if(throwcount > 20)
      {
         rotspeed += (11 - rotspeed) / 10;
         _rotation = _rotation - rotspeed;
      }
      if(_root.CameraZoom > 35)
      {
         _root.CameraZoom -= 2;
      }
      else if(_root.CameraZoom < 35)
      {
         _root.CameraZoom += 1;
      }
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed *= 0.95;
      if(_root.Course.Char.yspeed > -1)
      {
         _root.Course.Char.yspeed *= 0.9;
      }
      else
      {
         _root.Course.Char.yspeed -= _root.gravity * 0.9;
      }
      trace(_root.Course.Char._y - _root.Course.BackGFX._y);
      stayincentercount++;
      if(stayincentercount > 40 && stayincentercount < 190)
      {
         _root.Course.Char._y += (-275 - (_root.Course.Char._y - _root.Course.BackGFX._y)) / 10;
      }
      _root.Course.Char._x += (- (_root.Course.Char._x - _root.Course.BackGFX._x)) / 100;
      _root.Course.FrontGFX.Bowser.attack = true;
      _root.Course.FrontGFX.Bowser.attackFrame = "spin";
      if(_root.KeySPACE() && throwcount > 32 || _root.Course.Char._y - _root.Course.BackGFX._y > 0 && _root.Course.Char.yspeed > 0)
      {
         gotoAndStop("end");
         play();
      }
   }
};
