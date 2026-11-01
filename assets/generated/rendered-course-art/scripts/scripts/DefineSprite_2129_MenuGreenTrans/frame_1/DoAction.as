if(trans == true)
{
   _alpha = 0;
   onEnterFrame = function()
   {
      this._alpha += 10;
      if(_alpha >= 100)
      {
         _alpha = 100;
         _parent.gotoAndStop("Refresh");
         _root.CopyFile(file1,file2);
         _parent.gotoAndStop("Story");
         onEnterFrame = function()
         {
            this._alpha -= 10;
            if(_alpha <= 0)
            {
               _alpha = 0;
               this.removeMovieClip();
            }
         };
      }
   };
}
else
{
   this.swapDepths(0);
   this.removeMovieClip();
}
