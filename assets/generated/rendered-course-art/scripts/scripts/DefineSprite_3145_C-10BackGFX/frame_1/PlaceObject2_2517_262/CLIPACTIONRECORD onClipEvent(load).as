onClipEvent(load){
   if(random(2) == 0 && _parent.toaddisp1 == undefined)
   {
      staramount = 0;
      Mood = "Smile";
      M1Moo = "Happy";
      M1Txt = "(Singing) Spider-Toad! Spider-Toad!";
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
      M2Txt = "Does whatever a Spider-Toad can!";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
   }
   else
   {
      _parent.toaddisp1 = true;
      _Y = 500;
   }
}
