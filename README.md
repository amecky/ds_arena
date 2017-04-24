# ds_arena

This is a simple 2D arena shooter game. It is a showcase for the [diesel](https://github.com/amecky/diesel) DirectX 11 renderer.

Here is a screenshot:

![Screenshot](https://github.com/amecky/ds_arena/blob/master/docs/Screenshot.png "Screenshot")

## How to build

There is a Visual Studio 2015 solution file available. You also need to checkout [diesel](https://github.com/amecky/diesel) next
to this project. It means that both projects should be in the same directory.

## A short description about the implementation

The game is a 2D arena shooter. It uses a big Sprite batch buffer to render all objects. All sprites are
combined in one texture. Also the game uses a GPU based particle system. The entire update of the particles
are handled on the GPU. The particlesystem settings are read from a json file. This makes tweaking the
settings a lot easier.

Internally the game uses a StateMachine to manage the different game states. The StateMachine is not like a stack
as it often used by other games. Here every state can be active or inactive. If a state is active the tick and render
methods will be called. The order of updating and rendering is defined how they are added to the StateMachine at
the beginning. 
Also there is an event stream that is passed along to every state during the update phase. Every state
can add events. These events are handled in the main loop. 

Here is the list of states in the particular order and a short description  

### BackgroundState
The background state handles the hex grid and the elastic border. The hex grid highlights the player position on the grid
and also highlights enemy spawning positions. The elastic border is the border around the playfield. When a bullet
hits the border it will swing and change color based on the current distance to the target distance. You can
see it in the screenshot where the border is kind of yellow at some points.

### MainState
The main state will run the actual game. It will spawn enenmies and move the player and bullets.
Also it takes care of collision handling. When the player dies it will send out an event so that
the game over state will be activated.

### PrepareState
This state shows the "Get ready" message. After the defined time it will send an event. The event
is handled in the main loop and will deactivate this state and will start the spawning in the main state. 

### GameOverState
This one show the game over dialog. It will also figure out at which rank the user will be in the highscores.

### HighscoreState
This state shows the 10 best scores and fades between the first 5 and the last 5 entries.

### MainMenuState
The main menu state shows the main menu.

 


