onClipEvent(load){
   staramount = 0;
   if(_parent.toaddisp20 == undefined)
   {
      _parent.toaddisp20 = random(2);
   }
   if(_parent.toaddisp20 == 1)
   {
      Mood = "Smile";
      M1Moo = "Talk";
      if(_root.BowserKey2 == true || _root.CheatCastleUnlock == true)
      {
         M1Txt = "...Bowser is just beyond this door...";
      }
      else
      {
         M1Txt = "...Bowser must be hiding upstairs...";
      }
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Fra = "walk";
      M2Moo = "Happy";
      M2Txt = "<b>Go defeat him and save this Castle!</b>";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M4Moo = "Sad";
      M4Txt = "...I\'m Jealous of you...";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
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
