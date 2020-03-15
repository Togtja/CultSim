entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    SpriteComponent = {
        color = Vec3:new(0.10, 0.20, 0.60),
        texture = "sprites/circle_c.png",
        normal = "sprites/circle_n.png"
    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    TimerComponent = {
        time_to_complete = random:uniform(25.0, 120.0),
        number_of_loops = -1,
        on_complete = function(e)
            local my_pos = cultsim.get_component(e, component.position)
            cultsim.spawn_at("water", Vec2:new(random:uniform(my_pos.position.x - 10.0, my_pos.position.x + 10.0),
                                              random:uniform(my_pos.position.y - 10.0, my_pos.position.y + 10.0)))
        end
    }
}
