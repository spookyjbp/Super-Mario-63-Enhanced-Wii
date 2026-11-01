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
            if(keycount == 0)
            {
               _root.heldItemMessage = "You have 1 of Bowser\'s Keys!  There should be 3 more inside the Main Hall!";
            }
            if(keycount == 1)
            {
               _root.heldItemMessage = "You have 2 Bowser Keys!  The Cannon is now unlocked!  Use it to reach the upper story!";
            }
            if(keycount == 2)
            {
               _root.heldItemMessage = "You have 3 Bowser Keys!  One more!";
            }
            if(keycount == 3)
            {
               _root.heldItemMessage = "You have all 4 Bowser Keys!";
            }
         }
         _root[key] = true;
         gotoAndStop(6);
      }
   }
   else
   {
      count++;
   }
};
gotoAndStop(key);
