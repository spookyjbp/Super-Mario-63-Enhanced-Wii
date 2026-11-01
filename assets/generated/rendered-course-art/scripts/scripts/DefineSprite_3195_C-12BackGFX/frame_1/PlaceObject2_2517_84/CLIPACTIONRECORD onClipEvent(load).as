onClipEvent(load){
   staramount = 27;
   this._xscale = -100;
   Mood = "Sad";
   M1Moo = "Mad";
   M1Txt = "Mwahahahaha!!!! What are you doing in my Lair!?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Crazy";
   M2Txt = "...You dare Challange the All Powerful <b>MEWTWO</b> to a Poke-Match?!?!?!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = ".......Tremble at the face of my all powerful <i>PSYCHIC ABILITIES!!</i>";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = "........I Said <b>Tremble</b>!!!!!              .....For I am MEWTWO!!!!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M4F = function()
   {
      _root.Course.Char.charClip.frame.gotoAndPlay("no");
   };
   M5Moo = "Crazy";
   M5Txt = "....I COMMAND YOU TO OBEY ME NOW!!!!!!!";
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
      _root.Course.Char.charClip.frame.gotoAndPlay(1);
   };
   M6Moo = "Shocked";
   M6Txt = "..........WHAT??     You think that I\'m just <i>pretending</i> I\'m Mewtwo!?!?!";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
   M6F = function()
   {
      _root.Course.Char.charClip.frame.gotoAndPlay(1);
      sped = 8;
      OEF = function()
      {
         _X = _X + speed;
         speed -= 1;
         if(speed <= 0)
         {
            speed = undefined;
            OEF = null;
         }
      };
   };
   M7Moo = "Shocked";
   M7Txt = "........That I\'m <i>not really</i> the MASTER OF THE POKE-UNIVERSE?!?!?!";
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
      _root.Course.Char.charClip.frame.gotoAndPlay("yes");
   };
   M8Moo = "Mad";
   M8Txt = "...I\'LL SHOW YOU!!!!";
   M8Sta = function()
   {
      return true;
   };
   M8End = function()
   {
      return BP();
   };
   M8F = function()
   {
      _root.Course.Char.charClip.frame.gotoAndPlay(1);
   };
   M9Moo = "ShadowBall";
   M9Txt = "<b>.............SHADOW BALL!!!!!!!!!!!!!!!!!!!!!!!!!!!!</b>";
   M9Sta = function()
   {
      return true;
   };
   M9End = function()
   {
      return BP();
   };
   M9F = function()
   {
      _root.Course.Char.charClip.frame.gotoAndPlay("noo");
      _root.Course.FrontGFX.attachMovie("InvertWave","InvertWave" + depth,depth,{_x:_X,_y:_Y,speed:1});
      count2 = 0;
      OEF = function()
      {
         count2++;
         if(count2 > 15)
         {
            _root.Course.FrontGFX.attachMovie("InvertWave","InvertWave" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X,_y:_Y,speed:1});
            count2 = 0;
         }
      };
   };
   M10Moo = "Confused";
   M10Txt = "-fatal-error-occured-.........-forced-shut-down-of-system-........-error-M3W2-occured-";
   M10Sta = function()
   {
      return true;
   };
   M10End = function()
   {
      return BP();
   };
   M10F = function()
   {
      _root.Course.Char.charClip.frame.gotoAndPlay(1);
      OEF = null;
   };
   M11Moo = "Mad";
   M11Txt = "...Aw Darn it.........Not again........ I ran out of <i>Pokemon Power!!</i>";
   M11Sta = function()
   {
      return true;
   };
   M11End = function()
   {
      return BP();
   };
   M12Moo = "Crazy";
   M12Txt = "....TELEPORT!!!!";
   M12Sta = function()
   {
      return true;
   };
   M12End = function()
   {
      return BP();
   };
   M13Moo = "Crazy";
   M13Txt = " ";
   M13Sta = function()
   {
      return true;
   };
   M13End = function()
   {
      return true;
   };
   M13F = function()
   {
      OEF = function()
      {
         _alpha = _alpha - 6;
         _root.CameraZoom -= 9;
         if(_alpha <= 0)
         {
            _root.CameraZoom = 100;
            _root.ToadMessage = "";
            _parent.toaddisp1 = true;
            this._y = 500;
            M1Moo = undefined;
            M1Txt = undefined;
            _root.playpause = false;
            _root.Course.Char.attack = false;
            Phase = 9000;
            onEnterFrame = null;
            this.swapDepths(0);
            this.removeMovieClip();
            OEF = null;
         }
      };
   };
}
