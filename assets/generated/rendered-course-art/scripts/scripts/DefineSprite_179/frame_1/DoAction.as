CloseFunction = null;
onEnterFrame = function()
{
   if(_parent.mm.clip._currentframe < 5)
   {
      touch = false;
      i = 0;
      while(i < 4)
      {
         if(this["q" + i].hitTest(_root._xmouse,_root._ymouse,false) && touch == false)
         {
            this["q" + i].larger = true;
            touch = true;
         }
         else
         {
            this["q" + i].larger = false;
         }
         i++;
      }
      this["q" + _parent.qnum].larger = true;
   }
   i = 0;
   while(i < 4)
   {
      if(this["q" + i].larger == true && _parent.mm.clip._currentframe < 5)
      {
         this["q" + i]._xscale += (130 - this["q" + i]._xscale) / 5;
         this["q" + i]._yscale = this["q" + i]._xscale;
      }
      else
      {
         this["q" + i]._xscale += (100 - this["q" + i]._xscale) / 5;
         this["q" + i]._yscale = this["q" + i]._xscale;
      }
      i++;
   }
   CloseFunction();
};
onMouseDown = function()
{
   touch = false;
   i = 0;
   while(i < 4)
   {
      if(this["q" + i].hitTest(_root._xmouse,_root._ymouse,false) && touch == false && _parent.mm.clip._currentframe < 5)
      {
         _root._quality = _root.QualityArray[i];
         _parent.qnum = i;
         touch = true;
      }
      i++;
   }
   if(_parent.mm.clip.hitTest(_root._xmouse,_root._ymouse,false) && touch == false && CloseFunction == null)
   {
      if(_parent.mm.clip._currentframe < 5)
      {
         CloseFunction = function()
         {
            this._y += 1;
            _parent.mm._y = this._y;
            _parent.mm.clip.nextFrame();
            if(_parent.mm.clip._currentframe == 22)
            {
               CloseFunction = null;
            }
         };
      }
      else
      {
         CloseFunction = function()
         {
            this._y -= 1;
            _parent.mm._y = this._y;
            _parent.mm.clip.prevFrame();
            if(_parent.mm.clip._currentframe == 1)
            {
               CloseFunction = null;
            }
         };
      }
   }
};
