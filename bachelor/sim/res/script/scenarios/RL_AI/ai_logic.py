def hello():
    return "hello from python module"


import numpy as np
import random as rng
import math
from enum import Enum
from typing import List



#TODO: Get seed from CultSim
#rng.seed("Test")

class AIActions(Enum):
    nothing = 0
    search = 1
    move_to_food = 2
    move_to_water = 3
    eat = 4
    drink = 5

class AIStates(Enum):
    nothing = 0
    searching = 1
    moving_to_food = 2
    moving_to_water = 3
    eating = 4
    drinking = 5


print(AIActions.drink)

class Vec2:
    def __init__(self, x = 0, y=0 ):
        self.x = x
        self.y = y

    def set(self, x, y):
        self.x = x
        self.y = y

class Vec3:
    def __init__(self, x=0, y=0, z=0):
        self.x = x
        self.y = y
        self.z = z

    def set(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

class Command():
    action = 0
    location = Vec2()
    entity = 0
    def __init__(self, action = 0, location = Vec2(), entity = None):
        self.action = action
        self.location = location
        self.entity = entity

class ItemMeta():
    def __init__(self, type, loc, e):
        self.type = type
        self.loc = loc
        self.e = e


class Environment():
    #Takes in lua table as seen of ItemMeta
    def __init__(self, loc: Vec2, state ,seen):
        self.my_loc = loc
        self.state = state
        self.seen = []
        for i in seen:
            self.seen.append(seen[i])


class MoveAction():
    def __init__(self, x = 100, y = 100):
        self.bound_x = x
        self.bound_y = y

    def set_bounds(self, x, y):
        self.bound_x = x
        self.bound_y = y

    def move_random(self):
        x = rng.randint(-self.bound_x, self.bound_x)
        y = rng.randint(-self.bound_y, self.bound_y)
        return Vec2(x,y)


class Action():
    def __init__(self, action, loc : Vec2,item_type = None ,entity = None):
        self.action = action
        self.item_type = item_type
        self.loc = loc
        self.e = entity


class QBrain:
    def __init__(self):
        stuff = 0
        self.Q = np.zeros((len(AIStates), len(AIActions)))

    def get_action(self, env : Environment):
        if rng.uniform(0,1) < 0.05:
            #Doing some Exploring
            if len(env.seen) > 0:
                r_action = rng.randint(0, len(AIActions) - 1)
                if AIActions.search == r_action:
                    return Action(r_action, MoveAction().move_random())
                else:
                    i = rng.randint(0, len(env.seen) - 1)
                    return Action(rng.randint(0, 1), MoveAction().move_random())
            else:
                return Action(AIActions.search, MoveAction().move_random())
        else:
            #Do some Exploits
            action = self.Q[env.state]
            for item in env.seen:
                if(item.type == 2):
                    return Action(AIActions.eat.value, item.loc,item.type, item.e)

        return Action(AIActions.search.value, MoveAction().move_random())
    
    def updateQ(self, state, action, reward, new_state):
        self.Q[state, action] = self.Q[state, action] + self.lr * (reward + self.gamma * np.max(Q[new_state, :]) - self.Q[state, action])
