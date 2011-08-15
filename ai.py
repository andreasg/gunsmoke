from math import *


# input: p1 (x,y)
#        p2 (x,y)
# output: the distance between the two points
def distance(p1, p2):
    x = p1[0] - p2[0]
    y = p1[1] - p2[1]
    return sqrt(pow(x,2) + pow(y, 2))


# input: p1 (x,y)
#        p2 (x,y)
# output: a normalized vector, pointing from p1 to p2
def get_vector(p1, p2):
    dist = distance(p1,p2)
    dx = (1/dist)*(p1[0]-p2[0])
    dy = (1/dist)*(p1[1]-p2[1])

    # negate because world is upside down
    return -dx, -dy


class Assassin:
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.old_dx = 0.0
        self.old_dy = 0.0
        self.tx = 0
        self.ty = 0
        self.bounds_x = 0
        self.bounds_y = 0
        self.state = ""

        # if closer to the target then this, retreat
        self.inner_proximity_limit = 250
        self.outer_proximity_limit = 500

    def update(self, spos, delta, tpos, bounds, state):
        self.x = spos[0]
        self.y = spos[1]
        self.tx = tpos[0]
        self.ty = tpos[1]
        self.old_dx = delta[0]
        self.old_dy = delta[1]
        self.bounds_x = bounds[0]
        self.bounds_y = bounds[1]
        self.state = state

        if (distance(spos, tpos) < self.inner_proximity_limit):
            self.state = "retreat"

        if (self.y > self.bounds_y-100):
            self.state = "avoid_screen"

        if self.state == "search": return self._search() 
        elif self.state == "attack": return self._attack() 
        elif self.state == "retreat": return self._retreat()
        elif self.state == "avoid_screen": return self._avoid_screen()


    def _search(self):
        dx, dy = get_vector((self.x, self.y),(self.tx,self.ty))
        return (dx, dy, "attack")

    def _attack(self):
        p1 = (self.x, self.y)
        p2 = (self.tx, self.ty)
        if (distance(p1, p2) > self.outer_proximity_limit):
            return(self.old_dx, self.old_dy, "search");
        #continue with old vector
        return (self.old_dx, self.old_dy, self.state)
    
    def _retreat(self):
        dx, dy = get_vector((self.x, self.y),(self.tx,self.ty))
        p1 = (self.x, self.y)
        p2 = (self.tx, self.ty)
        if (distance(p1, p2) > self.outer_proximity_limit):
            return (dx, dy, "search");
        else:
            return (-dx, -dy, self.state)

    def _avoid_screen(self):
        if (self.y > self.bounds_y - 400):
            return (0.0, -1.0, self.state);
        else:
            return (0.0, 0.0, "search");
        



class Boss:
    """A generic boss AI, it behaves as follows:
    Default state should be attack. The boss then
    approaches the bottom of the screen until a 
    limit is reached - then it starts strafing 
    across the screen. If it gets too close to the 
    bottom it retreats up and resumes strafing"""


    def __init__(self):
        self.x = 0
        self.y = 0
        self.tx = 0
        self.ty = 0
        self.bounds_x = 0
        self.bounds_y = 0
        self.state = ""

        #configure variables
        #for strafing
        self.left_limit = 10
        self.right_limit = 590

        # when this y is reached, switch to strafe
        self.attack_limit = 100


    def update(self, spos, tpos, bounds, state):
        """core of the statemachine. Redirects depending on state"""
        self.x = spos[0]
        self.y = spos[1]
        self.tx = tpos[0]
        self.ty = tpos[1]
        self.bounds_x = bounds[0]
        self.bounds_y = bounds[1]
        self.state = state

        # return up if we are to close to the bottom
        if (self.y > self.bounds_y-100):
            self.state = "retreat"

        if self.state == "strafing_left": return self._strafing_left()
        if self.state == "strafing_right": return self._strafing_right()
        if self.state == "attack": return self._attack() 
        if self.state == "retreat": return self._retreat()


    def _strafing_left(self):
        if (self.x > self.left_limit):
            return (-1.0, 0.0, self.state);
        else:
            return (1.0, 0.0, "strafing_right")


    def _strafing_right(self):
        if (self.x < self.right_limit):
            return (1.0, 0.0, self.state);
        else:
            return (-1.0, 0.0, "strafing_left")
        

    def _attack(self):
        if (self.y > self.attack_limit):
            return (0.0, 0.0, "strafing_right")
        else:
            return (0.0,1.0, self.state)


    def _retreat(self):
        if (self.y > self.bounds_y - 400):
            return (0.0, -1.0, self.state);
        else:
            return (0.0, 0.0, "attack");
