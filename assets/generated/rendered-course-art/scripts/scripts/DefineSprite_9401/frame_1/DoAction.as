num = 2;
i = 1;
while(i < 4)
{
   if(_root["File" + _parent._parent.file + "BowserKey" + i] == true)
   {
      num++;
   }
   i++;
}
gotoAndStop(num);
