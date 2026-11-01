stop();
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("OrangeBlockPL","OrangeBlockPL" + depth,depth,{_x:_X,_y:_Y});
_root.Course.Platforms["OrangeBlockPL" + depth]._x = this._x;
_root.Course.Platforms["OrangeBlockPL" + depth]._y = this._y;
if(_root["coindata" + _root.playingcourse][coinnum] == 1 && _root["coindata" + _root.playingcourse][coinnum] !== undefined)
{
   stuff.gotoAndStop("none");
}
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
      {
         if(this.box3.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.yspeed > 3 && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Ground" && (_root.Course.Char.Water == false || _root.Metal == true))
         {
            _root.Course.Char.yspeed *= 0.75;
            _root.Course.Char.yspeed -= 3;
            play();
            _root.Course.Platforms["OrangeBlockPL" + depth].removeMovieClip(_root.Course.Platforms["OrangeBlockPL" + depth]);
         }
         if(this.box2.hitTest(_root.Course.Char.charClip.body) && Math.abs(_root.Course.Char.xspeed) < 2 && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "hurt2")
         {
            play();
            _root.Course.Platforms["OrangeBlockPL" + depth].removeMovieClip(_root.Course.Platforms["OrangeBlockPL" + depth]);
         }
         if(this.box4.hitTest(_root.Course.Char.charClip.body) && (Math.abs(_root.Course.Char.xspeed) > 9 || Math.abs(_root.Course.Char.xspeed) > 9 && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Dive"))
         {
            _root.HitWall();
            if(_root.Course.Char._x < _X + _parent._x)
            {
               _root.Course.Char._xscale = -100;
            }
            else
            {
               _root.Course.Char._xscale = 100;
            }
            play();
            _root.Course.Platforms["OrangeBlockPL" + depth].removeMovieClip(_root.Course.Platforms["OrangeBlockPL" + depth]);
         }
         if(this.box.hitTest(_root.Course.Char.charClip.frame.water) && _root.Course.Char.attackFrame == "Spin" && _root.Course.Char.attack == true || _root.Course.Enemyhurt.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest((_X + 10) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest((_X - 10) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 30) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root
         .Course._y,true))
         {
            play();
            _root.Course.Platforms["OrangeBlockPL" + depth].removeMovieClip(_root.Course.Platforms["OrangeBlockPL" + depth]);
         }
      }
   }
};
