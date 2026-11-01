onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body) && _root._quality !== "LOW")
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         if(savezoom == undefined)
         {
            savezoom = _root.CameraZoom;
         }
         if(_root.CameraZoom > 65)
         {
            _root.CameraZoom -= 1;
         }
      }
   }
   else if(savezoom !== undefined)
   {
      if(_root.CameraZoom < savezoom)
      {
         _root.CameraZoom += 1;
      }
      else
      {
         savezoom = undefined;
      }
   }
}
