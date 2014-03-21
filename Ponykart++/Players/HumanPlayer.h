#ifndef HUMANPLAYER_H_INCLUDED
#define HUMANPLAYER_H_INCLUDED
#include "Players/Player.h"
#include <boost/signals2.hpp>
#include "Input/KeyBindingManager.h"

// comment this out if you want drifting to be disabled for human-controlled karts.
// useful if you want to send a version out to users while the drifting is broken
/// TODO: Do something about the comment above. Might want to remove the define and #ifdef'd out parts.
#define DRIFTING_ENABLED

namespace Ponykart
{
namespace Input { class KeyBindingManager; }
namespace Levels{ class LevelChangedEventArgs; }
namespace Players
{
	class HumanPlayer : public Player
	{
	public:
		HumanPlayer(Levels::LevelChangedEventArgs* eventArgs, int id);
	protected:
		virtual void onSteeringChanged (float value) override;
		virtual void onAccelerateChanged (float value) override;
		virtual void onBrakeChanged (float value) override;
		virtual void onStartDrift () override;
		virtual void onStopDrift () override;
		virtual void useItem () override; // Uses an item
	public:
		Input::KeyBindingManager* bindings;

	private:
		void pressHandler (int playerID, Ponykart::Input::GameInputID inputID);
		void releaseHandler (int playerID, Ponykart::Input::GameInputID inputID);
		void axisMoveHandler (int playerID, Ponykart::Input::GameInputID inputID, float value);

		boost::signals2::scoped_connection pressConnection, releaseConnection, axisMoveConnection;
	};
}
}
#endif // HUMANPLAYER_H_INCLUDED
