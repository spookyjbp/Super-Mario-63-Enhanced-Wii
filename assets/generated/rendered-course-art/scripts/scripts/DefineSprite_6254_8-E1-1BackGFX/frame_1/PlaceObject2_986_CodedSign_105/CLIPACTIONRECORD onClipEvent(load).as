onClipEvent(load){
   read = false;
   count = 0;
   picture = true;
   signmessage = "Start";
   depth = _root.Course.BackGFX.getNextHighestDepth();
   _root.Course.BackGFX.createEmptyMovieClip("sign2center" + depth,depth);
   _root.Course.BackGFX["sign2center" + depth]._x = _root.Course.BackGFX.sign2._x;
   _root.Course.BackGFX["sign2center" + depth]._y = _root.Course.BackGFX.sign2._y - 60;
   othercamcenter = _root.Course.BackGFX["sign2center" + depth];
   waiting = 0;
   replacey = false;
}
