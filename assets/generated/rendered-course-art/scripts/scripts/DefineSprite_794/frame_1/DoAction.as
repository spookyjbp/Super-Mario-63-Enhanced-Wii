clicked = false;
Trans = function(a)
{
   fr = a;
   trans.play();
};
if(_root.Starnamenum == 0)
{
   level = "Castle Grounds";
}
else
{
   level = "Secret Course";
}
if(_root.Playinglevel == 1)
{
   level = "Bob-omb Battle Field";
}
else if(_root.Playinglevel == 2)
{
   level = "Snowmans Land";
}
else if(_root.Playinglevel == 3)
{
   level = "Hazy Maze Cave";
}
else if(_root.Playinglevel == 4)
{
   level = "Boo\'s Mansion";
}
else if(_root.Playinglevel == 5)
{
   level = "Lethal Lava Land";
}
else if(_root.Playinglevel == 6)
{
   level = "Tall Tall Mountain";
}
else if(_root.Playinglevel == 7)
{
   level = "Raindbow Ride";
}
else if(_root.Playinglevel == 8)
{
   level = "Bowsers Castle";
}
else if(_root.Playinglevel == 9)
{
   level = "Sinking Sand Land";
}
else if(_root.Playinglevel == 10)
{
   level = "Wet Dry World";
}
else if(_root.Playinglevel == 11)
{
   level = "Tick Tock Clock";
}
star = "<b>" + _root.StarNameArray[_root.Starnamenum];
desc = _root.StarDescArray[_root.Starnamenum];
gotoAndStop("default");
