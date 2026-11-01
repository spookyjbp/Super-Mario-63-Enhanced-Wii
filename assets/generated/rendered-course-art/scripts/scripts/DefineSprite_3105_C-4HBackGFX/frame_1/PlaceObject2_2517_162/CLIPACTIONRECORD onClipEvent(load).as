onClipEvent(load){
   staramount = 25;
   _xscale = -100;
   if(_parent.toaddisp10 == undefined)
   {
      _parent.toaddisp10 = random(2);
   }
   if(_parent.toaddisp10 == 1)
   {
      Mood = "Smile";
      M1Moo = "Mad";
      M1Txt = ".....AAaaAAAaaa..... AAaaAAAAaaA!!!!";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Crazy";
      M2Txt = "<b>AAAAaaaAaAAaChCHCHCHOOOOOO!!!!!!</b>";
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
         _root.Course.Char.charClip.frame.gotoAndPlay("noo");
      };
      M3Moo = "Mad";
      M3Txt = "Um.... Sorry about that....";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M3F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay(1);
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
      M7Moo = "Shocked";
      M7Txt = "..................You know,";
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
   }
   else
   {
      this._y = 500;
   }
}
