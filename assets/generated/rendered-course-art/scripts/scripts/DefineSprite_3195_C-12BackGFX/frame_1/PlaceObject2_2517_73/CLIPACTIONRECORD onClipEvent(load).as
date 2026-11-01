onClipEvent(load){
   staramount = 15;
   if(_parent.toaddisp20 == undefined)
   {
      _parent.toaddisp20 = random(2);
   }
   if(_parent.toaddisp20 == 0)
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
      Mood = "Mad";
      M1Moo = "Talk";
      M1Txt = "<b>....Right now...... I\'m exercising.....</b>";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Talk";
      M2Txt = "<b>.......So that when Bowser\'s Meteor comes........ I can run all the way......... to Bowser\'s Castle ....</b>";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talk";
      M3Txt = "<b>......You know..... just in case you fail....</b>";
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
      this._y = 500;
   }
}
