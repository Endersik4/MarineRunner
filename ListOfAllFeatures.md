<h2>Here is a brief overview of MarineRunner's features. <br/>
Keep in mind that each of these includes various details and functionalities, contributing to a seamless gaming experience. <br/>
Some of the features may be unclear. I recommend watching the latest demo version for better context.  <br/>
The game includes over 150 <i>animations (+ many widget animations)</i>, 360 <i>3D models</i>, 160 <i>sounds and </i>8<i> pieces of music</i>. <br/>
</h2>

<hr>
<h2> PLAYER </h2>
<h3> - Physics-based movement </h3>
<h3> - Arms as skeletal mesh  </h3>
<h3> - HUD: </h3>
	<ul>
		<li> <strong>Health Display:</strong> Presents the player's current health through a visually informative progress bar.</li>
		<li> <strong>Weapon and Ammunition Information:</strong> Indicates the currently equipped weapon along with ammunition details. </li>
		<li> <strong>Item Information Tooltip:</strong> Upon hovering over an item, displays relevant details such as name, description, and icon. </li>
		<li> <strong>(First Aid Kit, Dash, Slow Motion) information:</strong> when to use, how much left, what button to press </li>
		<li> <strong>Animation:</strong> Enhances user experience through visually appealing animations for events like hits, dashes, changes in health, and activation of slow motion. </li>
	</ul>
<h3> - Take Damage: </h3>
	<ul>
	    <li><strong>Handles the application of damage to the player character and manages related functionalities.</strong></li>
	    <li><strong>Damage Application:</strong> Utilizes the TakeDamageInterface to apply damage to the player character.</li>
	    <li><strong>Health Check:</strong> Monitors the player's health, and if it drops to or below 0, triggers the appearance of the YouDied Widget.</li>	
     </ul>
     
<h3> - Can use The First Ait kit </h3>

<h3> - Jump Component: </h3>
	<ul>
	    <li><strong>Jump Capability:</strong> Enables the player to execute jumps.</li>
	    <li><strong>Coyote Time Mechanic:</strong> Implements a responsive jumping experience with a coyote time mechanism.</li>
	    <li><strong>Landing Effect:</strong> Triggers a visual effect upon the player's initial landing on the ground.</li>
	    <li><strong>Mid-Air Detection:</strong> Checks whether the player is currently airborne.</li>
	    <li><strong>Ramp Interaction:</strong> Detects if the player is on a ramp, initiating a sliding action.</li>
	</ul>
<h3> - Arms sway component: </h3>
	<ul>
	    <li><strong>Responsive Arm Movement:</strong> Arms sway in response to mouse movements and return to their initial position.</li>
	    <li><strong>Dynamic Arm Animation:</strong> Arms follow the player's movement, adjusting accordingly, and smoothly return to their original position.</li>
	    <li><strong>Walking Animation:</strong> When the player moves, the arms simulate a walking motion using the Lemniscate of Bernoulli curve.</li>
	</ul>
<h3> - Message Handler component: </h3>
	<ul>
		<li><strong>Responsible for managing various in-game messages and interactions with widgets</strong> </li>
		<li><strong>Widget Message Handling:</strong> Displays messages related to in-game events, such as notifications for unlocking new recipes or warnings about insufficient weapon slots.</li>
   		<li><strong>Message Closure:</strong> Allows for the closure of the current message (e.g., a note) through a designated key binding, ensuring seamless interaction, except when in the pause menu.</li>
	</ul>
<h3> - Pause Game component: </h3>
	<ul>
	    <li><strong>Manages the game's pause functionality.</strong></li>
	    <li><strong>Check for Pause:</strong> Verifies if the player is allowed to pause the game.</li>
	    <li><strong>Control HUD Widget:</strong> Pauses the functions of the HUD Widget, ensuring proper interactions when activated.</li>
	    <li><strong>Activate PauseMenu Widget:</strong> When the player initiates a pause, the PauseMenu Widget is displayed, providing additional options and information (see below).</li>
	</ul>
	
<h3> - Save/Load/Restart Game component: </h3>
	<p> Save: </p>
	<ul>
	    <li>Checks if the player can save the game, considering factors like being in the air, in the ads, or in an elevator.</li>
	    <li>Determines save name (manual or autosave) based on whether the player saves in the Pause Menu or if a checkpoint triggers the save.</li>
	    <li>Saves essential information to the game instance, including the save name.</li>
	    <li>Saves player data, like Inventory, Recipes, weapon inventory, Location, Rotation, and Health.</li>
	    <li>Updates objects with the Save Interface, e.g saving their location if moved.</li>
	    <li>Saves CustomSavedData, an object storing specific data about other objects, e.g., unlocking a door with a pin.</li>
	    <li>Generates a JSON file containing save information (Save Name, Save date, thumbnail path, playtime, current level name, and more).</li>
	    <li>Saves the currently playing exploration music.</li>
	    <li>Spawns a widget notifying about the saved game.</li>
	</ul>
	<p> Load: </p>
	<ul>
	    <li>Loads the game by opening the current level.</li>
	    <li>Resets detected enemies, transitioning music from combat to exploration, in game instance.</li>
	    <li>Loads player data, including inventory, recipes, weapon inventory, and health.</li>
	    <li>Loading the music if different from the saved one.</li>
	    <li>Loads data for other objects, handling cases like item pickup or spawn.</li>
	    <li>Loads mouse sensitivity and Field Of View.</li>
	</ul>
	<p> Restart: </p>
	<ul>
	    <li>Restarts the game by resetting values for all temporary objects and the player.</li>
	    <li>Destroys all enemies and guns on the map.</li>
	    <li>Resets object data in TempCustomSavedData within SavedDataobject using SaveCustomDataInterface with RestartData function.</li>
	    <li>Restarts slow motion if activated when the player died.</li>
	    <li>Resets detected enemies.</li>
	    <li>Spawns a new player.</li>
	</ul>
	
<h3>  - Swing Component </h3>
	<ul>
	    <li><strong>Hook Detection:</strong> Checks if the raycast hits the Hook (actor) in the game world.</li>
	    <li><strong>Hook Visualization:</strong> Displays a flipbook animation when the Hook is found, enhancing visibility for the player.</li>
	    <li><strong>Hook Interaction:</strong> Allows the player to grab the hook when close, triggering another flipbook animation on the hook itself.</li>
	    <li><strong>Previous Hook Management:</strong> Disables the previous hook if the player moves away or stops looking at it.</li>
	    <li><strong>Swing Initiation:</strong> If all conditions are met and the player presses the Swing Button, spawns a Niagara particle (SwingLineActor), waits briefly, and swings using VInterp to HookLocation.</li>
	    <li><strong>Swing Interruption:</strong> If the player is close to the hook and has started swinging, stop the swing.</li>
	</ul>
	
<h3> - Weapon Handler Component: </h3>
<p>Manages various weapon interactions and functionalities for a dynamic and responsive gameplay experience.</p>
<ul>
    <li><strong>Shoot:</strong> Controls the firing mechanism of equipped weapons.</li>
    <li><strong>Reload:</strong> Manages the reloading process for the player's currently equipped weapon.</li>
    <li><strong>ADS In:</strong> Handles the aiming down sights (ADS) animation and behavior.</li>
    <li><strong>ADS Out:</strong> Manages the transition out of aiming down sights.</li>
    <li><strong>Zoom:</strong> enables sniper scope zoom-in functionality; Adjusts mouse sensitivity dynamically based on the zoom level.</li>
    <li><strong>Equips Weapon:</strong> Allows the player to draw a weapon from storage using a specified key (e.g., pressing '1' equips the weapon from slot 1).</li>
    <li><strong>Drops Gun:</strong> Facilitates the action of dropping the currently held weapon.</li>
    <li><strong>Loads Mouse Sensitivities:</strong> Retrieves mouse sensitivity settings from the configuration save, including values for ADS, 2x, 4x, and 8x zoom.</li>
</ul>
	
<h3> - Crouch and Slide Component </h3>
	<ul>
	    <li><strong>Crouch Check:</strong> Verifies if the player can enter a crouched state.</li>
	    <li><strong>Crouch Animation:</strong> If crouching is possible, smoothly lerps from the initial player scale (Actor 3D Scale) to the crouch scale.</li>
	    <li><strong>Crouch State Effects:</strong> While in the crouched state, adjusts gameplay elements such as footstep sounds and movement speed.</li>
	    <li><strong>Stand Check:</strong> If the player intends to stop crouching, checks if standing is achievable (e.g., no obstruction overhead) and lerps back to the initial actor scale.</li>
	    <li><strong>Slide Activation:</strong> Activates sliding, triggered by a different button than crouch.</li>
	    <li><strong>Slide Conditions:</strong> Checks various conditions for sliding, e.g., cant slide backward, cant slide during jumping, etc.</li>
	    <li><strong>Slide Mechanics:</strong> Initiates sliding by setting the CurrentMovementForce to the Initial Sliding Velocity and gradually subtracting SlideSpeed over time. If CurrentMovementForce drops below Crouch Speed, the player begins crouching.</li>
	    <li><strong>Ramp Slide:</strong> If the player starts sliding on a ramp, initiates a camera shake and adjusts movement forces. Instead of subtracting SlideSpeed, a ramp force is added, causing the player to accelerate until CurrentMovementSpeed exceeds MaxRampSpeed.</li>
	</ul>

<h3> - Dash Component </h3>
	<ul>
	    <li><strong>Dash Availability:</strong> Verifies if the player can perform a dash.</li>
	    <li><strong>Dash Location Calculation:</strong> Determines the dash destination by connecting two raycasts, one from the head location and the other from the ground location and selects the closer one.</li>
	    <li><strong>Dash Execution:</strong> Lerps from the initial player location to the calculated dash location, initiating dash effects in the process.</li>
	    <li><strong>Dash Widget Spawn:</strong> Spawns a Dash Widget to provide visual feedback to the player.</li>
	</ul>
	
<h3> - Pull Up Component</h3>
	<ul>
	    <li><strong>Edge Detection:</strong> Identifies the edge of the object that the player can pull up on.</li>
	    <li><strong>End Location Calculation:</strong> Calculates the end location over the detected edge, ensuring a proper pull-up position.</li>
	    <li><strong>Pull Up Execution:</strong> Lerps from the initial player location to the pull-up location and applies an impulse at the end for a smooth transition and realistic motion.</li>
	</ul>
	
<h3> - Slow Motion Component</h3>
	<ul>
	    <li><strong>Slow Motion Activation Check:</strong> Verifies if the slow motion effect can be activated.</li>
	    <li><strong>Slow Motion Activation:</strong> Sets the Global Time Dilation to a specified SlowMotionValue and initiates visual effects associated with slow motion (e.g., chromatic aberration, color grain in Post Process).</li>
	    <li><strong>Slow Motion Deactivation:</strong> Disables the slow motion effect after a defined SlowMotionTime duration.</li>
	    <li><strong>User-Initiated Deactivation:</strong> Allows the player to manually disable the slow-motion effect before the set duration elapses.</li>
	</ul>
	
<h3> - Take Component</h3>
	<ul>
	    <li><strong>Raycast for Items:</strong> Does raycasting to detect objects with ITakeInterface from the player's camera view.</li>
	    <li><strong>Item Hover Interaction:</strong> If the raycast hits an item with the ITakeInterface, highlights the item and displays relevant information about it.</li>
	    <li><strong>Item Pickup:</strong> If the player presses the take button, triggers the TakeItem function from the ITakeInterface and pick up the item.</li>
	</ul>

<h3> - Wallrun Component</h3>
	<ul>
	    <li><strong>Wallrun Check:</strong> Verifies if the player can perform a wallrun, considering conditions such as being in pull-up mode, crouching, or having a velocity below a certain threshold.</li>
	    <li><strong>Object Detection:</strong> Checks if the player is next to a wall or another suitable object for wallrunning.</li>
	    <li><strong>Sticking to Obstacle:</strong> Attaches the player to the obstacle by applying a significant impulse towards the object based on the hit normal.</li>
	    <li><strong>Wallrun Variable Adjustment:</strong> Sets player variables specific to wallrun, such as increased movement speed.</li>
	    <li><strong>Camera Roll Animation:</strong> Rotates the camera roll smoothly using Timelines to enhance the visual effect of wallrunning.</li>
	    <li><strong>Camera Yaw Rotation:</strong> Adjusts the camera yaw to align with the direction of the wallrun.</li>
	    <li><strong>Mouse Movement Interaction:</strong> Pauses the rotation of the camera yaw if the player moves the mouse during a wallrun.</li>
	    <li><strong>Automatic Wallrun Termination:</strong> If the obstacle is no longer detected, automatically pushes the player away from the obstacle.</li>
	    <li><strong>Coyote Time Implementation:</strong> Allows the player to jump even if the obstacle is no longer detected (slight delay).</li>
	</ul>

<h3> - Weapon Inventory Component</h3>
	<ul>
	    <li><strong>Weapon Storage:</strong> Manages the storage of all weapons within the game.</li>
	    <li><strong>Weapons Data Preservation:</strong> Stores weapons using pointers to spawned guns when items are taken. During game saves, transfers guns as pointers to an array with the class of the guns, enabling their respawn after loading the game.</li>
	    <li><strong>Weapon Addition:</strong> Adds a weapon to the inventory when an item with the "Weapon" boolean is taken, ensuring the total number of weapons does not exceed the specified <em>MaxAmountOfItems</em>.</li>
	    <li><strong>Weapon Removal:</strong> Removes a weapon from the inventory when the player drops the gun.</li>
	    <li><strong>Weapon Switching:</strong> When a new gun is added, puts away any currently equipped gun through an animation, then draws the new gun.</li>
	    <li><strong>Slot-Specific Weapon Handling:</strong> Manages taking weapons from a specified slot within the inventory.</li>
	</ul>

- Footsteps sounds



<h3> Pause Menu <a href="https://youtu.be/TWsT171ZXYA?t=9"> Showcase </a></h3> 
- Pause Menu with working resume, load game, save game, settings and quit buttons<br/>
<h3> - Load Game </h3>
- List View with all saves  <br/>
- list view entry shows the name of the save, screenshot taken while saving the game, save date, total game time saved <br/>
- The message "Confirm Loading" will appear. <br/>
<h3> - Save Game </h3>
- Saves game when can (cant save while in Air or in Combat) <br/>
- The notification "Saved Game" appear after saving the game <br/>
<h3> - Settings </h3>
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
<h3> - Quit Game </h3> 
- The message "Confirm quiting" will appear <br/>

<h3> Main Menu </h3>
- Almost the same as the pause menu, but without saving the game and with a new game/continue button

<h3> Weapons </h3>
	- The player can fire the weapon in hipfire or ADS mode. <br/>
	- Shooting mechanic:<br/>
	<ul>
		<li> When the player shoots, the bullet spawns </li>
		<li> The bullet moves based on physics or custom move (you can set how far it can move, at what speed, etc.). </li>
		<li> Camera, Fov and Gun (curves) recoil will play in timeline. Based on what is set </li>
	</ul>
	- ADS is more precise then Hipfire<br/>
	- When shooting camera can go up (its go as far as it was set and then returns to the starting posiiton)<br/>
	- Bullet Casing ejection after the shot<br/>
	- Scope with zoom (when in ADS player can zoom in or out)<br/>
	- Reload (reload the entire magazine or one bullet)<br/>
	- Ammunition based on pickable item<br/>
	- Can spawn more bullets at once (like shotgun)<br/>
	- The weapon can be set to automatic (by holding down the button, the weapon will fire continuously).<br/>
	- When a bullet spawns then a random value from the range is selected for the bullet rotation<br/>
	- The gun can sway while moving the camera or in motion  (diffrent sway for other case)<br/>
	- Animation: Shoot, Reload and empty magazine<br/>
	
<h3> <a href="https://www.youtube.com/embed/dtFB4vfd2Eg"> Albertos </a> (crafting item robot)</h3>
	- It Takes player's location and then selects a random location within a specified radius around the player's location and goes there <br/>
	- When player is nearby, Albertos looks toward the player and the player can interact with it:<br/>
	<ul>
		<li> The Craft Widget is visible and the player can interact with it</li>
		<li> On The left Side, the Crafting widget shows all the items the player has (weapons, ammunition, first aid kits...) </li>
		<li> In The upper Right corner there is TileView with resources that the player currently has (wood, metal, steel...)</li>
		<li> In the middle on the right are information about the item that is currently selected to craft - Item name, description, craft time, amount</li>
		<li> In the lower right corner are the resource requirements the player must have to create the item</li>
		<li> In The Center is a image with an item icon that is currently selected, under which the player can choose the multiplier of the item amount (2x, 4x...), 
		then there is a craft button</li>
		<li> Item can be changed by clicking on arrows (left or right side)</li>
	</ul>
	- When a player clicks the craft button:
		<ul>
			<li> if the front door of Albertos is not open, open it</li>
			<li> Disable all Buttons and start progress bar and timer</li>
			<li> Spawn crafted item in craft item location socket with dissolve material on it and start dissolve from the beginning</li>
			<li> Play Crafting Animation (turn on lasers, play albertos source code on monitor)</li>
			<li> Play loop crafting sound with start and end</li>
			<li> if the item is too large then lerp from mini scale to normal scale after crafting is completed</li>
			<li> Move Item to the "Final Item Location" socket when item was crafted and set that the player can pick up the item</li>	
		</ul>
	- When the item is created and the player leaves, close the door and widget and start wandering <br/>
	- Can play random sounds while moving <br/>
	- Diffrent "footsteps" sounds depending on speed of robot <br/>
	- Everything is animated and with sound <br/>
<h3> Enemies </h3>
	- Enemy can move to random location and after some amount of movements, enemy goes back to its original location <br/>
	- When enemy sees the player than starts shooting  <br/>
<h3> Saving/Loading Game </h3>
	- The game can save: 
		<ul>
			<li> Player Location and Rotation</li>
			<li> Player's Health </li>
			<li> Player's inventory and recipes for items</li>
			<li> Quick select inventory (weapon) </li>
			<li> Screenshot took in moment when player saved the game</li>
			<li> Information that are needed for load game description (SaveName, SaveNumber, Path To Thumbnail, SaveDate, Level, TotalPlayTime). Its saved in json file </li>
			<li> State of chest (was it open, was pin entered)</li>
			<li> State of Door (was pin entered)</li>
			<li> Custom Game Settings that are not saved by engine (e.g audio volume, fov, mouse sensitivity)</li>
		</ul>
	- saving chests or door is possible thanks to custom save interface 
<h3> Objects </h3>
<h3> Item: </h3>
	- The Item can be picked up or crafted <br/>
	- In the properties of an item, you can set its name, amount, description, icon, is craftable, resource requirements (and other small things). <br/>
	- When the player hovers the mouse over an item, an outline will appear and the item message widget will appear.<br/>
	- The item message widget contains the item icon, name and description. It plays a startup animation and disappears after some time. <br/>
	- If an object requires an item to work with it, then in that object you can set which item is required (for example, a weapon requires an ammunition-item). <br/>
<h3> Explosion Barrels </h3>
	- When the bullet hits the explosion barrel, spawn a geometry collection and add the impulse from the bullet to the geometry collection. <br/>
	- Spawn Explosion Decal, Sound and Particle Effect <br/>
	- When something is near it checks if nothing is blocking the way from explosion to the object that was hit and if not then apply damage according to distance and all the effects <br/>
<h3> Object Rotates when the bullet hits the button: </h3>
	- When the player shoots the button then play rotator timeline and apply new rotation  <br/>
	- Object Shows the current time remaining until the object returns to its original rotation.  (Text Render Comp)  <br/>
	- Play rotate sound <br/>
<h3> Message: </h3>
	- Works like item but instead of taking message to inventory, the massage widget is spawning with animation. <br/>
	- Message was made using Rich Text Block, its possible to customize the font (for example highlight the important thing) <br/>
	- Player can quit the message by clicking the quit message button (default - E) <br/>
<h3> Elevator: </h3>
	- Player can call elevator from outside and while elevator goes to the player, "Wait For Elevator" animation will play on widget <br/>
	- When Player is in the elevator, he can choose floor to go to and after elevator starting animation it wll go to the floor that player chose <br/>
	- Select Floor on widget is entry in Tile View and can be many of these, Select Floor entry can be also set up as blocked then player cant go to this floor <br/>
	- Eleveator can require pin code to enter <br/>
	- When player selects floor that he currently is then it will open elevator doors <br/>
	- Everything is animated in Widget Blueprint <br/>
<h3> Doors </h3>
	- Can be open from two sides by clicking on panel (3D Widget)  <br/>
	- Door can require pin code to open <br/>
	- After opening the door, close door timer will play <br/>


