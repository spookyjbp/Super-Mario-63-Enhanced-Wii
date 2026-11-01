onClipEvent(load){
   starfunction = function()
   {
      _root.StartNewMusicAndIntroNoVar("MetalCap - Intro","MetalCap - Repeat");
      if(isNaN(_parent.Time))
      {
         _parent.Time = 30;
      }
      _root.PowerTimer = _parent.Time * 32;
      _root.Metal = true;
   };
}
