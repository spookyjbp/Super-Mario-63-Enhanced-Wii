onClipEvent(load){
   if(_root.StarCoin[61] !== true)
   {
      staramount = 0;
      Mood = "Sleep";
      M1Moo = "Sleep";
      M1Txt = ".....Zzzz...Zzzzz..Zzzz";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Wake-up";
      M2Txt = "....Huh?......";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M2F = function()
      {
         this._xscale *= -1;
      };
      M3Moo = "Shocked";
      M3Txt = "The Blocks!!!";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Shocked";
      M4Txt = "They are broken!!!!!";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Crazy";
      M5Txt = "I\'ve been trapped in here for over 9,000 years!!!!!!";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M5F = function()
      {
         count2 = 0;
         _xscale = _xscale * -1;
      };
      M6Moo = "Happy";
      M6Txt = "And you just saved me!!!";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M7Fra = "walk";
      M7Moo = "Happy";
      M7Txt = "YES!!!!";
      M7Sta = function()
      {
         return true;
      };
      M7End = function()
      {
         return BP();
      };
      M7F = function()
      {
         this._xscale = -100;
      };
      M8Moo = "Happy";
      M8Txt = "*runs off laughing*";
      M8Sta = function()
      {
         return true;
      };
      M8End = function()
      {
         if(this._x < 100)
         {
            return true;
         }
      };
      M8F = function()
      {
         OEF = function()
         {
            this._x -= 4;
            if(_root.CameraZoom > 100)
            {
               _root.CameraZoom = _root.CameraZoom - 1;
            }
            if(this._x < 100)
            {
               _root.CameraZoom = 100;
               _root.playpause = false;
               _root.Course.Char.attack = false;
               _root.ToadMessage = "";
               this.swapDepths(0);
               this.removeMovieClip();
               _visible = false;
               onEnterFrame = null;
            }
         };
      };
   }
   else
   {
      Mood = "Happy";
      BP = null;
      _xscale = 100;
      onEnterFrame = null;
      M1Moo = undefined;
      M1Txt = undefined;
      M1Sta = null;
      M1End = null;
      bz.swapDepths(1);
      bz.removeMovieClip();
      bz._y = 1000;
      _X = -376;
      _Y = 42;
   }
}
