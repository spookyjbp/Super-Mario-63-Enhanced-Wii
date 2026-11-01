onClipEvent(load){
   if(random(2) == 0 && _parent.toaddisp3 == undefined)
   {
      staramount = 18;
      count2 = 0;
      speed = 2;
      OEF2 = function()
      {
         _X = _X + speed;
         _Y = _Y - speed;
         count2++;
         if(count2 > 32)
         {
            count2 = 0;
            speed *= -1;
         }
         _xscale = speed * 50;
      };
      frame = "walk";
      Mood = "Smile";
      M1Moo = "Sad";
      M1Txt = "........*pant....*pant*....";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Talking";
      M2Txt = ".......You know, Running up and down stairs is great exercise......";
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
      _parent.toaddisp3 = true;
   }
}
