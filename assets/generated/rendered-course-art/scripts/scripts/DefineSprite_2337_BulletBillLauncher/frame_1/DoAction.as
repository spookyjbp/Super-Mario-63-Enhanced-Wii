stop();
if(waittime == undefined)
{
   waittime = 828;
}
if(addrandomly == undefined)
{
   addrandomly = false;
}
if(limit == undefined)
{
   if(chase == true)
   {
      limit = 3;
   }
   else
   {
      limit = "none";
   }
}
if(onlyright == undefined)
{
   onlyright = false;
}
if(onlyleft == undefined)
{
   onlyleft = false;
}
if(lockedside == undefined)
{
   lockedside = false;
}
if(enemyhurt == undefined)
{
}
if(firstrun == undefined)
{
   total = 0;
   if(offset !== undefined)
   {
      count = offset;
   }
   else
   {
      count = 0;
   }
   firstrun = true;
}
else
{
   count = 0;
}
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 1000 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 1000)
      {
         if(addrandomly == true)
         {
            count += 2;
            count += Math.random() * 1;
         }
         else
         {
            count++;
         }
         if(count > waittime)
         {
            count = 0;
            if(rot == undefined)
            {
               boolean = _root.currentCamX - (_X + _parent._x) >= 0;
            }
            else if(rot == 180)
            {
               boolean = _root.currentCamX - (_X + _parent._x) <= 0;
            }
            else if(rot == 90)
            {
               boolean = _root.currentCamY - (_Y + _parent._y) >= 0;
            }
            else
            {
               boolean = _root.currentCamY - (_Y + _parent._y) <= 0;
            }
            if(boolean)
            {
               LD = "right";
            }
            else
            {
               LD = "left";
            }
            if((!(onlyright == true && LD == "left") && !(onlyleft == true && LD == "right") || lockedside == true) && (total < limit || limit == "none"))
            {
               play();
               onEnterFrame = function()
               {
                  if(_root.PauseGame == false)
                  {
                     play();
                  }
                  else
                  {
                     stop();
                  }
               };
            }
         }
      }
   }
};
