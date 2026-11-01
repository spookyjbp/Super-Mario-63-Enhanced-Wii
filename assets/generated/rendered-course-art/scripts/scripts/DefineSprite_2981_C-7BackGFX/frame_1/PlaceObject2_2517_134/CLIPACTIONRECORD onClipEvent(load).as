onClipEvent(load){
   staramount = 15;
   if(_parent.toaddisp10 == undefined)
   {
      _parent.toaddisp10 = random(2);
   }
   if(_parent.toaddisp10 == 0)
   {
      Mood = "Sad";
      M1Moo = "Talk";
      M1Txt = "I heard that there\'s a Secret Hallway around here.";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Happy";
      M2Txt = "But the thing is, this hallway is <b>Vertical</b>!!";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Happy";
      M3Txt = "And so I have no idea where it is!";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M5Moo = "Mad";
      M5Txt = "Why are you staring at me?!?!?!?";
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
      M6Moo = "Crazy";
      M6Txt = "<b>Are you trying to annoy me?</b>";
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
      M7Moo = "Shocked";
      M7Txt = "..........................You know,";
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
         _root.Course.Char.charClip.frame.gotoAndPlay(1);
      };
      M8Moo = "Sad";
      M8Txt = "I once looked up to you, " + _root.CurrentPlayer;
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
