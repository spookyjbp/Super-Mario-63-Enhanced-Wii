onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body) && _parent.musicswitch !== true)
   {
      _parent.musicswitch = true;
      _root.SongIntro = "HazyMazeCave-Haze";
      _root.SongRepeat = "HazyMazeCave-Haze";
      _root.PlayMusicAndIntro();
   }
}
