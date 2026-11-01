onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.Power += 3;
         if(savegrav == undefined)
         {
            savegrav = _root.gravity;
            _root.gravity *= 0.5;
         }
      }
   }
   else if(savegrav !== undefined)
   {
      _root.gravity = savegrav;
      savegrav = undefined;
   }
}
