onClipEvent(load){
   if(_root.Starnum == 1)
   {
      staramount = 0;
      _xscale = -100;
      frame = "idle";
      Mood = "Sad";
      M1Moo = "Talk";
      M1Txt = "....For the past several years, the Bullies and Toads have lived peacefully among each other.";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Mad";
      M2Txt = "........But with the arrival of these 5 Shine Sprites, somehow the Bullies have turned evil.";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talk";
      M3Txt = "............The Bully-Toad War (initials are B.T.W.) started and has been raging on for sometime now....";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Happy";
      M4Txt = "...But I think an end is in sight!!!! You see......";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Talk";
      M5Txt = "......One of our Toad sqaudrons managed to inflitrate the Bully Tower;  he found this <b>Key</b>, it in doing so, got himself badly injured.";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Talk";
      M6Txt = "The Bully\'s Door is unlocked!  But we don\'t have anybody else to infiltrate";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M7Moo = "Talk";
      M7Txt = "<b>We need you to finally end this War.</b>\r<i>(Plus, you get to keep the Shine Sprite)</i>";
      M7Sta = function()
      {
         return true;
      };
      M7End = function()
      {
         return BP();
      };
   }
   else if(_root.Starnum == 2)
   {
      staramount = 0;
      _xscale = -100;
      frame = "idle";
      Mood = "Sad";
      M1Moo = "Talk";
      M1Txt = ".......Something\'s been going on with the Volcano recently....";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Mad";
      M2Txt = "........Could you go over there and check out what\'s going on?";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
   }
   else if(_root.Starnum == 5)
   {
      staramount = 0;
      _xscale = -100;
      frame = "idle";
      Mood = "Sad";
      M1Moo = "Talk";
      M1Txt = ".......There is a Secret Passage way in this fortress......";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Mad";
      M2Txt = "........It\'s location was passed down through generations, but eventually it got lost.";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talk";
      M3Txt = ".....I have no idea where the Secret of the Fortress is...";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
   }
   else
   {
      _xscale = -100;
      frame = "idle";
      Mood = "Sad";
      M1Moo = "Talk";
      M1Txt = ".......I\'m sorry, but there\'s nothing I can help you with.........";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
   }
}
