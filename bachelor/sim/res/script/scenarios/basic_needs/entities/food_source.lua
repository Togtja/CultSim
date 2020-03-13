entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    SpriteComponent = {
        color = Vec3:new(0.30, 0.40, 0.10),
        texture = "sprites/circle.png"
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
