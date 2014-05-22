#ifndef MAINMENUSINGLEPLAYERHANDLER_H_INCLUDED
#define MAINMENUSINGLEPLAYERHANDLER_H_INCLUDED

#include <string>
#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace Handlers
{
	/// This handler responds to level and character selection events from the main menu, holds on to them, then loads the appropriate level with the right character.
	/// 
	/// At the moment it's only for single player and it's pretty rough. It'll probably need to be rewritten a bunch when we come to adding multiplayer.
	struct MainMenuSinglePlayerHandler : public LKernel::LKernelObject
	{
	public:
		MainMenuSinglePlayerHandler();

		/// Since character selection at the moment is the final stage in the menus, this loads the new level based on the previous
		/// level selection and current character selection
        void onCharacterSelect(const std::string& characterSelection);
		/// Saves the chosen level for later
        void onLevelSelect(const std::string& levelSelection);

	public:
		std::string levelSelection;
		std::string characterSelection;
	};
}
}

#endif // MAINMENUSINGLEPLAYERHANDLER_H_INCLUDED
