List of all features MarineRunner has, this is described in brief without details, each of these things has many more smaller things in it that make it all work nicely.
Some of the features may be unclear. I recommend watching the latest demo version for better context.

<h2> Player </h2>
- Movement based on physics
- Crouch
- Pull up on the edge
- Slide
- Slide on ramp (velocity increases when sliding on ramp)
- Wallrun
- Dash
- Slow Motion
- Hook (force is applied to the player in the direction of the hook)
- Can use First Aid Kit
- Quick Inventory for weapons (1 - first weapon, 2 - second weapon etc)
- Take Item (resource, weapon etc)
- Drop Item 
- Save game (player location, play time, date, inventory etc)
- Dynamic Music according to events with fade (menu, combat, exploration music)
- Pause Menu
- Death Widget when player died (player can load last save or quit the game)
- Messages that player can pick and read 
<h4> - HUD </h4>
- Shows current health
- Shows current holding weapon with information about ammo
- When hover on item, information(name, desc, icon) about this item will appear
- First Aid Kit, Dash, Slow Motion information (when can use, how many left, what button to press)
- Hit, Dash, Slow Motion animation
<h2> Pause Menu </h2>
- Pause Menu with working resume, load game, save game, settings and quit
<h4> - Load Game </h4>
- List View with saves 
- entry on list view shows save name, screenshot took when game was saved, saved date, saved total play time
- Confirm Loading message will pop up
<h4> - Save Game </h4>
- Saves game when can (cant save while in Air or in Combat)
- Notification pop up when the game was saved
<h4> - Settings </h4>
- (Game, Audio, Video, Bindings) Settings, Accept, Back to Defaults
- Every settings (game, audio etc) is in List View and one entry == one setting
- Any Entry value can be saved to config and is loaded When the game is first launched
- Entry can be set to:
	- Category (centered text with diffrent color)
	- Quality (like Low, Med, High, Ultra - 0,1,2,3 value)
	- Checkbox (true, false value) and checkbox can be connected to other settings
	- Slider Value (range for slider or how many decimal numbers can be set)
	- Key mapping (found by Key mapping name)
	- Set resolution of screen (list of possible resolutions)
	- Set Window Type (FullScreen, Windowed etc)
- Entry value can be applied by:
	- executing command in console
	- sounds (value is saved as volume in given sound class and mix sound class)
	- none
- Entry can be disabled and its grayed out
<h4> - Quit Game </h4>
- Confirm quiting from gave message will pop up
<h2> Weapons </h2>
	
<h2> Albertos (crafting item robot)</h2>
<h2> Enemies </h2>
<h2> Objects </h2>
