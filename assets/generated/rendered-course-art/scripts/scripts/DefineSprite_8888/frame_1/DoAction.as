stop();
num = Number(_name.charAt(1));
gotoAndStop(num);
array = [0,10,13,21,25,28,38,43];
trace(array[num]);
if(_root.MaxStarCoins >= array[num])
{
   greuy._visible = false;
}
else
{
   _alpha = 75;
   greuy._visible = true;
}
