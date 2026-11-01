rotation = 0;
rotspeed = 0;
rot2 = 0;
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char.air == true)
      {
         rot2 = - _root.radiansToAngle(Math.atan2(_root.Course.Char.yspeed,- _root.Course.Char.xspeed));
         rot2 += 10;
      }
      if(_root.Course.Char._xscale < 0)
      {
         rot2 = - _root.radiansToAngle(Math.atan2(_root.Course.Char.yspeed,_root.Course.Char.xspeed));
         rot2 -= 10;
      }
      if(!_root.Course.Platforms.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 5) * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true))
      {
         if(air == false)
         {
            air = true;
            if(groundrot < 0)
            {
               _root.Course.Char.yspeed -= Math.sin(groundrot) * _root.Course.Char.xspeed * 0.75;
            }
            _rotation = groundrot * (_root.Course.Char._xscale / 100);
            rotation = groundrot * (_root.Course.Char._xscale / 100);
         }
         if(rot > 0)
         {
            rot = rot2;
            if(rot < -90)
            {
               rotation -= 360;
            }
            rotspeed = (rot - rotation) / 10;
         }
         else
         {
            rot = rot2;
            if(rot > 90)
            {
               rotation += 360;
            }
            rotspeed = (rot - rotation) / 10;
         }
         if(rotation > 520)
         {
            rotation -= 360;
         }
         if(rotation < -520)
         {
            rotation += 360;
         }
         rotspeed = _root.groundFriction(rotspeed,0.1,1.15);
         rotspeed = Math.min(rotspeed,15);
         rotspeed = Math.max(rotspeed,-15);
         rotation += rotspeed;
         _rotation = rotation;
         air = true;
      }
      else
      {
         if(air == undefined || air == true)
         {
            rotation = 0;
            _rotation = 0;
            air = false;
         }
         rotation = _rotation * (_root.Course.Char._xscale / 100);
         distance = 5;
         frontpointX = _root.Course.Char._x - Math.cos(_root.angleToRadians(rotation)) * distance;
         frontpointY = _root.Course.Char._y - Math.sin(_root.angleToRadians(rotation)) * distance;
         coint = 0;
         while(!_root.Course.Platforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) && (_root.Course.Platforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,(frontpointY + 25) * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,(frontpointY + 15) * _root.coursescale / 100 + _root.Course._y,true)))
         {
            frontpointY += 0.5;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         coint = 0;
         while(_root.Course.Platforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(frontpointX * _root.coursescale / 100 + _root.Course._x,frontpointY * _root.coursescale / 100 + _root.Course._y,true))
         {
            frontpointY -= 0.5;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         backpointX = _root.Course.Char._x + Math.cos(_root.angleToRadians(rotation)) * distance;
         backpointY = _root.Course.Char._y - Math.sin(_root.angleToRadians(rotation)) * distance;
         coint = 0;
         while(!_root.Course.Platforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) && (_root.Course.Platforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,(backpointY + 25) * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,(backpointY + 15) * _root.coursescale / 100 + _root.Course._y,true)))
         {
            backpointY += 0.5;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         coint = 0;
         while(_root.Course.Platforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(backpointX * _root.coursescale / 100 + _root.Course._x,backpointY * _root.coursescale / 100 + _root.Course._y,true))
         {
            backpointY -= 0.5;
            count++;
            if(count > 50)
            {
               break;
            }
         }
         rotation = _root.radiansToAngle(Math.atan2(backpointY - frontpointY,backpointX - frontpointX));
         groundrot = rotation;
         _rotation = rotation * (_root.Course.Char._xscale / 100) * 0.9;
         speedslide = Math.sin(_root.angleToRadians(rotation));
         speedslide = Math.max(speedslide,-1);
         speedslide = Math.min(speedslide,1);
         if(_root.Course.Char.Ice == true)
         {
            _root.Course.Char.xspeed += speedslide * 0.75;
         }
         else if(_root.Course.Char.Puddle == true)
         {
            _root.Course.Char.xspeed += speedslide * 0.9;
         }
         else
         {
            _root.Course.Char.xspeed += speedslide;
         }
      }
      _root.DiveRot = _rotation;
      if(_root.KeyRIGHT() && _root.Controll == true)
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airwalkspeed) / (_root.airmaxWalkspeed / 3),0) / 8;
      }
      if(_root.KeyLEFT() && _root.Controll == true)
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airwalkspeed) / (_root.airmaxWalkspeed / 3),0) / 8;
      }
      if(_root.KeyTapRIGHT())
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airtapspeed) / (_root.airmaxtapWalkspeed / 3),0) / 5;
      }
      if(_root.KeyTapLEFT())
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airtapspeed) / (_root.airmaxtapWalkspeed / 3),0) / 5;
      }
      if(_root.KeyDOWN())
      {
      }
      if(_root.KeyUP() && _root.Course.Char.air == false && Math.abs(_root.Course.Char.xspeed) < 5 && (_root.Course.Char._xscale > 0 && _root.KeyRIGHT() || _root.Course.Char._xscale < 0 && _root.KeyLEFT()))
      {
         tellTarget(_root.Course.Char)
         {
            attack = true;
            attackFrame = "backflip";
            xspeed *= 0.75;
            if(_xscale < 0)
            {
               xspeed -= (xspeed + 30) / 5;
            }
            else
            {
               xspeed -= (xspeed - 30) / 5;
            }
            yspeed = Math.min(- _root.jumpspeed - 2,yspeed);
         }
      }
      if(_currentframe > 2 && _root.KeyUP() && _root.Course.Char.air == false && Math.abs(_root.Course.Char.xspeed) > 3 && Math.abs(rotation) < 25 && (_root.Course.Char._xscale > 0 && !_root.KeyRIGHT() || _root.Course.Char._xscale < 0 && !_root.KeyLEFT()) && !(Math.abs(rotation) > 15 && Ice == true))
      {
         tellTarget(_root.Course.Char)
         {
            attack = true;
            attackFrame = "diveflip";
            if(_xscale < 0)
            {
               xspeed -= (xspeed - 15) / 5;
            }
            else
            {
               xspeed -= (xspeed + 15) / 5;
            }
            yspeed = Math.min((- _root.jumpspeed) / 1.5,yspeed);
         }
      }
      if(_root.Course.Char.Ice == true && Math.abs(rotation) <= 3 || Math.abs(rotation) <= 15)
      {
         if(_root.KeyUP() && _root.Course.Char.air == false && Math.abs(_root.Course.Char.xspeed) < 3)
         {
            play();
         }
         if(_root.Course.Char.air == false && Math.abs(_root.Course.Char.xspeed) < 1 && !_root.KeyDOWN())
         {
            play();
         }
      }
      if(_root.Course.Char.Water == true && _root.Course.Char.air == true && _root.Metal !== true)
      {
         play();
      }
      count++;
      if(count == 3 && _root.Course.Char.air == true && Math.abs(_root.Course.Char.yspeed) + Math.abs(_root.Course.Char.xspeed) > 7 && _root.Course.Char.yspeed < 0 && _root.Course.Char.Water == false && _root.WingCap == true)
      {
         flying = true;
      }
      if(count > 20 && _root.Course.Char.air == true && flying == true && _root.WingCap == true)
      {
         _root.Course.Char.attackFrame = "Flying";
      }
   }
};
