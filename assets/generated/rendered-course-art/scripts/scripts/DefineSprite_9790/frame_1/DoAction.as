N = function(fr)
{
   fra = fr;
   trans.gotoAndPlay(2);
   count = 0;
   onEnterFrame = function()
   {
      count++;
      if(count >= 10)
      {
         if(fra == "LD")
         {
            _root.LDMenuFrame = 1;
            _root.EraseLevel();
            _root.CreateLevelDesigner();
         }
         if(fra == "Lev")
         {
            _root.LDMenuFrame = 2;
            gotoAndStop("Lev");
         }
         if(fra == "TS")
         {
            _root.LDMenuFrame = 1;
            _root.gotoAndStop("Title");
         }
         if(fra == "Sha")
         {
            gotoAndStop("Sha");
         }
         onEnterFrame = null;
      }
   };
};
stop();
if(firstrun == undefined)
{
   if(_root.LDMenuFrame !== undefined)
   {
      gotoAndStop(_root.LDMenuFrame);
   }
   firstrun = true;
}
if(_root.Page == undefined)
{
   _root.Page = 0;
   _root.Order = "newest";
}
