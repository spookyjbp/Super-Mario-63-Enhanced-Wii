st = 1;
unknown = true;
num = 0;
i = 0;
while(i < 5)
{
   if(_root.Star[st + i] == true)
   {
      unknown = false;
      num++;
   }
   i++;
}
if(unknown == true)
{
   stop();
}
else
{
   gotoAndStop(2);
}
if(num == 5)
{
   i = 0;
   while(i < 5)
   {
      _parent[st + i].shine = true;
      _parent[st + i].gotoAndStop(3);
      i++;
   }
}
