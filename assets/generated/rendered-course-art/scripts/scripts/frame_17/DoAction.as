stop();
StartCourseNochar("Level1",1200,0,1200,0,40,_root.CurrentPlayer,_root.Fluddpow,"Fromwhite");
_root.coursescale = 100;
speedX = 70;
zoom = false;
timer++;
onEnterFrame = function()
{
   _root.coursescale *= 0.99;
   camYspeed = 3.5;
   camXspeed = speedX -= 1;
   if(speedX < -50)
   {
      speedX *= 0.99;
   }
   if(speedX < -72)
   {
      _root.RemoveCourse();
      _root.gotoAndStop("Title");
   }
   CourseCodeNoChar();
};
