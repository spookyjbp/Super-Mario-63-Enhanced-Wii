onClipEvent(load){
   t = "Y";
   amount = 50;
   speed = 5;
   FindTrigger = function()
   {
      Trigger = this.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.yspeed < 3;
   };
}
