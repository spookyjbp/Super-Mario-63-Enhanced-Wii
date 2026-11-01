onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body) && _parent.musicswitch == true)
   {
      _parent.musicswitch = false;
      _root.SongIntro = "BowserTrapIntro";
      _root.SongRepeat = "BowserTrapRepeat";
      _root.PlayMusicAndIntro();
   }
}
