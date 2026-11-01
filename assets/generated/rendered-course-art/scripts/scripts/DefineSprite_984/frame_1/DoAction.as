if(_parent.picture == false)
{
   stop();
}
else
{
   gotoAndStop(2);
   this.attachMovie("PictureClip","PictureClip",0,{_xscale:9,_yscale:9,mini:true});
   count = 0;
   onEnterFrame = function()
   {
      if(count++ > 3)
      {
         this.PictureClip._width = 25;
         this.PictureClip._yscale = this.PictureClip._xscale;
         onEnterFrame = null;
         delete onEnterFrame;
      }
   };
   PictureClip.gotoAndStop(_parent.signmessage);
}
