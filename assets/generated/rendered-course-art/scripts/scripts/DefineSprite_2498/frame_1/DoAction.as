ChangeFrame = function()
{
   gotoAndPlay(_parent._parent.Mood);
};
Skip = function(a)
{
   gotoAndPlay(random(a) + _currentframe);
};
ChangeFrame();
if(_parent._parent.Mood == undefined)
{
   gotoAndStop(2);
}
