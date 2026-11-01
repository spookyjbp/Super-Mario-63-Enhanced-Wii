if(_root.MuteBGMusic !== true)
{
   _root.PlayBGSong(_root.SongRepeat);
}
onEnterFrame = function()
{
   _root.playpause = true;
   _root.PauseGame = false;
};
