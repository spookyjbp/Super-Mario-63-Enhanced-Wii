count++;
if(count < 12)
{
   gotoAndStop("repeat");
   play();
}
else
{
   _root.Course.Char.attack = true;
   _root.Course.Char.attackFrame = "Sleep";
}
