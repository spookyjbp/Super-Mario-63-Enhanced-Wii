keycount = 0;
if(_root.lvl8keyA == true)
{
   keycount++;
}
if(_root.lvl8keyB == true)
{
   keycount++;
}
if(_root.lvl8keyC == true)
{
   keycount++;
}
if(_root.lvl8keyD == true)
{
   keycount++;
}
if(keycount >= 2)
{
   nextFrame();
}
stop();
