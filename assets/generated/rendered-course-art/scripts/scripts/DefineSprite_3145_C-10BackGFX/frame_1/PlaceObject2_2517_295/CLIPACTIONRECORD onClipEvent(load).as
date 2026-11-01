onClipEvent(load){
   staramount = 15;
   if(_parent.toaddisp10 == undefined)
   {
      _parent.toaddisp10 = random(2);
   }
   if(_parent.toaddisp10 == 0)
   {
      count2 = 0;
      speed = 2;
      OEF2 = function()
      {
         _X = _X + speed;
         count2++;
         if(count2 > 60)
         {
            count2 = 0;
            speed *= -1;
         }
         _xscale = speed * 50;
      };
      frame = "walk";
      Mood = "Crazy";
      M1Moo = "Crazy";
      M1Txt = "<b>....Ahhh!!!</b>";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Crazy";
      M2Txt = "<b>.......I touched a flame, and so I\'m uncontrollably hopping and running around!!</b>";
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
      this._y = 500;
   }
}
