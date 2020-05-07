entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    MovementComponent = {
        speed = 80.0
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/deer_c.png",
        normal = "sprites/deer_n.png",
        scale = 20
    },
    VisionComponent = {
        radius = 40.0,
        fov = 0 -- 0 means no FOV
    },
    TagComponent = {
        tags = ETag.Avoidable | ETag.Creature
    },
    GoalComponent = {
     goals = {goals.eat}
	},
    ActionComponent = {
     actions = {action_sequences.find_food}
	},
    NameComponent = 
    {
     entity_type = "Deer"
	}
}
