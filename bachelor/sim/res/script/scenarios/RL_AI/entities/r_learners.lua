
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
    NameComponent =  {
     entity_type = "AI Learners"
    },
    LuaComponent = {
        lua_id = 1,
        speed = 50,
        direction = Vec2.new(0, 0),

        reach_goal = true,

        q_brain = ai.QBrain(),

        action = {
            ac = ai_actions.nothing.value,
            loc = Vec2.new(0,0),
            entity = nil,
            type = nil
        },

        state = ai_states.nothing.value,
        pre_state = ai_states.nothing.value

    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    HealthComponent = {
        health = 100.0,
        tickdown_rate = 1.0,
        vital_needs = ETag.Food | ETag.Drink
    },
    NeedComponent = {
        -- Corresponds to entires in the needs.lua file
        required_needs = { needs.hunger, needs.thirst}
    },
}