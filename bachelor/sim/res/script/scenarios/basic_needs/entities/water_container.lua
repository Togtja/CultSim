entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x + 50, scenario.bounds.x - 50),
                            random:uniform(-scenario.bounds.y + 50, scenario.bounds.y - 50))
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/crate_water_c.png",
        normal = "sprites/crate_n.png",
        scale = random:uniform(38, 50),
        rotation = random:uniform(0, 255),
        layer = 0 -- number from 1-3 (default is 0)
    },
    TagComponent = {
        tags = ETag.Avoidable | ETag.Drink
    },
    InventoryComponent = {
        max_size = 10
    },
    TimerComponent = {
        time_to_complete = random:uniform(50.0, 140.0),
        number_of_loops = -1,
        on_complete = function(e)
            local target = cultsim.spawn("water")
            cultsim.remove_component(target, component.position)
            cultsim.add_to_inventory(e,target)
        end
    },
        NameComponent = {
        entity_type = "Water Crate"
	}
}
