_root.CharRotationCode = function()
{
   tellTarget(_root.Course.Char)
   {
      rotation = _rotation;
      distance = 5;
      frontpointX = _root.Course.Char._x - Math.cos(_root.angleToRadians(rotation)) * distance;
      frontpointY = _root.Course.Char._y - Math.sin(_root.angleToRadians(rotation)) * distance + 15;
      hitplatform = false;
      i = 0;
      while(i < 9)
      {
         if(hitplatform == false && i <= 8)
         {
            frontpointY -= 5;
         }
         coint = 0;
         while(!(_root.Course.Platforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true)) && !_root.Course.BPlatforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) && (_root.Course.Platforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,(frontpointY + 7) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,(frontpointY + 7) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) || _root.Course.BPlatforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,(frontpointY + 7) * _root.coursescale / 100 + _root
         .Course._y,true)))
         {
            frontpointY++;
            hitplatform = true;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         coint = 0;
         while(_root.Course.Platforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) || _root.Course.BPlatforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true))
         {
            frontpointY--;
            hitplatform = true;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         if(hitplatform == true)
         {
            break;
         }
         if(i >= 8)
         {
            frontpointX = _root.Course.Char._x - Math.cos(_root.angleToRadians(rotation)) * distance;
            frontpointY = _root.Course.Char._y - Math.sin(_root.angleToRadians(rotation)) * distance;
         }
         i++;
      }
      backpointX = _root.Course.Char._x + Math.cos(_root.angleToRadians(rotation)) * distance;
      backpointY = _root.Course.Char._y - Math.sin(_root.angleToRadians(rotation)) * distance + 15;
      hitplatform = false;
      i = 0;
      while(i < 9)
      {
         if(hitplatform == false && i <= 8)
         {
            backpointY -= 5;
         }
         coint = 0;
         while(!(_root.Course.Platforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true)) && !_root.Course.BPlatforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) && (_root.Course.Platforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,(backpointY + 7) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,(backpointY + 7) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) || _root.Course.BPlatforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,(backpointY + 7) * _root.coursescale / 100 + _root.Course
         ._y,true)))
         {
            backpointY++;
            hitplatform = true;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         coint = 0;
         while(_root.Course.Platforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) || _root.Course.BPlatforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true))
         {
            backpointY--;
            hitplatform = true;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         if(hitplatform == true)
         {
            break;
         }
         if(i >= 8)
         {
            backpointX = _root.Course.Char._x + Math.cos(_root.angleToRadians(rotation)) * distance;
            backpointY = _root.Course.Char._y - Math.sin(_root.angleToRadians(rotation)) * distance;
         }
         i++;
      }
      rotation = _root.radiansToAngle(Math.atan2(backpointY - frontpointY,backpointX - frontpointX));
      groundrot = rotation;
      speedslide = Math.sin(_root.angleToRadians(rotation));
      speedslide = Math.max(speedslide,-0.5);
      speedslide = Math.min(speedslide,0.5);
      if(Ice == true)
      {
         xspeed += speedslide * 0.3;
      }
      else if(Puddle == true)
      {
         xspeed += speedslide * 0.1;
      }
      _rotation = _rotation + (rotation / 2 - _rotation) / 2;
      if(Math.abs(rotation) > 25 && Ice == true)
      {
         if(icedive == true)
         {
            attack = true;
            attackFrame = "Dive";
         }
         icedive = true;
      }
      else
      {
         icedive = false;
      }
   }
};
