The project has been compiled with VS2015 Community Edition.
It uses features until the C++14 standard and uses the default engine provided.

The structure of the project is using TestGame as the receiver of the updates of the king engine.
It also is the main class holding all the other important classes.

The mouse input is controlled by InputControl and it has a way to register InputReceivers for the 
events of the mouse.

Grid has the main weight of the game logic. It owns the gems and the interaction with them.
I have applied the pimpl idiom here to separate the private part, make only the interface visible in the header
and try to minimize include dependency to improve compile time.

I have chosen a POD struct to represent gem for trying to mantain the size at minimum, 
so the grid could be easier kept in cache when working with it.


I have been working iteratively in the game, focusing my efforts in the fact that each iteration had to be
working with the features implemented.

