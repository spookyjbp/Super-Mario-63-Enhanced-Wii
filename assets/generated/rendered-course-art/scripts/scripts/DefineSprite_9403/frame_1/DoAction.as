if(_root["File" + _parent.file + "TotalStars"] == 0 && _root.m.MenuRedTrans.file !== _parent.file)
{
   gotoAndStop(35);
}
else if(_root.m.MenuRedTrans.file !== _parent.file || _root.m.MenuRedTrans.YesF == null)
{
   stop();
}
