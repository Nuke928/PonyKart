#include "pch.h"
#include "Levels/LevelChangedEventArgs.h"

using namespace Ponykart::Levels;

LevelChangedEventArgs::LevelChangedEventArgs(Level& NewLevel, Level& OldLevel, LevelChangeRequest& Request)
: newLevel(NewLevel), oldLevel(OldLevel), request(Request)
{

}
