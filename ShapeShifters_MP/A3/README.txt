Grant Bohlmann / grantbohlmann@mines.edu
A3 / The Cabin in the Woods

The program will spawn my hero at the center of a slightly modified version of the Flight Sim city and the hero can move around, and there is an arcball camera that can be used to look around the hero.

W - moves hero forward (based on direction the hero is facing)
S - moves hero backward
A - turns hero to the left
D - turns hero to the right
Space - Zooms in the arcball camera
Space + Shift - Zooms out the arcball camera
Left mouse click and drag - Allows user to move the arcball camera around the hero

The code should compile using a proper CMakeLists.txt file, I don't think there should be any problems there

Bugs: There shouldn't be any program-breaking bugs. There's no collision with the buildings, but the hero will respawn at the center of the world if it reaches the world limit

How Long: ~10 hours

Lab helpful: 8 (you can probably see a lot of correlation between my code and the Lab5 code because I used it as a template) 

Fun: 9.9 (if the Arcball camera was already created and implemented it would've been a 10)

NOTE: looking at the rubric, it says character must use different colors for different parts. The reds on my character look extremely similar (mainly due to the shaders), but the Body and Head have an RGB value of (1,0,0), the Legs and Bag have an RGB value of (0.75,0,0) and the eyes have an RGB value of (0.6,0.6,0.6). Hope that fulfills that requirement on the rubric.

ANOTHER NOTE: The rubric also says, user must be able to zoom in/out by holding Shift + left mouse button and dragging. I implemented it as Space: Zooms in and Space+Shift: Zooms out. Didn't see this part of the rubric until I was finished with everything, and I don't think I can be bothered to go change that (also, I'm running low on time). 
