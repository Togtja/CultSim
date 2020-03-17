entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x + 20, scenario.bounds.x - 20),
                            random:uniform(-scenario.bounds.y + 20, scenario.bounds.y - 20))
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/bush_c.png",
        normal = "sprites/bush_n.png",
        scale = 50,
        rotation = random:uniform(0, 255),
        layer = 3 -- number from 1-3 (default is 0)
    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    TimerComponent = {
        time_to_complete = random:uniform(50.0, 140.0),
        number_of_loops = -1,
        on_complete = function(e)
            local my_pos = cultsim.get_component(e, component.position)
            cultsim.spawn_at("food", Vec2:new(random:uniform(my_pos.position.x - 10.0, my_pos.position.x + 10.0),
                                              random:uniform(my_pos.position.y - 10.0, my_pos.position.y + 10.0)))
        end
    }
}
