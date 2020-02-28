-- This is an example entity defnition that represents one "type" of agen in the world
-- It consists of a series of components that will be created at runtime when spawning this entity
-- You always assign the entity to the next_entity variable so the system knows what entity to read
-- when spawning a new one.

next_entity = {
    PositionComponent = {},
    MetaComponent = {
        name = "Food Tree",
        description = "This is a tree that periodically produces food around it."
    },
    SpriteComponent = {
        texture = "sprites/circle.png",
        color = { 0.6, 0.2, 0.1}
    },
    TimerComponent = {
        time_to_complete = random:uniform(5.0, 15.0)
        on_complete = function(e, reg) log.warning("the timer has been reset!") end
    }
}
