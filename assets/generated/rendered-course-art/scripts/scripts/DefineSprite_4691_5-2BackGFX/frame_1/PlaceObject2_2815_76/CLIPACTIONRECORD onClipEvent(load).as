onClipEvent(load){
   StarCoinNum = 26;
   saveX = _X - 1350;
   saveY = _Y - 30;
   saverot = Math.atan2(saveY,saveX);
   distance = Math.sqrt(saveX * saveX + saveY * saveY);
}
