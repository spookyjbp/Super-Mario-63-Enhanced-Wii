stop();
count = 0;
onEnterFrame = function()
{
   if(count > 3 && _root.Course.Char.air == false)
   {
      if(this.box.hitTest(_root.Course.Char.charClip.body) || this.box.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true))
      {
         if(_root[key] !== true)
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.attackFrame = "Item";
            _root.heldItemObtained = key;
            if(key == "lvl4keyA")
            {
               _root.heldItemMessage = "You got Boo\'s Key to the Staircase";
            }
            if(key == "lvl4keyB")
            {
               _root.heldItemMessage = "You got Boo\'s Key to the West Tower (a.k.a. The Cable Room).";
            }
         }
         _root[key] = true;
         gotoAndStop(4);
      }
   }
   else
   {
      count++;
   }
};
