Explode = function()
{
   if(Math.abs(_root.Course.Char._x - (_X + _parent._x)) < 500 && Math.abs(_root.Course.Char._y - (_Y + _parent._y)) < 400)
   {
      _root.bgsoundFX = new Sound(this);
      _root.bgsoundFX.attachSound("Explode9");
      _root.bgsoundFX.start(0,1);
      _root.bgsoundFX.setVolume(_root.BgVolume);
   }
   _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y,count:300});
   _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y,count:300});
   _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y,count:300});
   xspeed = 0;
   yspeed = 0;
   this.play();
   onEnterFrame = function()
   {
      if(_root.PauseGame !== true)
      {
         if(this.body.hitTest(_root.Course.Char.charClip.body))
         {
            if(_root.Course.Char.attackFrame !== "SlingStar" && _root.Course.Char.attackFrame !== "JumpSpin" || _root.Course.Char.attack == false)
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(1,90,-5,7,7,"");
               }
               else
               {
                  _root.MarioHit(1,90,-5,-7,7,"");
               }
            }
         }
         play();
      }
      else
      {
         stop();
      }
   };
};
