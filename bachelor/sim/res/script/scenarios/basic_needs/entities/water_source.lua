entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/well_c.png",
        normal = "sprites/well_n.png",
        rotation = random:uniform(0, 255),
        scale = 20
    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    TimerComponent = {
        time_to_complete = random:uniform(25.0, 120.0),
        number_of_loops = -1,
        on_complete = function(e)
            local my_pos = cultsim.get_component(e, component.position)
            cultsim.spawn_at("water", Vec2:new(random:uniform(my_pos.position.x - 20.0, my_pos.position.x + 20.0),
                                              random:uniform(my_pos.position.y - 20.0, my_pos.position.y + 20.0)))
        end
    }
}
