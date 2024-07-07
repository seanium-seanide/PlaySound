#include "SoundPlayer.hpp"

int main(int argc, char **argv)
{
  SoundPlayer player;
  player.load("assets/StarWars3.wav", "star_wars");
  player.load("assets/beep.wav", "beep");

  // Think concurrency - need less wasteful to do this! :)
  // Maybe somehow with a mutex lock?
  // Problem: sound plays in thread that we have no access to!
  // (This is not urgent - this is not normally a problem - usually,
  // the sound should play until it finishes, or we stop it!)
  player.play("beep");
  while (player.isPlaying())
  {
  }

  player.play("star_wars");
  while (player.isPlaying())
  {
  }

  player.play("beep");
  while (player.isPlaying())
  {
  }

  return 0;
}
