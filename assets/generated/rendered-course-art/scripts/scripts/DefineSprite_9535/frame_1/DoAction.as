this._name = "m";
TransitionF = function(framy)
{
   if(framy == "Delete")
   {
      depth = this.getNextHighestDepth();
      this.attachMovie("MenuRedTrans","MenuRedTrans",depth,{trans:true,_x:0,_y:0});
   }
   else if(framy == "Play")
   {
      depth = this.getNextHighestDepth();
      this.attachMovie("MenuBlackTrans","MenuBlackTrans" + depth,depth,{trans:true,_x:0,_y:0});
   }
   else
   {
      depth = this.getNextHighestDepth();
      this.attachMovie("MenuTransition","MenuTransition" + depth,depth,{trans:true,_x:0,_y:200});
      GSF = function()
      {
         gotoAndStop(framy);
         clicked = false;
      };
   }
};
SpecialDeleteFile = function(file)
{
   _root.m.filem.HL = undefined;
   _root.m.filem[file].em.gotoAndPlay(2);
};
Message = function(a, color, important)
{
   if(messageonscreen == undefined)
   {
      messageonscreen = 0;
   }
   if(messageonscreen < 3 || important !== false)
   {
      if(color == undefined)
      {
         color = red;
      }
      depth = _root.m.getNextHighestDepth();
      _root.m.attachMovie("MenuMessage","MenuMessage" + depth,depth,{_x:0,_y:200,txt:a,color:color});
      messageonscreen++;
   }
};
stop();
