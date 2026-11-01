onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(touch !== true)
      {
         _root.AreaTextClipF("Snowman\'s Land:\r<i>The Secret of the Igloo</i>",1);
         _root.CanSwitchFludd = false;
         _root.Fluddpow = "";
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("FluddSwitch");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
         touch = true;
      }
   }
}
