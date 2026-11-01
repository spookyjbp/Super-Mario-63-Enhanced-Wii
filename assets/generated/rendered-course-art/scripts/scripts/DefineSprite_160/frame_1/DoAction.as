stop();
onEnterFrame = function()
{
   frame = Math.round(_root.Waterpercent * 5);
   frame = Math.max(frame,2);
   frame = Math.min(frame,500);
   cl.gotoAndStop(frame);
   percentnum = Math.round(_root.Waterpercent);
   powernum = Math.round(_root.Power / _root.TotalPower * 100) + 1;
   if(_root.Fluddpow == "")
   {
      gotoAndStop(2);
      onEnterFrame = function()
      {
         if(_root.Fluddpow !== "")
         {
            gotoAndStop(1);
         }
      };
   }
};
