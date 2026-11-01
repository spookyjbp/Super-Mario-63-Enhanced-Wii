if(staramount == undefined)
{
   staramount = 0;
}
if(frame == undefined)
{
   frame = "idle";
}
if(Mood == undefined)
{
   Mood = "Smile";
}
if(M1Moo == undefined)
{
   M1Moo = "Talk";
   M1Fra = "idle";
   M1Txt = "Hello, I am a Toad.";
   M1Sta = function()
   {
      return BP;
   };
   M1End = function()
   {
      return BP;
   };
   M1F = function()
   {
      count2 = 0;
      _xscale = _xscale * -1;
      OEF = function()
      {
         count2++;
         if(count2 > 16)
         {
            count2 = 0;
            _xscale = _xscale * -1;
            OEF = function()
            {
               count2++;
               if(count2 > 16)
               {
                  count2 = 0;
                  _xscale = _xscale * -1;
                  OEF = function()
                  {
                     count2++;
                     if(count2 > 16)
                     {
                        count2 = undefined;
                        _xscale = _xscale * -1;
                        OEF = null;
                     }
                  };
               }
            };
         }
      };
   };
}
HitTestF = function()
{
   if(count > 4)
   {
      return this.box.hitTest(_root.Course.Char.charClip.body) || this.box.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true);
   }
};
BP = function()
{
   if(z == 2)
   {
      return true;
   }
   return false;
};
StartMood = Mood;
Startframe = frame;
Moo = Mood;
Phase = 1;
action = false;
z = 0;
if(OEF == undefined)
{
   OEF = null;
}
if(standnext == undefined)
{
   standnext = true;
}
talking = false;
camerazoom = false;
count = 0;
if(onEnterFrame == undefined)
{
   onEnterFrame = function()
   {
      if(count < 32)
      {
         count++;
      }
      OEF();
      if(this["M" + Phase + "Txt"] == undefined)
      {
         Phase = 1;
         _root.playpause = false;
         _root.Course.Char.attack = false;
         Mood = StartMood;
         frame = Startframe;
         talking = false;
      }
      if(HitTestF() && _root.playpause == false && (_root.Course.Char.air == false || _root.Course.Char.Water == true) && _root.Course.Char.attack == false)
      {
         bz.aa = 100;
      }
      else
      {
         bz.aa = 0;
      }
      if(_root.KeyZ())
      {
         if(z == 0)
         {
            z = 2;
         }
         else
         {
            z = 1;
         }
      }
      else
      {
         z = 0;
      }
      if(this["M" + Phase + "Sta"]() == true && action == false && _root.ToadMessage == "" && (_root.playpause == false && (bz._alpha >= 98 || standnext == false) || Phase !== 1))
      {
         _root.ToadMessage = this["M" + Phase + "Txt"];
         talking = true;
         Mood = this["M" + Phase + "Moo"];
         if(this["M" + Phase + "Fra"] !== undefined)
         {
            frame = this["M" + Phase + "Fra"];
         }
         if(this["M" + Phase + "F"] !== undefined)
         {
            this["M" + Phase + "F"]();
         }
         action = true;
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "Talk";
         _root.Course.Char.yspeed = 0;
         _root.Course.Char.xspeed = 0;
         _root.playpause = true;
         if(Phase == 1)
         {
            if(_root.Course.Char._x > this._x + _root.Course.BackGFX._x)
            {
               this._xscale = 100;
               _root.Course.Char._xscale = 100;
            }
            else
            {
               this._xscale = -100;
               _root.Course.Char._xscale = -100;
            }
         }
      }
      else if(this["M" + Phase + "End"]() && (_root.Stats.tm.txt == _root.ToadMessage && _root.Stats.tm.bz._alpha >= 98) && action == true)
      {
         action = false;
         _root.ToadMessage = "";
         Phase += 1;
      }
      if(talking == true)
      {
         _root.Camcenter = this;
         if(this.body.hitTest(_root.Course.Char.charClip.body) || this.body.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(_root.Course.Char._x > this._x + _root.Course.BackGFX._x)
            {
               _root.Course.Char._x += 2;
            }
            else
            {
               _root.Course.Char._x -= 2;
            }
         }
         if(_root.CameraZoom < 150)
         {
            _root.CameraZoom += 3;
            camerazoom = true;
         }
         else
         {
            _root.CameraZoom = Math.min(_root.CameraZoom + 3,150);
         }
      }
      else if(camerazoom == true)
      {
         if(_root.CameraZoom > 100)
         {
            _root.CameraZoom -= 3;
         }
         else
         {
            _root.CameraZoom = 100;
            camerazoom = false;
         }
      }
      gotoAndStop(frame);
      if(Moo !== Mood)
      {
         body.head.ChangeFrame();
      }
      Moo = Mood;
      OEF2();
   };
}
if(_root.TotalStars < staramount && _root.CheatCastleUnlock !== true)
{
   onEnterFrame = null;
   this.swapDepths(0);
   this.removeMovieClip();
}
