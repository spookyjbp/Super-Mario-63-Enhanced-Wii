realqual = _root._quality;
onEnterFrame = function()
{
   if(qual !== undefined)
   {
      if(tempqual == undefined)
      {
         tempqual = _root._quality;
      }
      _root._quality = qual;
   }
   else if(tempqual !== undefined)
   {
      _root._quality = tempqual;
      tempqual = undefined;
   }
};
onMouseDown = function()
{
   if(qual !== undefined)
   {
      _root._quality = qual;
      realqual = qual;
      qual = undefined;
   }
   tempqual = undefined;
};
