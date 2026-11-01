stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.playpause == false)
      {
         if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.KeyZ() && !_root.KeyRIGHT() && !_root.KeyLEFT())
         {
            _root.Course.Char.yspeed = 0;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.attack = true;
            _root.Course.Char.attackFrame = "Sign";
            onEnterFrame = function()
            {
               aa._alpha += (- aa._alpha) / 3;
               if(_root.PauseGame == false)
               {
                  _root.playpause = true;
                  X = _X + _root.Course.BackGFX._x;
                  _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
                  _root.Camcenter = this;
               }
            };
            play();
         }
      }
      if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && !_root.KeyRIGHT() && !_root.KeyLEFT() && _root.playpause == false)
      {
         aa.aa = 100;
      }
      else
      {
         aa.aa = 0;
      }
      aa._alpha += (aa.aa - aa._alpha) / 3;
   }
};
