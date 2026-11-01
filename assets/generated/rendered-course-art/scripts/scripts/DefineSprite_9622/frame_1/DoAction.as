stop();
onEnterFrame = function()
{
   if(_root.StartCourse == true)
   {
      if(_root.CurrentPlayer == "Luigi")
      {
         gotoAndStop("Luigi");
         play();
      }
      if(_root.CurrentPlayer == "Mario")
      {
         gotoAndStop("Mario");
         play();
      }
      onEnterFrame = null;
   }
};
