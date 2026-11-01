if(rotation == undefined)
{
   rotation = 0;
   rotspeed = 0;
   rot = 0;
   rot2 = 0;
}
else
{
   rot = rotation;
   rot2 = rotation;
}
if(speed == undefined)
{
   speed = 4;
}
if(rotdiv == undefined)
{
   rotdiv = 10;
}
if(rotmaxspeed == undefined)
{
   rotmaxspeed = 3;
}
destroyed = false;
if(_xscale < 0)
{
   _xscale = 100;
   rotation += 180;
   rot = rotation;
   rot2 = rotation;
   _yscale = _yscale * -1;
}
if(eat == undefined)
{
   eat = true;
}
if(chase == undefined)
{
   chase = true;
}
if(enemyhurt == undefined)
{
   enemyhurt = true;
}
gotoAndStop(frame);
counter = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
      {
         if(enemyhurt == true)
         {
            _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y,_yscale:35,_xscale:22,time:2});
         }
         if(Math.abs(_root.Course.Char._x - (_X + _parent._x)) < 100 && Math.abs(_root.Course.Char._y - (_Y + _parent._y)) < 100)
         {
            if((this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water)) && destroyed == false)
            {
               if(_root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Spin")
               {
                  destroyed = true;
                  _root.bgsoundFX = new Sound(this);
                  _root.bgsoundFX.attachSound("flippers");
                  _root.bgsoundFX.start(0,1);
                  _root.bgsoundFX.setVolume(_root.BgVolume);
               }
               if(_root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Ground")
               {
                  _root.Course.Char.yspeed = -10;
                  destroyed = true;
                  _root.bgsoundFX = new Sound(this);
                  _root.bgsoundFX.attachSound("flippers");
                  _root.bgsoundFX.start(0,1);
                  _root.bgsoundFX.setVolume(_root.BgVolume);
               }
            }
            if(this.body.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > -3 && destroyed == false)
            {
               _root.Course.Char.yspeed = -11;
               if(_root.KeyUP())
               {
                  _root.Course.Char.yspeed -= 2;
               }
               destroyed = true;
               _root.Power += 25;
               _root.Power = Math.min(_root.Power,_root.TotalPower);
               _root.bgsoundFX = new Sound(this);
               _root.bgsoundFX.attachSound("flippers");
               _root.bgsoundFX.start(0,1);
               _root.bgsoundFX.setVolume(_root.BgVolume);
            }
            if(this.body2.hitTest(_root.Course.Char.charClip.body) && !(this.body.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > -3) && destroyed == false)
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(1,90,-5,7,7,"");
               }
               else
               {
                  _root.MarioHit(1,90,-5,-7,7,"");
               }
               destroyed = true;
            }
         }
         if(destroyed == false)
         {
            rot2 = _root.radiansToAngle(Math.atan2(_root.Course.Char._y - (_Y + _parent._y),_root.Course.Char._x - (_X + _parent._x)));
         }
         else
         {
            rot2 += 15;
            rotation += 15;
            _Y = _Y + counter;
            counter++;
            if(counter > 32)
            {
               parent.total--;
               this.removeMovieClip(this);
            }
         }
         if(chase == true)
         {
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
         }
         _rotation = rotation;
         _root.moveObjectFromAngle(speed,rotation,this);
      }
      else
      {
         parent.total--;
         this.removeMovieClip(this);
      }
      if(eat !== false)
      {
         clip.play();
      }
      else
      {
         clip.stop();
      }
   }
   else
   {
      clip.stop();
   }
};
