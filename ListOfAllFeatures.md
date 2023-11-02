<h3>List of all features MarineRunner has, this is described in brief without details, each of these things has many more smaller things in it that make it all work nicely. <br/>
Some of the features may be unclear. I recommend watching the latest demo version for better context. </h3>

<h2> Player </h2>
- Physics-based movement  <br/>
- Crouch <br/>
- Pull up on the edge <br/>
- Slide <br/>
- Sliding down the ramp (speed increases while sliding down the ramp) <br/>
- Wallrun <br/>
- Dash <br/>
- Slow Motion <br/>
- Hook (force is applied to the player in the direction of the hook) <br/>
- Can use the First Aid Kit <br/>
- Quick Inventory for weapons (1 - first weapon, 2 - second weapon etc) <br/>
- Take Item (resources, weapons etc) <br/>
- Drop Item  <br/>
- Save game (player location, play time, date, inventory etc) <br/>
- Dynamic music depending on events with fade out (menu, combat, exploration music) <br/>
- Pause Menu <br/>
- Death widget when player died (player can load last save or exit the game) <br/>
- Messages that player can pick up and read  <br/>
<h4> - HUD </h4> 
- Shows current health on progress bar <br/>
- Shows currently holding weapon with ammunition information <br/>
- When you hover over an item, information (name, description, icon) about that item will appear <br/>
- (First Aid Kit, Dash, Slow Motion) information (when to use, how much left, what button to press) <br/>
- Hit, Dash, Slow Motion animation <br/>

<h2> Pause Menu <a href="https://youtu.be/TWsT171ZXYA?t=9"> Showcase </a></h2> 
- Pause Menu with working resume, load game, save game, settings and quit buttons<br/>
<h4> - Load Game </h4>
- List View with all saves  <br/>
- list view entry shows the name of the save, screenshot taken while saving the game, save date, total game time saved <br/>
- The message "Confirm Loading" will appear. <br/>
<h4> - Save Game </h4>
- Saves game when can (cant save while in Air or in Combat) <br/>
- The notification "Saved Game" appear after saving the game <br/>
<h4> - Settings </h4>
- (Game, Audio, Video, Bindings) Settings, Accept, Back to Defaults <br/>
- Every settings (game, audio etc) is in List View and one entry == one setting <br/>
- Any Entry value can be saved to config and is loaded When the game is first launched <br/>
- Entry can be set to: 
<ul>
        <li>Category (centered text with diffrent color) </li>
	<li>Quality (like Low, Med, High, Ultra - 0,1,2,3 value) </li>
	<li>Checkbox (true, false value) and checkbox can be connected to other settings </li>
	<li>Slider value (slider range or number of decimal digits that can be set) </li>
	<li>Key mapping (changed by Key mapping name) </li>
	<li>Set resolution of screen (gets list of possible resolutions)</li>
	<li>Set Window Type (FullScreen, Windowed etc) </li>
</ul>
- Entry value can be applied by:
<ul>
	<li>executing command in console <br/></li>
	<li>sounds (value is saved as volume in given sound class and mix sound class) <br/></li>
	<li>none <br/></li>
</ul>
- Entry can be disabled and grayed out <br/>
<h4> - Quit Game </h4> 
- The message "Confirm quiting" will appear <br/>
<h2> Weapons </h2>
	- The player can fire the weapon in hipfire or ADS mode. 
	- Shooting mechanic:
	<ul>
		<li> When the player shoots, the bullet spawns </li>
		<li> The bullet moves based on physics or custom (you can set how far it can move, at what speed, etc.). </li>
		<li> Camera, Fov and Gun (curves) recoil will play in timeline. Based on what is set </li>
	</ul>
	- ADS is more precise then Hipfire
	- When shooting camera can go up (its go as far as it was set and then returns to the starting posiiton)
	- Bullet Casing ejection after the shot
	- Scope with zoom (when in ADS player can zoom in or out)
	- Reload (reload the entire magazine or one bullet)
	- Ammunition based on pickable item
	- Can spawn more bullets at once (like shotgun)
	- The weapon can be set to automatic (by holding down the button, the weapon will fire continuously).
	- When a bullet spawns then a random value from the range is selected for the bullet rotation
	- The gun can sway while moving the camera or in motion  (diffrent sway for other case)
	- Animation: Shoot, Reload and empty magazine
<h2> Albertos (crafting item robot)</h2>
<h2> Enemies </h2>
<h2> Objects </h2>
