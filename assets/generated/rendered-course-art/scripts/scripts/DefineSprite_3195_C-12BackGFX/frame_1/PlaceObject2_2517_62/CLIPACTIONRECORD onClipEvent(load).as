onClipEvent(load){
   staramount = 21;
   _X = _X + random(30);
   if(_parent.toaddisp10 == undefined)
   {
      _parent.toaddisp10 = random(2);
   }
   if(_parent.toaddisp10 == 0)
   {
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "That clock over there is unsually large....";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Talk";
      M2Txt = "I heard the Princess got it imported over from some strange magical place.";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talk";
      M3Txt = "And so the clock has lots of weird, strange properties to it, almost as if there is an entire world inside it!";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M5Moo = "Mad";
      M5Txt = "....Mine didn\'t get first, in fact, no <b>Pun in Ten did</b>....";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Happy";
      M6Txt = ".........................";
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
         _root.Course.Char.charClip.frame.gotoAndPlay("no");
      };
      M7Moo = "Crazy";
      M7Txt = "<b>.....................Why Aren\'t you Laughing?!?!?</b>";
      M7Sta = function()
      {
         return true;
      };
      M7End = function()
      {
         return BP();
      };
      M9Moo = "Mad";
      M9Txt = "I thought you were a nice person!!";
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
      };
      M10Moo = "Sad";
      M10Txt = "And then you come here and Destroyed all my Dreams!";
      M10Sta = function()
      {
         return true;
      };
      M10End = function()
      {
         return BP();
      };
      M11Moo = "Crazy";
      M11Txt = "DESTROYED IT!!!!";
      M11Sta = function()
      {
         return true;
      };
      M11End = function()
      {
         return BP();
      };
      M11F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay("noo");
      };
      M12Moo = "Mad";
      M12Txt = "......";
      M12Sta = function()
      {
         return true;
      };
      M12End = function()
      {
         return BP();
      };
      M12F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay("talk");
      };
      M13Moo = "Mad";
      M13Txt = "...Gee, Thanks for ruining my life...";
      M13Sta = function()
      {
         return true;
      };
      M13End = function()
      {
         return BP();
      };
      M13F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay(1);
         this._xscale *= -1;
         Mood = "Mad";
         StartMood = "Mad";
         M1Moo = "Mad";
         M1Txt = "......Go Away!!";
         M1Sta = function()
         {
            return BP();
         };
         M1End = function()
         {
            return BP();
         };
         M2Moo = undefined;
         M2Txt = undefined;
         M2Sta = null;
         M2End = null;
      };
   }
   else
   {
      this._y = 500;
   }
}
