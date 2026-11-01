onClipEvent(load){
   Mood = "Shocked";
   M1Moo = "Happy";
   M1Fra = "walk";
   M1Txt = "<b>YES!!!</b>";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Fra = "idle";
   M2Moo = "Happy";
   M2Txt = "I\'ve been trapped in <b>Bowser\'s Secret Lab</b> for 3 years!  I\'m finally rescued!!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Fra = "idle";
   M3Moo = "Talk";
   M3Txt = "Bowser was just about to experiment on me!  I\'m so glad I could get away before I get mutated...";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Fra = "idle";
   M4Moo = "Talk";
   M4Txt = "Here, give me some of your Invisibility Cap, and I\'ll be able to walk through these bars.";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Fra = "idle";
   M5Moo = "Talk";
   M5Txt = "There we go ....     ...        ..  .....   ..  ....  .   .......  ..  .  Oh!   Too Much Invisibility!";
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
      _root.Course.Char.charClip.frame.gotoAndPlay("talk");
      OEF = function()
      {
         if(this._alpha > 0)
         {
            this._alpha -= 2;
         }
      };
   };
   M6Fra = "idle";
   M6Moo = "Talk";
   M6Txt = "Oh well, I can still get out of here...    ...";
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
      _visible = false;
      _X = _X + 70;
      OEF = function()
      {
         if(_X < 1600)
         {
            _X = _X + 1;
         }
      };
   };
   M7Fra = "idle";
   M7Moo = "Talk";
   M7Txt = "...  Oh! And before I forget...";
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
      _visible = false;
      OEF = function()
      {
         if(_X < 1600)
         {
            _X = _X + 1;
         }
      };
   };
   M8Fra = "idle";
   M8Moo = "Talk";
   M8Txt = "There was an <b>Invisible Platform</b> here all along;  ...   ..   .... Good-Bye";
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
      _root.Course.BackGFX.gotoAndStop(4);
      OEF = null;
      _X = 1535;
      _Y = 920;
   };
   M9Fra = "idle";
   M9Moo = "Talk";
   M9Txt = "blah";
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
      _root.CameraZoom = 100;
      _root.ToadMessage = "";
      _parent.toaddisp1 = true;
      this._y = 1000;
      M1Moo = undefined;
      M1Txt = undefined;
      _root.playpause = false;
      _root.Course.Char.attack = false;
      Phase = 9000;
      onEnterFrame = null;
      this.swapDepths(0);
      this.removeMovieClip();
      OEF = null;
   };
}
