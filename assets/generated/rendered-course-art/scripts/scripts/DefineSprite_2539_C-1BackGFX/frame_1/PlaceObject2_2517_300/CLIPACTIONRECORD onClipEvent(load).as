onClipEvent(load){
   if(random(2) == 0 && _parent.toadmove4 == undefined)
   {
      this._y = -172;
      _parent.toadmove4 = true;
   }
   staramount = 15;
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
   Mood = "Mad";
   M1Moo = "Crazy";
   M1Txt = "....Oh No!!!!....";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = ".......I was told to deliver this letter to General Pepper, Commander-in-Cheif of the Cornerian Defense Forces.....";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Crazy";
   M3Txt = "......But I have no idea what a Corneria is!!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = "......................My only hint was to \"Do a Barrel Roll!\" .................... What does that Mean!?!?!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
